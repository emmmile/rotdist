
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
#include "equivalence.hpp"
#include "config.hpp"
using namespace std;


namespace tree {


enum rotation_type { LEFT, RIGHT };
template <class T> class print;











template <class T = unsigned int>
class ptree {
protected:

  class structure_info {
    T* base;
  public:
    T* left;
    T* right;
    T* c;
    T* r;

    structure_info( ) {
    }

    structure_info( T size ) {
      T step = size + 1;
      base = new T [ step * 4 ];
      fill( base, base + 4 * step, EMPTY );

      left  = base;
      right = base + 1 * step;
      c     = base + 2 * step;
      r     = base + 3 * step;
    }

    void free ( ) {
      delete[] base;
    }

    void clear ( T beg, T end ) {
      fill( left + beg, left + end, EMPTY );
      fill( right + beg, right + end, EMPTY );
      fill( c + beg, c + end, EMPTY );
      fill( r + beg, r + end, EMPTY );
    }
  };

  typedef T node_type;


  node<T>* nodes;
  bool allocated;     // is the array allocated by this instance?
  T __size;           // number of nodes
  T __root;           // the value of the root node, nodes[root]
  structure_info info;



  // costruttore di copia, crea un albero da un array di nodi gia' presente,
  // Serve per poter trattare i sottoalberi di un albero dato, modificando anche l'originale
  ptree ( const ptree& parent, T root ) {
    this->nodes = parent.nodes;
    this->info = parent.info;
    this->allocated = false;
    this->__root = root;
    if ( root == EMPTY )
      this->__size = 0;
    else
      this->__size = nodes[root].maxvalue - nodes[root].minvalue + 1;
    //cout << "shared\n";
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

  ////////////////////////////////////////////////////////////////////////////////
  // Constructors
  ////////////////////////////////////////////////////////////////////////////////

  ptree ( ) {
    init( );
  }

  // build a random tree with n nodes
  ptree ( T n )
    : __size( n ), info( n ) {
    nodes = new node<T> [ n + 1 ];
    memset( nodes, 0, sizeof( node<T> ) * ( n + 1 ) );
    __root = build_randomly( n, 1, nodes + 1, 0 );
    allocated = true;
  }

  // costruisce un ptree da uno ztree
  template<unsigned int V>
  ptree ( const ztree<V>& z )
    : __size( z.vertices() ), info( z.vertices() ) {
    nodes = new node<T> [ __size + 1 ];
    memset( nodes, 0, sizeof( node<T> ) * ( __size + 1 ) );
    allocated = true;

    z.get_tree( __root, nodes );
  }

  // copy constructor, not shared!
  ptree ( const ptree<T>& x )
    : __size( x.__size ), __root( x.__root ), info( x.__size ) {
    nodes = new node<T> [ x.__size + 1 ];
    copy( x.nodes, x.nodes + x.__size + 1, nodes );
    allocated = true;

    cout << "not shared\n";
  }

  ~ptree ( ) {
    if ( allocated ) {
      delete[] nodes;
      this->info.free();
    }
  }


  ////////////////////////////////////////////////////////////////////////////////
  // Utility functions
  ////////////////////////////////////////////////////////////////////////////////

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

  ptree<T> left ( ) const {
    return ptree<T>( *this, nodes[__root].left() );
  }

  ptree<T> right ( ) const {
    return ptree<T>( *this, nodes[__root].right() );
  }

  node<T>& operator[] ( size_t index ) const {
    return nodes[index];
  }

  // restituisce un sottoalbero condiviso
  ptree<T> subtree ( const T value ) const {
    return ptree<T>( *this, value );
  }

  bool sameInterval ( node<T>& a, node<T>& b ) const {
    return a.minvalue == b.minvalue && a.maxvalue == b.maxvalue;
  }

  // L'array eqinfo conterra' le informazioni sui nodi omologhi.
  // Gli indici rappresentano i nodi in *this, mentre il contenuto l'eventuale nodo in s
  // che e' omologo (EMPTY altrimenti). XXX considera un solo array, riferito a *this,
  // se serve anche quello riferito a s, seqinfo allore e' != NULL.
  T equal_subtrees ( const ptree<T>& s, equivalence_info<T>& eqinfo ) const {
    if ( s.__size != __size ) {
      cerr << "equal_subtrees(): not equivalent trees.\n";
      return 0;
    }

    T out = 0;
    eqinfo.clear( minimum(), maximum() + 1 );

    for ( T i = minimum(); i <= maximum(); ++i ) {
      //if ( i == __root ) continue;

      if ( sameInterval( nodes[i], s.nodes[i] ) ) {
        //cout << "node " << i << " has same interval of " << i << endl;
        eqinfo.set(i, i); ++out;
        continue;
      }

      if ( s.nodes[i].minvalue > minimum() ) {
        T y = nodes[s.nodes[i].minvalue - 1].right();
        // puo' succedere che y sia EMPTY!
        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) ) {
          //cout << "node " << y << " has same interval of " << i << endl;
          eqinfo.set(y,i); ++out;
        }
      }

      if ( s.nodes[i].maxvalue < maximum() ) {
        T y = nodes[s.nodes[i].maxvalue + 1].left();

        if ( y != EMPTY && sameInterval( nodes[y], s.nodes[i] ) ) {
          //cout << "node " << y << " has same interval of " << i << endl;
          eqinfo.set(y,i); ++out;
        }
      }
    }

