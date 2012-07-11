
#ifndef _PTREE_HPP
#define _PTREE_HPP 	1
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "node.hpp"
#include "ztree.hpp"
#include "utils.hpp"
using namespace std;

namespace tree {


enum rotation_type { LEFT, RIGHT };

#define DEBUG 1



template <class T = unsigned int>
class ptree {
	

private:
	node<T>* nodes;		// base dell'array
	bool allocated;
	T size;			// dimensione (numero di nodi)


	T root;			// indice del nodo radice
	T minvalue;
	T maxvalue;
	
	
	// costruttore di copia, crea un albero da un array di nodi gia' presente, 
	// XXX da non usare verso l'esterno! Serve per poter trattare i sottoalberi 
	// di un albero dato, modificando anche l'originale
	ptree ( const ptree& parent, T root, T minvalue, T maxvalue ) {
		this->nodes = parent.nodes;
		this->allocated = false;
		
		this->root = root;
		this->size = maxvalue - minvalue + 1;
		this->maxvalue = maxvalue;
		this->minvalue = minvalue;
		//cout << "this subtree ranges from " << minvalue << " to " << maxvalue << endl;
	}
	
	// costruisce un albero casuale, con n nodi, partendo dall'etichetta start_value, 
	// cominciando a salvare nodi nella posizione begin dell'array
	T build_randomly ( T n, T start_value, node<T>* begin, T father ) {
		if ( n == 0 )
			return EMPTY;
		if ( n == 1 )
			return begin->set( start_value, EMPTY, EMPTY, father );

		// nodi nei due sottoalberi
		T l = random( ) % n;
		T r = n - l - 1;
	
		// l'albero sinistro ha l nodi, e' etichettato a partire da start_value
		T ls = build_randomly ( l, start_value, begin, start_value + l );
		// l'albero destro ha r nodi, e' etichettato a partire da start_value + l + 1
		T rs = build_randomly ( r, start_value + l + 1, begin + l + 1, start_value + l );
		// start_value + l e' usato dalla radice
		return ( begin + l )->set( start_value + l, ls, rs, father );
	}
	
	// funzione di utilita' chiamata dai costruttori
	void init ( ) {
		nodes = NULL;
		root = 0;
		size = 0;
		allocated = false;
	}
	
//	// funzione di utilita', copia x dentro *this
//	void copy ( const ptree& x ) {
//		size = x.size;
//		root = x.root;
//	}
	
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
		} else {
			if ( !l ) return;

			// sinistra invariata, destra cambia e padre cambia
			u->simple_set( l->right(), u->right(), u->left() );
		
			// il nodo che ora e' figlio sinistro di u, ha u come padre
			if ( l->right() ) locate( l->right() )->set_father( value );

			l->simple_set( l->left(), value, f ? f->value() : EMPTY );

			if ( !f || root == value ) root = l->value();
		}

		// nel padre adesso e' cambiato il figlio destro o sinistro,
		// a seconda se u era attaccato come nodo destro o sinistro
		if ( f && f->left()  == value ) f->set_left ( u->father() );
		if ( f && f->right() == value )	f->set_right( u->father() );
	}
public:
	// utile per poter utilizzare il tipo di nodo anche dall'esterno
	typedef T node_type;
	// tipo di utilita', utile per gestire gli intervalli
	typedef pair<T, T> couple;
	
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
		minvalue = 1;
		maxvalue = n;
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

		minvalue = x.minvalue;
		maxvalue = x.maxvalue;
		size = x.size;
		root = x.root;
