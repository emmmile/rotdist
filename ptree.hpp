
#ifndef _PTREE_HPP
#define _PTREE_HPP 	1
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "print.hpp"
#include "node.hpp"
#include "ztree.hpp"
#include "node_set.hpp"
#include "equivalence.hpp"
using namespace std;

namespace tree {


enum rotation_type { LEFT, RIGHT };
template <class T> class print;


template <class T = unsigned int>
class ptree {
protected:
  node<T>* nodes;
  bool allocated;   // is the array allocated by this instance?
  T size;           // number of nodes
  T root;           // the value of the root node, nodes[root]


	
	// costruttore di copia, crea un albero da un array di nodi gia' presente, 
  // Serve per poter trattare i sottoalberi di un albero dato, modificando anche l'originale
  ptree ( const ptree& parent, T root ) {
		this->nodes = parent.nodes;
		this->allocated = false;
    this->root = root;
    this->size = nodes[root].maxvalue - nodes[root].minvalue + 1;
	}
	
	// costruisce un albero casuale, con n nodi, partendo dall'etichetta start_value, 
	// cominciando a salvare nodi nella posizione begin dell'array
	T build_randomly ( T n, T start_value, node<T>* begin, T father ) {
		if ( n == 0 )
			return EMPTY;
    if ( n == 1 )
      return begin->set( start_value, EMPTY, EMPTY, father, start_value, start_value );

		// nodi nei due sottoalberi
		T l = random( ) % n;
		T r = n - l - 1;
    T minvalue = start_value;
    T maxvalue = start_value + n - 1;
	
		// l'albero sinistro ha l nodi, e' etichettato a partire da start_value
		T ls = build_randomly ( l, start_value, begin, start_value + l );
		// l'albero destro ha r nodi, e' etichettato a partire da start_value + l + 1
		T rs = build_randomly ( r, start_value + l + 1, begin + l + 1, start_value + l );
		// start_value + l e' usato dalla radice
    return ( begin + l )->set( start_value + l, ls, rs, father, minvalue, maxvalue );
	}
	
	// funzione di utilita' chiamata dai costruttori
	void init ( ) {
		nodes = NULL;
		root = 0;
		size = 0;
		allocated = false;
	}

	// effettua una rotazione di un nodo a sinistra o a destra
	void rotate ( T value, rotation_type type ) {
		if ( ! valid( value ) ) return;
		node<T>* u = locate( value );
		node<T>* f = locate( u->father() );
		node<T>* l = locate( u->left() );
		node<T>* r = locate( u->right() );
//		printf( "%d %d %d %d\n", value, u->father(), u->left(), u->right() );
//		printf( "%p %p %p %p\n", u, f, l, r );

		if ( type == LEFT ) {
			// se non ho niente a destra non posso ruotare a sinistra, esco subito
			if ( !r ) return;

			// sinistra invariata, destra cambia e padre cambia
			u->simple_set( u->left(), r->left(), u->right() );
	
			if ( r->left() ) locate( r->left() )->set_father( value );

			r->simple_set( value, r->right(), f ? f->value() : EMPTY );

			// la rotazione ha portato a radice un nuovo nodo
			if ( !f || root == value ) root = r->value();

      // per preservare gli intervalli
      if( u->right() ) u->maxvalue = locate( u->right() )->maxvalue;
      else u->maxvalue = value;
      r->minvalue = u->minvalue;
		} else {
			if ( !l ) return;

			// sinistra invariata, destra cambia e padre cambia
			u->simple_set( l->right(), u->right(), u->left() );
		
			// il nodo che ora e' figlio sinistro di u, ha u come padre
			if ( l->right() ) locate( l->right() )->set_father( value );

			l->simple_set( l->left(), value, f ? f->value() : EMPTY );

			if ( !f || root == value ) root = l->value();

      // il minimo dell'intervallo del nodo u potrebbe essere cambiato
      if( u->left() ) u->minvalue = locate( u->left() )->minvalue;
      else u->minvalue = value;
      l->maxvalue = u->maxvalue;
		}

		// nel padre adesso e' cambiato il figlio destro o sinistro,
		// a seconda se u era attaccato come nodo destro o sinistro
		if ( f && f->left()  == value ) f->set_left ( u->father() );
		if ( f && f->right() == value )	f->set_right( u->father() );
  }

public:
	// utile per poter utilizzare il tipo di nodo anche dall'esterno
  typedef T node_type;
	
