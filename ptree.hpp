
#ifndef _PTREE_HPP
#define _PTREE_HPP   1
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "print.hpp"
#include "node.hpp"
#include "ztree.hpp"
#include "simple_set.hpp"
#include "equivalence.hpp"
using namespace std;

namespace tree {


enum rotation_type { LEFT, RIGHT };
template <class T> class print;

template<class T>
struct Less {
  Less() { }
  inline bool operator () ( T& a, T& b ) {
    return a < b;
  }
};

template<class T>
struct Greather {
  Greather() { }
  inline bool operator () ( T& a, T& b ) {
    return a > b;
  }
};



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
    node<T>* u = nodes + value;
    node<T>* f = nodes + u->father();
    node<T>* l = nodes + u->left();
    node<T>* r = nodes + u->right();
//    printf( "%d %d %d %d\n", value, u->father(), u->left(), u->right() );
//    printf( "%p %p %p %p\n", u, f, l, r );

    if ( type == LEFT ) {
      // se non ho niente a destra non posso ruotare a sinistra, esco subito
      if ( !r ) return;

      // sinistra invariata, destra cambia e padre cambia
      u->simple_set( u->left(), r->left(), u->right() );

      if ( r->left() ) nodes[r->left()].set_father( value );

      r->simple_set( value, r->right(), f ? f->value() : EMPTY );

      // la rotazione ha portato a radice un nuovo nodo
      if ( !f || root == value ) root = r->value();

      // per preservare gli intervalli
      if( u->right() ) u->maxvalue = nodes[u->right()].maxvalue;
      else u->maxvalue = value;
      r->minvalue = u->minvalue;
    } else {
      if ( !l ) return;

      // sinistra invariata, destra cambia e padre cambia
      u->simple_set( l->right(), u->right(), u->left() );

      // il nodo che ora e' figlio sinistro di u, ha u come padre
      if ( l->right() ) nodes[l->right()].set_father( value );

      l->simple_set( l->left(), value, f ? f->value() : EMPTY );

      if ( !f || root == value ) root = l->value();

      // il minimo dell'intervallo del nodo u potrebbe essere cambiato
      if( u->left() ) u->minvalue = nodes[u->left()].minvalue;
      else u->minvalue = value;
      l->maxvalue = u->maxvalue;
    }

    // nel padre adesso e' cambiato il figlio destro o sinistro,
    // a seconda se u era attaccato come nodo destro o sinistro
    if ( f && f->left()  == value ) f->set_left ( u->father() );
    if ( f && f->right() == value )  f->set_right( u->father() );
  }

