
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
#include "config.hpp"
using namespace std;


namespace tree {


enum rotation_type { LEFT, RIGHT };
template <class T> class print;


template <class T = unsigned int>
class ptree {
protected:
  node<T>* nodes;
  bool allocated;   // is the array allocated by this instance?
  T __size;           // number of nodes
  T __root;           // the value of the root node, nodes[root]
  typedef equivalence_info<T> info;


  // costruttore di copia, crea un albero da un array di nodi gia' presente,
  // Serve per poter trattare i sottoalberi di un albero dato, modificando anche l'originale
  ptree ( const ptree& parent, T root ) {
    this->nodes = parent.nodes;
    this->allocated = false;
    this->__root = root;
    this->__size = nodes[root].maxvalue - nodes[root].minvalue + 1;
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
    __root = 0;
    __size = 0;
    allocated = false;
  }


public:

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
      if ( !f || __root == value ) __root = r->value();

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

      if ( !f || __root == value ) __root = l->value();

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

  typedef T node_type;


  ptree ( ) {
    init( );
  }

  // build a random tree with n nodes
  ptree ( T n ) : __size( n ) {
    nodes = new node<T> [ n + 1 ];
    memset( nodes, 0, sizeof( node<T> ) * ( n + 1 ) );
    __root = build_randomly( n, 1, nodes + 1, 0 );
    allocated = true;
  }

  // copy constructor. If copyData is true this effectively copy the data
  // otherwise the real data is shared
  ptree ( const ptree<T>& x, bool shared = false ) {
    if ( shared == false ) {
      nodes = new node<T> [ x.__size + 1 ];
      //fill( nodes, nodes + x.size + 1, node<T>() );
      copy( x.nodes, x.nodes + x.__size + 1, nodes );
      allocated = true;
    } else {
      nodes = x.nodes;
      allocated = false;
    }

    __size = x.__size;
    __root = x.__root;
  }

  // costruisce un ptree da uno ztree
  ptree ( const ztree<N>& z ) : __size( z.vertices() ) {
    nodes = new node<T> [ __size + 1 ];
    memset( nodes, 0, sizeof( node<T> ) * ( __size + 1 ) );
    allocated = true;

    z.get_tree<T>( __root, nodes );
  }

  inline unsigned int word ( ) {
    unsigned int result = 0, bit = 0;
    word_r( __root, result, bit );
    //printf( "\n" );
    return result;
  }

  inline void word_r ( T node, unsigned int& result, unsigned int& bit ) {
    if ( node == EMPTY ) {
      bit++;
      //printf( "0");
      return;
    }

    result |= ( 1 << bit++ ); //printf( "1" );
    word_r( nodes[node].left(), result, bit );
    word_r( nodes[node].right(), result, bit );
  }

  ~ptree ( ) {
    if ( allocated ) delete[] nodes;
  }

  bool operator == ( const ptree<T>& x ) const {
    bool out = ( x.__size == __size ) && ( x.__root == __root );


    for ( T i = nodes[__root].minvalue; i <= nodes[__root].maxvalue && out; ++i )
      out = out && ( nodes[i] == x.nodes[i] );

    return out;
  }

  bool operator != ( const ptree<T>& x ) const {
    return !(*this == x);
  }

  bool operator < ( const ptree<T>& another ) const {
    if ( __size != another.__size )
      return __size < another.__size;

    for ( T i = nodes[__root].minvalue; i <= nodes[__root].maxvalue; ++i ) {
      if ( !( nodes[i] == another.nodes[i] ) )
        return nodes[i] < another.nodes[i];
    }

    // sono uguali
    return false;
  }

  // stampa l'albero sullo stream dato
  friend ostream& operator << ( ostream& stream, const ptree<T>& t ) {
    print<T>( t, stream );
    return stream;
  }

  node<T>& operator[] ( size_t index ) const {
    return nodes[index];
  }

  /*node<T>& operator[] ( size_t index ) {
    return nodes[index];
  }*/

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
    for ( typename simple_set<T>::iterator i = r.begin(); i < r.end(); ++i ) {
      total += up( *i );
      cout << *i << " ";
    }

    cout << endl;
    return total;
  }