	// costruttore di default
	ptree ( ) {
		init( );
	}

	// costruttore che genera un albero random con n nodi
	ptree ( T n ) : size( n ) {
		nodes = new node<T> [ n + 1 ];
		memset( nodes, 0, sizeof( node<T> ) * ( n + 1 ) );
		root = build_randomly( n, 1, nodes + 1, 0 );
    allocated = true;
	}
	
	// costruttore di copia, copia another dentro this
	ptree ( const ptree<T>& x, bool copyData = true ) {
		if ( copyData == true ) {
			nodes = new node<T> [ x.size + 1 ];
			//memset( nodes, 0, sizeof( node<T> ) * ( x.size + 1 ) );
			//memcpy( nodes, x.nodes, ( size + 1 ) * sizeof( node<T> ) );
			fill( nodes, nodes + x.size + 1, node<T>() );
			copy( x.nodes, x.nodes + size + 1, nodes );
			allocated = true;
		} else {
			nodes = x.nodes;
			allocated = false;
		}

		size = x.size;
    root = x.root;
  }
	
	// costruisce un ptree da uno ztree
	ptree ( const ztree& z ) : size( z.vertices() ) {
		nodes = new node<T> [ size + 1 ];
		memset( nodes, 0, sizeof( node<T> ) * ( size + 1 ) );
    allocated = true;
	
		z.get_tree<T>( root, nodes );
	}
	
	// distruttore
	~ptree ( ) {
		if ( allocated ) delete[] nodes;
	}

	// restituisce true se value e' contenuto nel sottoalbero, false altrimenti
	bool valid ( const T value ) const {
//		return !( value > start + size || value == EMPTY || value < 0 );
    //return minvalue <= value && value <= maxvalue && value != EMPTY;
    return true;
	}
	
	// restituisce in tempo costante il puntatore al nodo con chiave value
	node<T>* locate ( const T value ) const {
		//if ( !valid( value ) ) return NULL;
		// potrei cercare anche un nodo che non e' nel sottoalbero
		// ad esempio il nodo padre del nodo radice!

		return nodes + value;
	}
	
	bool operator == ( const ptree<T>& x ) const {
    bool out = ( x.size == size ) && ( x.root == root );
		
		
    for ( T i = nodes[root].minvalue; i <= nodes[root].maxvalue && out; ++i )
			out = out && ( nodes[i] == x.nodes[i] );
			
		return out;
	}

	bool operator != ( const ptree<T>& x ) const {
		return !(*this == x);
	}
	
	bool operator < ( const ptree<T>& another ) const {
		if ( size != another.size ) 
			return size < another.size;
		
    for ( T i = nodes[root].minvalue; i <= nodes[root].maxvalue; ++i ) {
			if ( !( nodes[i] == another.nodes[i] ) )
				return nodes[i] < another.nodes[i];
		}
		
		// sono uguali
		return false;
  }


	// stampa l'albero sullo stream dato
  friend ostream& operator << ( ostream& stream, const ptree<T>& t ) {
    print_tree( stream, t.nodes + t.root, t.nodes );
    //print<T>( t ).draw( stream );
		return stream;
  }

  string to_str ( equivalence_info<T>* eqinfo = NULL ) const {
    ostringstream stream;
    print_tree( stream, this->nodes + this->root, this->nodes, eqinfo );
    return stream.str();
  }

	
	