    return out;
  }

  template<class Comp = less<T> >
  T best_r ( ptree<T>& s, equivalence_info<T>& eqinfo, T& rvalue, Comp comp = Comp() ) {
    get_structure(eqinfo);
    s.get_structure(eqinfo.inverse());

    T selected = EMPTY;

    for ( T i = minimum(); i <= maximum(); ++i ) {
      if ( info.r[i] == EMPTY && s.info.r[i] == EMPTY )
        continue;

      T current = info.r[i] + s.info.r[i];

      if ( comp( current, rvalue ) ) {
        selected = i;
        rvalue = current;
      }
    }

    return selected;
  }


  template<class Comp = less<T> >
  T best_c ( ptree<T>& s, equivalence_info<T>& eqinfo, T& cvalue, T& rvalue, Comp comp = Comp() ) {
    get_structure(eqinfo);
    s.get_structure(eqinfo.inverse());

    T selected = EMPTY;

    for ( T i = minimum(); i <= maximum(); ++i ) {
      T current = info.c[i] + s.info.c[i];
      T rx = info.r[i] + s.info.r[i];

      if ( comp( current, cvalue ) ) { // <
        selected = i;
        cvalue = current;
        rvalue = rx;
      } else {
        if ( ! comp( cvalue, current ) && rx < rvalue ) { // ==
          selected = i;
          cvalue = current;
          rvalue = rx;
        }
      }
    }

    return selected;
  }

  void get_structure ( equivalence_info<T>& eqinfo ) {
    info.clear( nodes[root()].minvalue, nodes[root()].maxvalue + 1 );
    get_structure_r( root(), 0, eqinfo );

    /*printf( "x\tc(x)\tr(x)\n" );
    for ( T i = nodes[root()].minvalue; i < nodes[root()].maxvalue + 1; ++i ) {
      printf( "%d\t%d\t%d\t%d\t%d\n", i, info.c[i], info.r[i], info.left[i], info.right[i] );
    }*/
  }

  void get_structure_r ( T value, T rx, equivalence_info<T>& eqinfo ) {
    T l = nodes[value].left();
    T r = nodes[value].right();

    bool left  = ( l && eqinfo[l] == EMPTY );
    bool right = ( r && eqinfo[r] == EMPTY );

    info.r[value] = rx;

    // base
    if ( !left && !right ) {
      info.left[value] = info.right[value] = info.c[value] = 0;
      return;
    }

    // recursion
    if ( left )
      get_structure_r( l, rx + 1, eqinfo );

    if ( right )
      get_structure_r( r, rx + 1, eqinfo );

    info.left[value]  =   left ? info.left[l]  + 1 : 0;
    info.right[value] =  right ? info.right[r] + 1 : 0;
    info.c[value]     = ( left ? info.right[l] + 1 : 0 ) + ( right ? info.left[r] + 1 : 0 );
  }


  ////////////////////////////////////////////////////////////////////////////////
  // Rotations
  ////////////////////////////////////////////////////////////////////////////////

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

  // it is another way of seeing the rotation
  bool up ( const T value ) {
    node<T>* u = nodes + value;
    if ( u->father() == EMPTY ) return false;

    node<T>* f = nodes + u->father();
    if ( f->left() == value ) rotate( u->father(), RIGHT );
    else                      rotate( u->father(), LEFT );

    return true;
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

  // porta un nodo alla radice
  T to_root ( const T value ) {
    T rotations = 0;

    while ( __root != value )
      rotations += up( value );

    return rotations;
  }


  ////////////////////////////////////////////////////////////////////////////////
  // Conversion and comparison
  ////////////////////////////////////////////////////////////////////////////////


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

    for ( T i = minimum(); i <= maximum(); ++i ) {
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

  void debug ( ) {
    for ( T i = minimum(); i <= maximum(); ++i )
      cout << nodes[i] << endl;
  }
};


}

#endif