  // rotate the edge a-b, where b is the father
  bool rotate ( const T a, const T b ) {
    if ( nodes[b].left() == a )         rotate( b, RIGHT );
    else if ( nodes[b].right() == a )   rotate( b, LEFT );
    else {
      // edge does not exist
      cerr << "improper usage of rotate(" << a << "," << b << ").\n";
      return false;
    }

    return false;
  }

  node<T>* base ( ) const {
    return nodes;
  }

  T size ( ) const {
    return __size;
  }

  T root ( ) const {
    return __root;
  }

  T minimum ( ) const {
    return nodes[__root].minvalue;
  }

  T maximum ( ) const {
    return nodes[__root].maxvalue;
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

  // porta un nodo alla radice
  T to_root ( const T value ) {
    //cout << "to_root" << endl;
    T rotations = 0;

    while ( __root != value )
      rotations += up( value );

    return rotations;
  }

  bool sameInterval ( node<T>& a, node<T>& b ) const {
    return a.minvalue == b.minvalue && a.maxvalue == b.maxvalue;
  }

  // L'array eqinfo conterra' le informazioni sui nodi omologhi.
  // Gli indici rappresentano i nodi in *this, mentre il contenuto l'eventuale nodo in s
  // che e' omologo (EMPTY altrimenti). XXX considera un solo array, riferito a *this,
  // se serve anche quello riferito a s, seqinfo allore e' != NULL.
  T equal_subtrees ( const ptree<T>& s, info& eqinfo ) const {
    if ( s.__size != __size ) {
      cerr << "equal_subtrees(): not equivalent trees.\n";
      return 0;
    }

    T out = 0;

    for ( T i = nodes[__root].minvalue; i <= nodes[__root].maxvalue; ++i ) {
      if ( i == __root ) continue;

      if ( sameInterval( nodes[i], s.nodes[i] ) ) {
        eqinfo.set(i, i); ++out;
        continue;
      }

      if ( s.nodes[i].minvalue > 1 ) {
        T y = nodes[s.nodes[i].minvalue - 1].right();
        // puo' succedere che y sia EMPTY!
        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) ) {
          eqinfo.set(y,i); ++out;
        }
      }

      if ( s.nodes[i].maxvalue < __size ) {
        T y = nodes[s.nodes[i].maxvalue + 1].left();

        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) ) {
          eqinfo.set(y,i); ++out;
        }
      }
    }

    return out;
  }

  // cerca il nodo con il cx piu' piccolo (o piu' grande) fra this e s.
  // cval     the minimal cx found
  // selected the correspondent node
  template<class Comp = less<T> >
  void best_c ( ptree<T>& s, info& eqinfo, T value, T& cval, T& selected, T& rx, Comp comp = Comp() ) {
    // compute c for the current node
    T current   = c( value, eqinfo ) + s.c( value, eqinfo.inverse() );
    T currentrx = r( value, eqinfo ) + s.r( value, eqinfo.inverse() );
    if ( comp( current, cval ) ) {  // it means: current < cval, if comp is <
      selected = value;
      cval = current;
      rx = currentrx;
    } else {
      if ( ! comp( cval, current ) ) { // current == cval
        // check rx
        if ( currentrx < rx ) {
          selected = value;
          cval = current;
          rx = currentrx;
        }
      }
    }

    // recursion
    T l = nodes[value].left();
    T r = nodes[value].right();

    if ( l && eqinfo[l] == EMPTY )
      best_c( s, eqinfo, l, cval, selected, rx, comp );

    if ( r && eqinfo[r] == EMPTY )
      best_c( s, eqinfo, r, cval, selected, rx, comp );
  }

  /* SIMPLIFY processing, take the node with minimum c (not already equivalent),
  // and make it leaf, until there are such nodes
  T process ( ptree<T>& s, info& eqinfo ) {
    T total = 0;

    do {
      T selected, cx = 4, rx = size(); // there is always a node with cx <= 3
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
    if ( s.__size != __size ) {
      cerr << "Not compatible trees in simplify()\n";
      return 0;
    }

    T total = 0;
    info eqinfo( __size );

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
      ptree<T> tt( subtree( *i ), true );
      ptree<T> ss( s.subtree( eqinfo[*i] ), true );

      total += tt.process( ss, eqinfo );
      // and updates the equivalence informations
      equal_subtrees( s, eqinfo );
      equivalent.update( eqinfo );
    }

    return total;
  }*/

};


}

#endif