	// porta il nodo value di una rotazione verso l'alto
	bool up ( const T value ) {
		node<T>* u = locate( value );
		if ( u == NULL ) return false;
		if ( u->father() == EMPTY ) return false;
		
		
		node<T>* f = locate( u->father() );
		// se il nodo da portare su sposta la radice, la nuova radice e' il nodo corrente
		if ( u->father() == root )
			root = value;
	
		if ( f->left() == value ) 
			rotate( u->father(), RIGHT );
		else 	rotate( u->father(), LEFT );
		
		return true;
	}

	
	// restituisce il sottoalbero radicato in value. E' necessario calcolare l'offset del 
	// sottoalbero nell'array, ovvero da dove inizia, e il numero di nodi. 
	// XXX Il sottoalbero generato e' a tutti gli effetti un ptree ma i suoi nodi sono in 
	// comune con quelli dell'albero di origine, per cui attenzione a non confondersi.
	ptree<T> subtree ( const T value ) const {
		// in caso di errore restituisco un albero vuoto
		if ( !valid( value ) ) return ptree<T>( );
		
		T left = value;
		// altrimenti determino l'intervallo e la dimensione del sottoalbero, 
		// prima vado a sinistra il piu' possibile
		while ( locate( left )->left() != EMPTY ) left = locate( left )->left();
		
		
		T right = value;
		// poi vado a destra il piu' possibile e determino l'intervallo in O(n)
		while ( locate( right )->right() != EMPTY ) right = locate( right )->right();

    return ptree<T>( *this, value );
	}
	
	// ometto i controlli tanto verranno chiamate o da c() o da to_leaf
  T phi ( const T x, equivalence_info<T>& eqinfo ) const {
		T total = 0;
		
		// conto i nodi nel sottobraccio di sinistra
		for ( T j = locate( x )->left(); j != EMPTY; j = locate( j )->right() ) {
      if ( eqinfo[j] != EMPTY ) break;
			total++;
		}
		
		return total;
	}
	
  T gamma ( const T x, equivalence_info<T>& eqinfo ) const {
		T total = 0;
	
		for ( T j = locate( x )->right(); j != EMPTY; j = locate( j )->left() ) {
      if ( eqinfo[j] != EMPTY ) break;
			total++;
		}
		
		return total;
	}
	
	// calcola il c(x) ovvero |F(x)| + |G(x)| per un determinato nodo x
	// restituisce -1 in caso l'indice non sia valido (non dovrebbe mai verificarsi!)
	// Nell'array eqinfo sono contenute le informazioni a riguardo i nodi omologhi, 
	// se e' != NULL va considerato.
  T c ( const T x, equivalence_info<T>& eqinfo ) const {
		if ( ! valid( x ) ) {
			cerr << "c(): invalid value " << x << ".\n";
			return -1;
		}
		
		T total = 0;
		
		// conto i nodi nel sottobraccio di sinistra
		total += phi( x, eqinfo );
			
		// conto i nodi nel sottobraccio di destra
		total += gamma( x, eqinfo );
		
		return total;
	}

	// calcola r(x), la distanza dalla radice
  T r ( const T x, equivalence_info<T>& eqinfo ) const {
		if ( ! valid( x ) ) {
      cerr << "r(): invalid value " << x << ".\n";
			return -1;
		}

		T total = 0;
		T j = x;

		// conto i nodi per arrivare alla radice
		do {
      if ( eqinfo[j] != EMPTY ) break;
			if ( locate( j )->father() == EMPTY ) break;
			total++;

			j = locate( j )->father();
		} while ( j != EMPTY );

		return total;
	}
	
	// funzione usata da simplify per portare il nodo x ad essere una foglia, 
	// eseguendo c(x) rotazioni.
  T to_leaf ( const T x, equivalence_info<T>& eqinfo ) {
		if ( ! valid( x ) ) {
			cerr << "to_leaf(): invalid value " << x << ".\n";
			return -1;
		}
		
		T total = 0;
		
		while ( phi( x, eqinfo ) != 0 ) {
			rotate( x, RIGHT );
			total++;
		}
		
		while ( gamma( x, eqinfo ) != 0 ) {
			rotate( x, LEFT );
			total++;
		}

		return total;
	}