//		printf( "Tree from [%d, %d]\n", minvalue, maxvalue );
//		printf( "Size = %d, root = %d\n", size, root );

	}
	
	// costruisce un ptree da uno ztree
	ptree ( const ztree& z ) : size( z.vertices() ) {
		nodes = new node<T> [ size + 1 ];
		memset( nodes, 0, sizeof( node<T> ) * ( size + 1 ) );
		allocated = true;
		minvalue = 1;
		maxvalue = z.vertices();
	
		z.get_tree<T>( root, nodes );
	}
	
	// distruttore
	~ptree ( ) {
		if ( allocated ) delete[] nodes;
	}

	// restituisce il numero di nodi
	T vertices ( ) const {
		return size;
	}
	
	ptree<T> left ( ) const {
		return subtree( nodes[root].left() ); 
	}
	
	ptree<T> right ( ) const {
		return subtree( nodes[root].right() ); 
	}
	
	// restituisce true se value e' contenuto nel sottoalbero, false altrimenti
	bool valid ( const T value ) const {
//		return !( value > start + size || value == EMPTY || value < 0 );
		return minvalue <= value && value <= maxvalue && value != EMPTY;
	}
	
	// restituisce in tempo costante il puntatore al nodo con chiave value
	node<T>* locate ( const T value ) const {
		//if ( !valid( value ) ) return NULL;
		// potrei cercare anche un nodo che non e' nel sottoalbero
		// ad esempio il nodo padre del nodo radice!

		return nodes + value;
	}
	
	bool operator == ( const ptree<T>& x ) const {
		bool out = ( x.size == size ) && ( x.root == root )
			   && ( x.minvalue == minvalue) && ( x.maxvalue == maxvalue );
		
		
		for ( T i = minvalue; i <= maxvalue && out; ++i )
			out = out && ( nodes[i] == x.nodes[i] );
			
		return out;
	}

	bool operator != ( const ptree<T>& x ) const {
		return !(*this == x);
	}
	
	bool operator < ( const ptree<T>& another ) const {
		if ( size != another.size ) 
			return size < another.size;
		
		for ( T i = minvalue; i <= maxvalue; ++i ) {
			if ( !( nodes[i] == another.nodes[i] ) )
				return nodes[i] < another.nodes[i];
		}
		
		// sono uguali
		return false;
	}
	
//	// copia x dentro *this, eliminando il contenuto precedente
//	ptree<T>& operator = ( const ptree<T>& x ) {
//		return ptree<T>( x );
//		// caso in cui x == this, non devo deallocare niente.
//		// semplicamente assegno le altre variabili
//		if ( allocated && x.nodes == nodes ) {
//			size = x.size;
//			root = x.root;
//			return *this;
//		}

		
//		// caso in cui x != this e le dimensioni sono diverse, qui devo riallocare
//		if ( allocated && x.size != size ) {
//			delete[] nodes;
//			nodes = new node<T> [ x.size + 1 ];
//			memset( nodes, 0, sizeof( node<T> ) * ( x.size + 1 ) );
//		}
		