public:
  typedef T node_type;
  typedef equivalence_info<T> info;

  ptree ( ) {
    init( );
  }

  // build a random tree with n nodes
  ptree ( T n ) : size( n ) {
    nodes = new node<T> [ n + 1 ];
    memset( nodes, 0, sizeof( node<T> ) * ( n + 1 ) );
    root = build_randomly( n, 1, nodes + 1, 0 );
    allocated = true;
  }

  // copy constructor. If copyData is true this effectively copy the data
  // otherwise the real data is shared
  ptree ( const ptree<T>& x, bool copyData = true ) {
    if ( copyData == true ) {
      nodes = new node<T> [ x.size + 1 ];
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

  ~ptree ( ) {
    if ( allocated ) delete[] nodes;
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

  string to_str ( info& eqinfo ) const {
    ostringstream stream;
    print_tree( stream, this->nodes + this->root, this->nodes, &eqinfo );
    return stream.str();
  }


  // it is another way of seeing the rotation
  bool up ( const T value ) {
    node<T>* u = nodes + value;
    if ( u->father() == EMPTY ) return false;

    node<T>* f = nodes + u->father();
    if ( f->left() == value ) rotate( u->father(), RIGHT );
    else                      rotate( u->father(), LEFT );

    return true;
  }

  T up_all ( simple_set<T> r ) {
    T total = 0;
    for ( typename simple_set<T>::iterator i = r.begin(); i < r.end(); ++i )
      total += up( *i );

    return total;
  }

  // rotate the edge a-b, b is the father
  bool rotate ( const T a, const T b ) {
    if ( nodes[b].left() == a )         rotate( b, RIGHT );
    else if ( nodes[b].right() == a )   rotate( b, LEFT );
    else {
      cerr << "improper usage of rotate(a,b).\n";
      return false;
    }

    return false;
  }



  // restituisce il sottoalbero radicato in value. E' necessario calcolare l'offset del
  // sottoalbero nell'array, ovvero da dove inizia, e il numero di nodi.
  // XXX Il sottoalbero generato e' a tutti gli effetti un ptree ma i suoi nodi sono in
  // comune con quelli dell'albero di origine, per cui attenzione a non confondersi.
  ptree<T> subtree ( const T value ) const {
    T left = value;
    // altrimenti determino l'intervallo e la dimensione del sottoalbero,
    // prima vado a sinistra il piu' possibile
    while ( nodes[left].left() != EMPTY ) left = nodes[left].left();


    T right = value;
    // poi vado a destra il piu' possibile e determino l'intervallo in O(n)
    while ( nodes[right].right() != EMPTY ) right = nodes[right].right();

    return ptree<T>( *this, value );
  }

  // ometto i controlli tanto verranno chiamate o da c() o da to_leaf
  T phi ( const T x, info& eqinfo ) const {
    T total = 0;

    // conto i nodi nel sottobraccio di sinistra
    for ( T j = nodes[x].left(); j != EMPTY; j = nodes[j].right() ) {
      if ( eqinfo[j] != EMPTY ) break;
      total++;
    }

    return total;
  }

  T gamma ( const T x, info& eqinfo ) const {
    T total = 0;

    for ( T j = nodes[x].right(); j != EMPTY; j = nodes[j].left() ) {
      if ( eqinfo[j] != EMPTY ) break;
      total++;
    }

    return total;
  }

  // calcola il c(x) ovvero |F(x)| + |G(x)| per un determinato nodo x
  // restituisce -1 in caso l'indice non sia valido (non dovrebbe mai verificarsi!)
  // Nell'array eqinfo sono contenute le informazioni a riguardo i nodi omologhi,
  // se e' != NULL va considerato.
  T c ( const T x, info& eqinfo ) const {
    T total = 0;

    // conto i nodi nel sottobraccio di sinistra
    total += phi( x, eqinfo );

    // conto i nodi nel sottobraccio di destra
    total += gamma( x, eqinfo );

    return total;
  }

  // calcola r(x), la distanza dalla radice
  T r ( const T x, info& eqinfo ) const {
    T total = 0;
    T j = x;

    // conto i nodi per arrivare alla radice
    do {
      if ( eqinfo[j] != EMPTY ) break;
      if ( nodes[j].father() == EMPTY ) break;
      total++;

      j = nodes[j].father();
    } while ( j != EMPTY );

    return total;
  }

  // funzione usata da simplify per portare il nodo x ad essere una foglia,
  // eseguendo c(x) rotazioni.
  T to_leaf ( const T x, info& eqinfo ) {
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
  void equal_subtrees ( ptree<T>& s, info& eqinfo ) {
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
        T y = nodes[s.nodes[i].minvalue - 1].right();
        // puo' succedere che y sia EMPTY!
        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) )
          eqinfo.set(y,i);
      }

      if ( s.nodes[i].maxvalue < size ) {
        T y = nodes[s.nodes[i].maxvalue + 1].left();

        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) )
          eqinfo.set(y,i);
      }
    }
  }

  // cerca il nodo con il cx piu' piccolo (o piu' grande) fra this e s.
  // cval     the minimal cx found
  // selected the correspondent node
  template<class Comp = Less<T> >
  void best_c ( ptree<T>& s, info& eqinfo, T value, T& cval, T& selected, Comp comp = Comp() ) {
    // compute c for the current node
    T current = c( value, eqinfo ) + s.c( value, eqinfo.inverse() );
    if ( comp( current, cval ) ) {  // it means: current < cval, if comp is <
      selected = value;
      cval = current;
    }

    // recursion
    T l = nodes[value].left();
    T r = nodes[value].right();

    if ( l && eqinfo[l] == EMPTY )
      best_c( s, eqinfo, l, cval, selected, comp );

    if ( r && eqinfo[r] == EMPTY )
      best_c( s, eqinfo, r, cval, selected, comp );
  }

  // SIMPLIFY processing, take the node with minimum c (not already equivalent),
  // and make it leaf, until there are such nodes
  T process ( ptree<T>& s, info& eqinfo ) {
    T total = 0;

    do {
      T selected, cx = 4; // there is always a node with cx <= 3
      best_c( s, eqinfo, root, cx, selected );

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
    info eqinfo( size );

    // 1. Preprocessing, found the equivalent subtrees
    equal_subtrees( s, eqinfo );

    // 2. On the main tree, executes the processing.
    // this can cause new equivalent subtrees so it's good to be processed before
    // Notice also that it should not be handled as a subtree because you loose informations
    // about the root node (the variable it's copied!)
    total += process( s, eqinfo );
    equal_subtrees( s, eqinfo );
    simple_set<T> equivalent( eqinfo );

    // 2. On every equivalent subtree it executes the processing
    for ( typename simple_set<T>::iterator i = equivalent.begin(); i < equivalent.end(); ++i ) {
      ptree<T> tt( subtree( *i ), false );
      ptree<T> ss( s.subtree( eqinfo[*i] ), false );

      total += tt.process( ss, eqinfo );
      // and updates the equivalence informations
      equal_subtrees( s, eqinfo );
      equivalent.update( eqinfo );
    }

    return total;
  }





  // cerca un nodo nell'albero s, che abbia il solito intervallo del nodo value in this.
  bool existSameInterval( T svalue, ptree<T>& s, info& eqinfo, T value ) {
    if ( sameInterval( nodes[value], s.nodes[svalue] ) )
      return true;

    // recursion
    T l = s.nodes[svalue].left();
    T r = s.nodes[svalue].right();

    // return left + right
    return ( (l && eqinfo[l] == EMPTY) && existSameInterval( l, s, eqinfo, value ) ) ||
           ( (r && eqinfo[r] == EMPTY) && existSameInterval( r, s, eqinfo, value ) );
  }

  // searches for nodes in this, that after an up rotation, becomes equivalent
  void semi_equivalent ( ptree<T>& s, T value, info& eqinfo, simple_set<T>& rset ) {
    T father = nodes[value].father();
    T l = nodes[value].left();
    T r = nodes[value].right();

    if ( l && eqinfo[l] == EMPTY )
      semi_equivalent( s, l, eqinfo, rset );

    if ( r && eqinfo[r] == EMPTY )
      semi_equivalent( s, r, eqinfo, rset );


    // since recursion begins at the root, I check the base case at the end (post-visit)
    if ( value == root || father == root ) return;

    // simulate the situation after the rotation
    rotate( value, father );
    // check if there is such new interval in s
    if ( existSameInterval( s.root, s, eqinfo.inverse(), father ) )
      rset.insert( value );

    // restore the old situation
    rotate( father, value );
  }


  T make_equivalent ( ptree<T>& s ) {
    info eqinfo( size );
    simple_set<T> rset( size );           // contains the nodes that need 1 rotation to be equivalent
    equal_subtrees( s, eqinfo );
    simple_set<T> equivalent( eqinfo );


    // call on the root
    semi_equivalent( s, root, eqinfo, rset );
    for ( typename simple_set<T>::iterator i = equivalent.begin(); i < equivalent.end(); ++i ) {
      ptree<T> tt( subtree( *i ), false );
      ptree<T> ss( s.subtree( eqinfo[*i] ), false );

      // call on the already equivalent subtrees
      tt.semi_equivalent( ss, tt.root, eqinfo, rset );

      equal_subtrees( s, eqinfo );
      equivalent.update( eqinfo );
    }

    return up_all( rset );
  }

  //  // porta un nodo alla radice
  //  T to_root ( const T value, T* eqinfo = NULL ) {
  //    //cout << "to_root" << endl;
  //    T rotations = 0;
  //    bool stop = false;

  //    if ( eqinfo && eqinfo[value-start] != EMPTY )
  //      return 0;

  //    while ( root != value && !stop ) {
  //      T on =  locate(value)->father();
  //      if ( eqinfo && on && eqinfo[on-start] !=EMPTY )
  //        stop = true;
  //      up( value );
  //      rotations++;
  //    }

  //    return rotations;
  //  }


//  // porta il sottolbero radicato in x in una catena sinistra o destra a seconda di type
//  // (algoritmi LEFT e RIGHT). Restituisce -1 in caso di errore, oppure 0 se l'albero e' nullo
//  T list ( T i, rotation_type type = LEFT, T* eqinfo = NULL ) {
//    if ( i == EMPTY ) return 0;

//    if ( ! valid( i ) ) {
//      cerr << "list(): invalid value " << i << ".\n";
//      return -1;
//    }

//    T total = 0;
//    while ( i != EMPTY ) {
//      T next = i;

//      T j = i;
//      while ( ( type == RIGHT && phi( j, eqinfo ) != 0 ) ||
//        ( type == LEFT && gamma( j, eqinfo ) != 0 ) ) {
//        next = ( type == LEFT ) ? locate( j )->right() : locate( j )->left();

//        rotate( j, type );
//        total++;


//        j = next;
//      }

//      // l'ultimo nodo che ho ruotato (oppure i invariato) e' quello da cui devo scendere
//      i = ( type == LEFT ) ? locate( next )->left() : locate( next )->right();
//    }

//    return total;
//  }

//  // processing basato sull'algoritmo CENTRAL
//  T central ( ptree<T>& z, T* eqinfo = NULL, T* seqinfo = NULL ) {
//    if ( z.size != size ) {
//      cerr << "central(): invalid subtrees.\n" << *this << endl << z << endl;
//      return -1;
//    }

//    T total = 0;

//    // cerco il nodo con c(x) massimo
//    T selected = EMPTY, max;
//    selected = max_non_homologue( z, eqinfo, seqinfo, root, max );

//#if DEBUG
//    printf( "CENTRAL: Selected node %d with c = %d\n", selected, max );
//#endif
//    if ( max == 0 )
//      return total;

//    // porto il nodo selezionato alla radice in entrambi gli alberi
//    total +=   to_root( selected );
//    total += z.to_root( selected );

//    // applico gli algoritmi left e right ai sottoalberi
//    total +=   list(   locate( selected )->right( ), LEFT, eqinfo );
//    total += z.list( z.locate( selected )->right( ), LEFT, seqinfo );
//    total +=   list(   locate( selected )->left( ), RIGHT, eqinfo );
//    total += z.list( z.locate( selected )->left( ), RIGHT, seqinfo );

//    return total;
//  }





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