  bool sameInterval ( node<T>& a, node<T>& b ) {
    return a.minvalue == b.minvalue && a.maxvalue == b.maxvalue;
  }

  // L'array eqinfo conterra' le informazioni sui nodi omologhi.
  // Gli indici rappresentano i nodi in *this, mentre il contenuto l'eventuale nodo in s
  // che e' omologo (EMPTY altrimenti). XXX considera un solo array, riferito a *this,
  // se serve anche quello riferito a s, seqinfo allore e' != NULL.
  void equal_subtrees ( ptree<T>& s, equivalence_info<T>& eqinfo ) {
    if ( s.size != size ) {
      cerr << "equal_subtrees(): not equivalent trees.\n";
      return;
    }

    for ( T i = nodes[root].minvalue; i <= nodes[root].maxvalue; ++i ) {
      if ( i == root ) continue;
			
      if ( sameInterval( nodes[i], s.nodes[i] ) ) {
        eqinfo.set(i, i);
        continue;
      }
			
      if ( s.nodes[i].minvalue > 1 ) {
        T y = locate( s.nodes[i].minvalue - 1 )->right();
        // puo' succedere che y sia EMPTY!
        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) )
          eqinfo.set(y,i);
      }
			
      if ( s.nodes[i].maxvalue < size ) {
        T y = locate( s.nodes[i].maxvalue + 1 )->left();
				
        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) )
          eqinfo.set(y,i);
      }
    }
  }
	
  // cerca il nodo con il cx piu' piccolo fra this e s.
  // cval     the minimal cx found
  // selected the correspondent node
  void min_cx ( ptree<T>& s, equivalence_info<T>& eqinfo, T value, T& cval, T& selected ) {
    // compute c for the current node
    T current = c( value, eqinfo ) + s.c( value, eqinfo.inverse() );
    if ( current < cval ) {
      selected = value;
      cval = current;
    }

    // recursion
    T l = locate( value )->left();
    T r = locate( value )->right();

    if ( l && eqinfo[l] == EMPTY )
      min_cx( s, eqinfo, l, cval, selected );

    if ( r && eqinfo[r] == EMPTY )
      min_cx( s, eqinfo, r, cval, selected );
  }

  // SIMPLIFY processing, take the node with minimum c (not already equivalent),
  // and make it leaf, until there are such nodes
  T process ( ptree<T>& s, equivalence_info<T>& eqinfo ) {
    T total = 0;

    do {
      T selected, cx = 4; // there is always a node with cx <= 3
      min_cx( s, eqinfo, root, cx, selected );

//      printf( "Selected node %d with c = %d.\n", selected, cx );
//      if ( cx > 3 ) {
//        cerr << "process(): selected node x = " << selected << ", with c(x) > 3.\n";
//        return -1;
//      }

      if ( cx == 0 ) break;

      total += to_leaf( selected, eqinfo );
      total += s.to_leaf( selected, eqinfo.inverse() );

      // possibly there are new equivalent nodes (if we are lucky)
      // notice that the root is not set as equivalent, so you need to call
      // this method from the parent (where there is the bigger tree)
      equal_subtrees( s, eqinfo );
    } while ( true );

    return total;
  }
	

  // the SIMPLIFY algorithm
  T simplify ( ptree<T>& s ) {
    if ( s.size != size ) {
      cerr << "Not compatible trees in simplify()\n";
      return 0;
    }

    T total = 0;
    equivalence_info<T> eqinfo( size );

    // 1. Preprocessing, found the equivalent subtrees
    equal_subtrees( s, eqinfo );
    node_set<T> equivalent( eqinfo );

    // 2. On every equivalent subtree it executes the processing
    for ( typename node_set<T>::iterator i = equivalent.begin(); i < equivalent.end(); ++i ) {
      ptree<T> tt( subtree( *i ), false );
      ptree<T> ss( s.subtree( eqinfo[*i] ), false );

      total += tt.process( ss, eqinfo );

      // and updates the equivalence informations
      equal_subtrees( s, eqinfo );
      equivalent.update( eqinfo );
    }

    // 3. On the remaining nodes executes the same processing
    total += process( s, eqinfo );
		
    return total;
  }



	