//		// infine copio x
//		allocated = true;
//		copy( x );
//		return *this;
//	}
	
	// porta su il nodo value di una posizione
	ptree<T>& operator ^ ( const T value ) {
		up( value );
		return *this;
	}
	
	// stampa l'albero sullo stream dato
	friend ostream& operator << ( ostream& stream, const ptree<T>& t ) {
		print_tree( stream, t.nodes + t.root, t.nodes );
		return stream;
	}
	
	string to_str ( T* eqinfo = NULL ) const {
		ostringstream stream;
		print_tree( stream, this->nodes + this->root, this->nodes, eqinfo );
		return stream.str();
	}
	

	
	// Conversione da ptree a ztree.. assai inefficiente, si puo' migliorare!
	static ztree to_sequence ( const ptree<T>& x ) {
		if ( x.size == 0 ) return ztree( );
		
		return ztree( to_sequence( x.left() ), to_sequence( x.right() ) );
	}
	
	ztree to_sequence ( ) const {
		return to_sequence ( *this );
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

		return ptree<T>( *this, value, left, right );
	}
	
	// ometto i controlli tanto verranno chiamate o da c() o da to_leaf
	T phi ( const T x, T* eqinfo = NULL ) const {
		T total = 0;
		
		// conto i nodi nel sottobraccio di sinistra
		for ( T j = locate( x )->left(); j != EMPTY; j = locate( j )->right() ) {
			if ( eqinfo && eqinfo[j] != EMPTY ) break;
			total++;
		}
		
		return total;
	}
	
	T gamma ( const T x, T* eqinfo = NULL ) const {
		T total = 0;
	
		for ( T j = locate( x )->right(); j != EMPTY; j = locate( j )->left() ) {
			if ( eqinfo && eqinfo[j] != EMPTY ) break;
			total++;
		}
		
		return total;
	}
	
	// calcola il c(x) ovvero |F(x)| + |G(x)| per un determinato nodo x
	// restituisce -1 in caso l'indice non sia valido (non dovrebbe mai verificarsi!)
	// Nell'array eqinfo sono contenute le informazioni a riguardo i nodi omologhi, 
	// se e' != NULL va considerato.
	T c ( const T x, T* eqinfo = NULL ) const {
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
	T r ( const T x, T* eqinfo = NULL ) const {
		if ( ! valid( x ) ) {
			cerr << "c(): invalid value " << x << ".\n";
			return -1;
		}

		T total = 0;
		T j = x;

		// conto i nodi per arrivare alla radice
		do {
			if ( eqinfo && eqinfo[j] != EMPTY ) break;
			if ( locate( j )->father() == EMPTY ) break;
			total++;

			j = locate( j )->father();
		} while ( j != EMPTY );

		return total;
	}
	
	// funzione usata da simplify per portare il nodo x ad essere una foglia, 
	// eseguendo c(x) rotazioni.
	T to_leaf ( const T x, T* eqinfo = NULL ) { 
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
	
	// procedura ricorsiva che calcola tutti gli intervalli e li salva negli array l e r
	// in input viene passato l'intervallo che e' stato propagato dall'alto.
	void intervals ( couple* i, T value, T left, T right ) const {
		if ( value == EMPTY ) return;
		
		// XXX rivedere
		i[value].first = left;
		i[value].second = right;
		
		
		intervals( i, locate( value )->left(), left, value - 1 );
		intervals( i, locate( value )->right(), value + 1, right );
	}

	// L'array eqinfo conterra' le informazioni sui nodi omologhi. 
	// Gli indici rappresentano i nodi in *this, mentre il contenuto l'eventuale nodo in s 
	// che e' omologo (EMPTY altrimenti). XXX considera un solo array, riferito a *this, 
	// se serve anche quello riferito a s, seqinfo allore e' != NULL.
	bool equal_subtrees ( ptree<T>& s, T* eqinfo, T* seqinfo, couple* intt, couple* ints ) {
		memset( eqinfo, EMPTY, ( size + 1 ) * sizeof( T ) );
		memset( seqinfo, EMPTY, ( size + 1 ) * sizeof( T ) );

		memset( intt, EMPTY, ( size + 1 ) * sizeof( couple ) );
		memset( ints, EMPTY, ( size + 1 ) * sizeof( couple ) );
		// questa funzione deve essere chiamata su alberi equivalenti, 
		// ovvero con gli stessi nodi, per cui se le dimensioni sono diverse e 
		// l'indice di partenza e' diverso, esco.
		if ( s.size != size || s.maxvalue != maxvalue || s.minvalue != minvalue ) {
			cerr << "equal_subtrees(): not equivalent trees.\n";
			return false;
		}
		
		// un array grande quanti sono i nodi per contenere tutti gli intervalli
		// l'intervallo del nodo value, viene salvata in posizione value - start.
		//couple intt [ size ];
		//couple ints [ size ];
		// calcola tutti gli intervalli
		intervals( intt, root, minvalue, maxvalue );
		s.intervals( ints, s.root, minvalue, maxvalue );
		
		
		for ( T i = minvalue; i <= maxvalue; ++i ) {
			if ( i == root ) continue;
			
			if ( ints[i] == intt[i] ) {
				eqinfo[i] = i;
				if ( seqinfo ) seqinfo[i] = i;
				continue;
			}
			
			if ( ints[i].first > 1 ) {
				T y = locate( ints[i].first - 1 )->right();
				// puo' succedere che y sia EMPTY!
				if ( y != EMPTY && intt[y] == ints[i] ) {
					eqinfo[y] = i;
					if ( seqinfo ) seqinfo[i] = y;
				}
			}
			
			if ( ints[i].second < size ) {
				T y = locate( ints[i].second + 1 )->left();
				
				if ( y != EMPTY && intt[y] == ints[i] ) {
					eqinfo[y] = i;
					if ( seqinfo ) seqinfo[i] = y;
				}
			}
		}
		
		return true;
	}
	
	// la funzione e' ricorsiva quindi value e' il nodo per cui si testa la non omologia
	// in eqinfo sono presenti le informazioni sui nodi equivalenti per *this, e in seqinfo quelle per s.
	// un prerequisito fondamentale e' che questa funzione venga chiamata su (sotto) alberi equivalenti
	// e che negli array info siano presenti le informazioni sugli altri alberi equivalenti contenuti:
	// in sostanze *this e s devono avere gli stessi nodi NON omologhi. Non faccio controlli a riguardo.
	T min_non_homologue ( ptree<T>& s, T* eqinfo, T* seqinfo, T value, T& cval ) {
		// calcolo il c sul nodo corrente
		cval = c( value, eqinfo ) + s.c( value, seqinfo );

		// XXX alcune volte non mi accorgo che la radice non deve essere spostata (in eqinfo
		// o in seqinfo non trovo che le radici sono equivalenti)
		if ( value == s.root && value == this->root ) cval = 0;
		if ( cval == 0 ) return value;
		
		
		T temp_value = EMPTY, temp_cx = 4, out = value;
		// guardo a sinistra quanto e' il minimo dei c(x)
		T x = locate( value )->left();
		// se il nodo sinistro e' non vuoto e non e' un nodo equivalente, mi calcolo c(x) minimo a sinistra
		if ( x != EMPTY && eqinfo[x] == EMPTY )
			temp_value = min_non_homologue( s, eqinfo, seqinfo, x, temp_cx );
		
		// se tale valore e' minore di quello corrente, salvo i dati
		
		if ( temp_cx < cval ) {
			out = temp_value;
			cval = temp_cx;
		}
	
		// allo stesso modo guardo se ha destra posso migliorare il c(x)
		x = locate( value )->right();
		if ( x != EMPTY && eqinfo[x] == EMPTY )
			temp_value = min_non_homologue( s, eqinfo, seqinfo, x, temp_cx );
		
		if ( temp_cx < cval ) {
			out = temp_value;
			cval = temp_cx;
		}
		
		return out;
	}
	
	// processing di SIMPLIFY: rende identici s e *this, rendendo foglia mano mano i nodi che restano.
	// se non_trivial != NULL viene 1 se sono state fatte scelte non banali (con c(x) > 1)
	T process ( ptree<T>& s, T* eqinfo, T* seqinfo, bool* non_trivial = NULL ) {
		T total = 0;
		
		
		T selected = EMPTY, cx;
		do {
			selected = min_non_homologue( s, eqinfo, seqinfo, root, cx );
			if ( selected == root && cx == 0 ) break;
			//printf( "Selected node %d with c = %d.\n", selected, cx );
			
			if ( cx > 3 ) {
				cerr << "process(): selected node x = " << selected << ", with c(x) > 3.\n";
				return -1;
			}
			
			
			
			if ( cx > 1 &&  non_trivial ) // non e' corretto, ma e' solo per avere un indicazione..
				*non_trivial = true;
			
			//cout << *this << endl;
			total += to_leaf( selected, eqinfo );
			total += s.to_leaf( selected, seqinfo );

//			cout << *this << endl;
			
			// aggiorno eqinfo e seqinfo, il nodo selected adesso e' equivalente. Potrebbero
			// esserci anche altri nodi equivalenti, ma verranno determinati dalla
			// min_non_homologue(), ovvero verranno selezionati per primi gli eventuali nodi x
			// con c(x) pari a 0
			eqinfo[selected] = selected;
			seqinfo[selected] = selected;
		} while ( true );
		
		// le radici non sono state settate come equivalenti
		eqinfo[root] = root;
		seqinfo[root] = root;
		return total;
	}
	
	// l'algoritmo SIMPLIFY
	T simplify ( ptree<T>& s, bool* non_trivial = NULL ) {
		if ( s.minvalue != minvalue || s.maxvalue != maxvalue || s.size != size ) {
			cerr << "Not compatible trees in simplify()\n";
			return 0;
		}

		T total = 0;
		T eqinfo [ size + 1 ];
		T seqinfo [ size + 1 ];	// serve per calcolare il c(x) indipendentemente anche sul secondo albero
		
		couple ints [ size + 1 ];
		couple intt [ size + 1 ];

		// 1. Preprocessing, cerca eventuali sottoalberi gia' equivalenti
		equal_subtrees( s, eqinfo, seqinfo, intt, ints );
		
		// se mi interessa sapere quante volte il cx > 1, setto non_trivial a false
		if ( non_trivial ) *non_trivial = false;
		
		// 2. Su ogni sottoalbero equivalente esegue il processing
		for ( T i = minvalue; i <= maxvalue; ++i ) {
			if ( eqinfo[i] == EMPTY ) continue;
//			cout << "sottoalbero " << i << endl;
//			cout << "equivalente a " << eqinfo[i] << endl;
			
			// per ogni coppia di nodi equivalenti prendo i sottoalberi
			ptree<T> tt( subtree( i ), false );
			ptree<T> ss( s.subtree( eqinfo[i] ), false );
			

//			cout << *this << endl;
//			cout << s << endl;

			// eseguo il processing
			total += tt.process( ss, eqinfo, seqinfo, non_trivial );

//			cout << tt.nodes[tt.root] << endl;
//			cout << this->nodes[tt.root] << endl;

//			cout << *this << endl;
//			cout << s << endl;

		}
		
		// 3. Sui nodi non omologhi rimasti nell'albero corrente, esegue il processing
		total += process( s, eqinfo, seqinfo, non_trivial );
		
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
};

}

#endif