//	// porta il sottolbero radicato in x in una catena sinistra o destra a seconda di type
//	// (algoritmi LEFT e RIGHT). Restituisce -1 in caso di errore, oppure 0 se l'albero e' nullo
//	T list ( T i, rotation_type type = LEFT, T* eqinfo = NULL ) {
//		if ( i == EMPTY ) return 0;
		
//		if ( ! valid( i ) ) {
//			cerr << "list(): invalid value " << i << ".\n";
//			return -1;
//		}
		
//		T total = 0;
//		while ( i != EMPTY ) {
//			T next = i;
			
//			T j = i;
//			while ( ( type == RIGHT && phi( j, eqinfo ) != 0 ) ||
//				( type == LEFT && gamma( j, eqinfo ) != 0 ) ) {
//				next = ( type == LEFT ) ? locate( j )->right() : locate( j )->left();
				
//				rotate( j, type );
//				total++;
				
				
//				j = next;
//			}
			
//			// l'ultimo nodo che ho ruotato (oppure i invariato) e' quello da cui devo scendere
//			i = ( type == LEFT ) ? locate( next )->left() : locate( next )->right();
//		}
		
//		return total;
//	}
	
//	// processing basato sull'algoritmo CENTRAL
//	T central ( ptree<T>& z, T* eqinfo = NULL, T* seqinfo = NULL ) {
//		if ( z.size != size ) {
//			cerr << "central(): invalid subtrees.\n" << *this << endl << z << endl;
//			return -1;
//		}
		
//		T total = 0;
		
//		// cerco il nodo con c(x) massimo
//		T selected = EMPTY, max;
//		selected = max_non_homologue( z, eqinfo, seqinfo, root, max );
		
//#if DEBUG
//		printf( "CENTRAL: Selected node %d with c = %d\n", selected, max );
//#endif
//		if ( max == 0 )
//			return total;
		
//		// porto il nodo selezionato alla radice in entrambi gli alberi
//		total +=   to_root( selected );
//		total += z.to_root( selected );
		
//		// applico gli algoritmi left e right ai sottoalberi
//		total +=   list(   locate( selected )->right( ), LEFT, eqinfo );
//		total += z.list( z.locate( selected )->right( ), LEFT, seqinfo );
//		total +=   list(   locate( selected )->left( ), RIGHT, eqinfo );
//		total += z.list( z.locate( selected )->left( ), RIGHT, seqinfo );
		
//		return total;
//	}
	
//	// algoritmo CENTRAL con preprocessing
//	int central_preprocessing ( ptree<T>& s ) {
//		int total = 0;
//		T eqinfo [ size ];
//		T seqinfo [ size ];

//		couple ints [ size ];
//		couple intt [ size ];
		
//		equal_subtrees( s, eqinfo, seqinfo, intt, ints );
//		// Su ogni sottoalbero equivalente esegue il processing basato su central
//		for ( T i = 0; i < size; ++i ) {
//			if ( eqinfo[i] == EMPTY ) continue;
			
//			// per ogni coppia di nodi equivalenti prendo i sottoalberi
//			ptree<T> tt = subtree( i + start );
//			ptree<T> ss = s.subtree( eqinfo[ i ] );
			

//			//cout << to_string( tt, ss, eqinfo, seqinfo ) << endl;
//			//for ( int i = 1; i <= size; ++i )
//			//	printf("node %d = %d / %d\n", i, eqinfo[i-1], seqinfo[i-1]);

//			// eseguo il processing
//			total += tt.central( ss, eqinfo, seqinfo );
//			//printf( "CENTRAL: eseguite %d rotazioni.\n", total );
			
//			// e ricalcolo i nodi equivalenti, che non vengono auto-riconosciuti come in simplify
//			equal_subtrees( s, eqinfo, seqinfo, intt, ints );
//		}

//		//cout << to_string( *this, s, eqinfo, seqinfo ) << endl;
//		total += central( s, eqinfo, seqinfo );
//		//printf( "CENTRAL: eseguite %d rotazioni.\n", total );
//		return total;
//	}





////	NUOVA PARTE

//	T max_non_homologue ( ptree<T>& s, T* eqinfo, T* seqinfo, T value, T& cval ) {
//		// calcolo il c sul nodo corrente
//		cval = c( value, eqinfo ) + s.c( value, seqinfo );

//		// XXX alcune volte non mi accorgo che la radice non deve essere spostata (in eqinfo
//		// o in seqinfo non trovo che le radici sono equivalenti)
//		if ( value == s.root && value == this->root ) cval = 0;
//		if ( cval == 0 ) return value;


//		T temp_value = EMPTY, temp_cx = cval, out = value;
//		// guardo a sinistra quanto e' il massimo dei c(x)
//		T x = locate( value )->left();
//		// se il nodo sinistro e' non vuoto e non e' un nodo equivalente, mi calcolo c(x) massimo a sinistra
//		if ( x != EMPTY && eqinfo[x - 1] == EMPTY )
//			temp_value = max_non_homologue( s, eqinfo, seqinfo, x, temp_cx );

//		if ( temp_cx > cval ) {
//			out = temp_value;
//			cval = temp_cx;
//		}

//		// allo stesso modo guardo se ha destra posso migliorare il c(x)
//		x = locate( value )->right();
//		if ( x != EMPTY && eqinfo[x - 1] == EMPTY )
//			temp_value = max_non_homologue( s, eqinfo, seqinfo, x, temp_cx );

//		if ( temp_cx > cval ) {
//			out = temp_value;
//			cval = temp_cx;
//		}

//		return out;
//	}

//	// porta un nodo alla radice
//	T to_root ( const T value, T* eqinfo = NULL ) {
//		//cout << "to_root" << endl;
//		T rotations = 0;
//		bool stop = false;

//		if ( eqinfo && eqinfo[value-start] != EMPTY )
//			return 0;

//		while ( root != value && !stop ) {
//			T on =  locate(value)->father();
//			if ( eqinfo && on && eqinfo[on-start] !=EMPTY )
//				stop = true;
//			up( value );
//			rotations++;
//		}

//		return rotations;
//	}

//	// step c(x) = 1 di MIX: porta a foglia tutti i nodi con c(x) = 1
//	T remove_c1 ( ptree<T>& s, T* eqinfo, T* seqinfo ) {
//		T total = 0;


//		T selected = EMPTY, cx;
//		do {
//			selected = min_non_homologue( s, eqinfo, seqinfo, root, cx );
//			if ( selected == root && cx == 0 ) break;

//			if ( cx > 1 ) break;
//#if DEBUG
//			printf( "Selected node %d with c = %d.\n", selected, cx );
//#endif
//			total += to_leaf( selected, eqinfo );
//			total += s.to_leaf( selected, seqinfo );

//			eqinfo[selected - 1] = selected;
//			seqinfo[selected - 1] = selected;
//		} while ( true );

//		return total;
//	}

//	// TODO: questa e' O(n^2), si puo' fare tranquillamente in O(n)
//	void calculate_r ( ptree<T>& s, T* eqinfo, T* seqinfo, T* rxs ) {
//		memset( rxs, EMPTY, (size + 1) * sizeof( T ) );

//		for ( T i = 1; i <= size; ++i ) {
//			// provo a risalire ogni volta il piu' possibile, costoso ma semplice
//			rxs[i] = this->r( i, eqinfo ) + s.r( i, seqinfo );
//			//printf ("%d: %d %d\n", i, this->r( i, eqinfo ), s.r( i, seqinfo ) );
//		}
//	}


//	void calculate_intervals_r ( T value, T* eqinfo, couple* intt, T* intervalIDs ) {
//		if ( value == EMPTY ) return;

//		T l = locate( value )->left();
//		T r = locate( value )->right();

//		// posso andare normalmente a sinistra, eredito le informazioni sull'intervallo
//		if ( l != EMPTY && eqinfo[l-start] == EMPTY )
//			intervalIDs[l] = intervalIDs[value];

//		// posso andare normalmente a destra, eredito le informazioni sull'intervallo
//		if ( r != EMPTY && eqinfo[r-start] == EMPTY )
//			intervalIDs[r] = intervalIDs[value];

//		// non posso andare a sinistra, NON eredito le informazioni sull'intervallo
//		if ( l != EMPTY && eqinfo[l-start] != EMPTY )
//			if ( intt[intervalIDs[l]-start].first == intt[intervalIDs[value]-start].first )
//				intt[intervalIDs[value]-start].first = intt[intervalIDs[l]-start].second + 1;

//		if ( r != EMPTY && eqinfo[r-start] != EMPTY )
//			if ( intt[intervalIDs[r]-start].second == intt[intervalIDs[value]-start].second )
//				intt[intervalIDs[value]-start].second = intt[intervalIDs[r]-start].first - 1;

//		calculate_intervals_r( l, eqinfo, intt, intervalIDs );
//		calculate_intervals_r( r, eqinfo, intt, intervalIDs );
//	}


//	void calculate_intervals ( T* eqinfo, couple* intt, T* intervalIDs ) {
//		for ( T i = 0; i <= size; ++i ) { intervalIDs[i] = i; }

//		calculate_intervals_r ( root, eqinfo, intt, intervalIDs );
//	}

//	// l'algoritmo MIX
//	T mix ( ptree<T>& s ) {
//		T total = 0;
//		T eqinfo [ size ];
//		T seqinfo [ size ];	// serve per calcolare il c(x) indipendentemente anche sul secondo albero
//		T rxs [ size + 1 ];
//		T selected;

//#if DEBUG
//		T tmp = 0;
//#endif

//		couple intt [ size ];
//		couple ints [ size ];
//		T intervalIDs [ size + 1];

//		do {
//			//T removed = total;
//			equal_subtrees( s, eqinfo, seqinfo, intt, ints );
//			for ( T i = 0; i < size; ++i ) {
//				if ( eqinfo[i] == EMPTY ) continue;

//				ptree<T> tt = subtree( i + start );
//				ptree<T> ss = s.subtree( eqinfo[ i ] );

//				total += tt.remove_c1( ss, eqinfo, seqinfo );
//			}
//			//cout << to_string( *this, s, eqinfo, seqinfo );
//			total += remove_c1( s, eqinfo, seqinfo );
//			//removed = total - removed;

//#if DEBUG
//			printf( "Primo passo: eseguite %d rotazioni.\n", total - tmp);
//			cout << to_string( *this, s, eqinfo, seqinfo );
//			tmp = total;
//#endif


//			equal_subtrees( s, eqinfo, seqinfo, intt, ints );
//			calculate_r( s, eqinfo, seqinfo, rxs );
//			calculate_intervals( eqinfo, intt, intervalIDs );

//#if DEBUG
//			cout << to_string<T>( *this, s, eqinfo, seqinfo ) << endl;

//			/*for ( T i = start; i < start + size; ++i )
//				printf ("Nodo %d: (%d, %d)\n", i, intt[intervalIDs[i]-start].first,
//					intt[intervalIDs[i]-start].second);

//			getchar();*/
//#endif

//			selected = EMPTY;
//			// prendo quello con r(x) minimo
//			for ( T i = 1; i <= size; ++i ) {
//				//cout << "r(" << (*it).first << ") = " << (*it).second << endl;

//				if ( rxs[i] == 1 || ( ( rxs[i] == 2 || rxs[i] == 3 ) &&
//				     i > intt[intervalIDs[i]-start].first && i < intt[intervalIDs[i]-start].second ) ) {
//					if ( !selected || ( selected && rxs[i] < rxs[selected] ) )
//						selected = i;
//				}
//			}

//#if DEBUG
//			cout << "Selected node " << selected  << " with r = " << rxs[selected] << endl;
//#endif

//			if ( selected != EMPTY ) {
//				T security = 0;
//				security += to_root( selected, eqinfo );
//				security += s.to_root( selected, seqinfo );

//				if ( security != rxs[selected] ) {
//					cerr << "invalid to_root().\n";
//					return -1;
//				}

//				total += rxs[selected];
//			}
//#if DEBUG
//			printf( "Secondo passo: eseguite %d rotazioni.\n", total - tmp );
//			cout << to_string( *this, s, eqinfo, seqinfo );
//			tmp = total;
//#endif
//		} while ( selected != EMPTY /*|| removed != 0*/ );

//		//terzo step: eseguo central sugli alberi equivalenti
//		T lastStep = central_preprocessing( s );
//#if DEBUG
//		printf( "Terzo passo: eseguite %d rotazioni.\n", lastStep );
//		cout << to_string( *this, s, eqinfo, seqinfo );
//#endif
//		return total + lastStep;
//	}

//	T mixed ( const ptree<T>& a ) const {
//		ptree<T> x = *this;
//		ptree<T> y = a;
//		return x.mix( y );
//	}


  void testIntervals ( int n ) {
    for( int i = 0; i < n; ++i ) {
        up( random() % (size + 1) );

        cout << *this << endl;

        for( int i = 1; i <= size; ++i ) {
            cout << nodes[i] << endl;
        }

        getchar();
    }
  }


};



template <class T>
class print : public ptree<T> {
  int xmin;
  int xmax;
  int ymax;
  int* edges;
  char* buffer;
  int bufferSize;
  int rowlen;

  void findBoundaries ( T value, int x, int y ) {
    if ( x < xmin ) xmin = x;
    if ( x > xmax ) xmax = x;
    if ( y > ymax ) ymax = y;

    T l = this->nodes[value].left();
    T r = this->nodes[value].right();

    if ( l ) findBoundaries( l, x - edges[l] - 1, y + edges[l] + 1 );
    if ( r ) findBoundaries( r, x + edges[r] + 1, y + edges[r] + 1 );
  }

  void printTree ( T value, int x, int y, bool left ) {
    T l = this->nodes[value].left();
    T r = this->nodes[value].right();

    char numBuffer [10];
    sprintf( numBuffer, "%d", value );
    //this works until value < 100
    int correction = strlen( numBuffer ) > 1 ? !left : 0;
    copy( numBuffer, numBuffer + strlen(numBuffer),
          buffer + x + rowlen * y - correction );

    if ( l ) {
      for ( int i = 0; i < edges[l]; ++i )
        buffer[(x - i - 1) + rowlen * (y + i + 1)] = '/';

      printTree( l, x - edges[l] - 1, y + edges[l] + 1, true );
    }
    if ( r ) {
      for ( int i = 0; i < edges[r]; ++i )
         buffer[(x + i + 1) + rowlen * (y + i + 1)] = '\\';

      printTree( r, x + edges[r] + 1, y + edges[r] + 1, false );
    }
  }


public:
  print( const ptree<T>& t ) : ptree<T>( t, false ) {
    edges = new int [this->size + 1];
    xmin = xmax = ymax = 0;
    fill( edges, edges + this->size + 1, 1 );
  }

  ~print( ) {
    delete[] edges;
    delete[] buffer;
  }

  void draw ( ostream& stream ) {
    findBoundaries( this->root, 0, 0 );

    rowlen = xmax - xmin + 2; // +1 also for the \n
    bufferSize = rowlen * (ymax + 1);
    buffer = new char [bufferSize];
    fill( buffer, buffer + bufferSize, ' ' );

    for ( int y = 1; y <= ymax; ++y )
      buffer[rowlen * y - 1] = '\n';
      buffer[bufferSize - 1] = '\0';

    printTree( this->root, -xmin, 0, true );
    stream << buffer;
  }
};



}

#endif

