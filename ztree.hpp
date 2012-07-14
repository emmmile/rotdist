
#ifndef _ZTREE_HPP
#define _ZTREE_HPP  1

#include <string>
#include <cstdlib>
#include "node.hpp"
#include <boost/dynamic_bitset.hpp>
using namespace std;
using namespace boost;
using namespace tree;


class ztree {
private:
  // struttura dati che contiene la codifica dell'albero
  dynamic_bitset<unsigned char> tree;



  // restituisce la posizione del nodo i-esimo in preordine
  unsigned int preorder_node ( unsigned int n ) const {
    if ( n > vertices() )
      return -1;

    for ( unsigned int i = 0; i < tree.size(); i++ ) {
      n -= tree[i];
      if ( n == 0 ) return i;
    }

    // XXX questo caso non dovrebbe mai verificarsi, altrimenti vuol dire che
    // la struttura non e' consistente
    cerr << "Fatal Error in ztree::preorder_node().\n";
    return -1;
  }

  // restituisco la posizione del padre del nodo pos
  unsigned int father ( unsigned int pos ) const {
    if ( pos >= tree.size() )
      return -1;//string::npos;

    unsigned int z;
    unsigned int o;
    for ( z = o = 0, pos--; pos >= 0; pos-- ) {
      z += !tree[pos];
      o +=  tree[pos];
      if ( z <= o ) break;
    }

    return pos;
  }

  // restituisco la fine del primo sottoalbero che parte da pos
  unsigned int first_subtree ( unsigned int pos ) const {
    if ( pos >= tree.size() )
      return -1;//string::npos;

    unsigned int z;
    unsigned int o;
    for ( z = o = 0, pos++; pos < tree.size(); pos++ ) {
      z += !tree[pos];
      o +=  tree[pos];
      if ( z > o ) break;
    }

    return pos;
  }

  void random_tree ( const int n ) {
    unsigned int begin = 0;
    fill_randomly_r( begin, n );
  }

  void fill_randomly_r ( unsigned int& out, const int n ) {
    if ( n == 0 ) return;

    tree[out] = true;
    int i = random( ) % n;
    fill_randomly_r( ++out, i );
    fill_randomly_r( ++out, n - i - 1 );
  }

public:
  typedef unsigned int node_type;

  // XXX COSTRUTTORI
  // costruisco un albero vuoto (0)
  ztree ( ) : tree( 1 ) {
    tree[0] = false;
  }

  // costruisco un albero casuale con n nodi
  ztree( const unsigned int n ) : tree( 2 * n + 1, false ) {
    random_tree( n );
  }

  // costruttore di copia
  ztree ( const ztree& a ) {
    tree = a.tree;
  }

  // costruisce un albero dai sui sotto-alberi (aggiunge la radice)
  ztree ( const ztree& a, const ztree& b )
    : tree( a.tree.size() + 1 + b.tree.size(), false ) {

    tree[0] = true;
    for ( unsigned int i = 0; i < a.tree.size(); ++i )
      tree[1 + i] = a.tree[i];

    for ( unsigned int i = 0; i < b.tree.size(); ++i )
      tree[1 + a.tree.size() + i] = b.tree[i];
  }

  // costruisce un albero da una stringa (generalmente passata da utente)
  ztree ( const string& s, const unsigned int n ) : tree( 2 * n + 1, false ) {
    unsigned int i, o, z;
    o = z = i = 0;

    // scorro la stringa finche trovo 0 e 1 e finche il numero di zeri e' minore o uguale
    while ( ( s.size() == 2 * n + 1 ) &&
      i < s.size() &&
      ( s[i] == '0' || s[i] == '1' ) && z <= o ) {

      // aggiungo alla sequenze
      tree[i] = ( s[i] == '1' ) ? true : false;

      // incremento i contatori
      o += ( s[i] == '1' );
      z += ( s[i] != '1' );
      i++;
    }

    // nel caso abbia trovato una lettera e la stringa attuale non sia valida
    if ( ( s.size() != 2 * n + 1 ) || ( z != o + 1 && i != s.size() - 1 ) )
      tree.resize( 1, false );
  }


  // XXX FUNCTIONS
  // restituisce una stringa che rappresenta l'albero
  string to_string ( ) const {
    string out( tree.size(), '0' );
    for ( unsigned int i = 0; i < tree.size(); i++ )
      if ( tree[i] ) out[i] = '1';

    return out;
  }

  // ruota il nodo selezionato verso l'alto
  ztree& rotate ( unsigned int ii ) {
    unsigned int i = preorder_node( ii );
    // casi eccezionali: radice, indice grande o negativo, selezionato non un nodo
    if ( i == 0 || i >= tree.size() || tree[i] == false ) return *this;


    unsigned int j = father( i );
    unsigned int h = first_subtree( i );

    // rotazione a destra
    if ( j == i - 1 ) {
      for ( unsigned int l = j; l <= j + h - i; l++ )
        tree[l] = tree[l + 1];

      // posiziono il padre
      tree[h] = true;
      // il resto, cioe' i figli destro di i e di j restano nella solita posizione
    // rotazione a sinistra
    } else {
      for ( unsigned int l = i - 1; l >= j + 1; l-- )
        tree[l + 1] = tree[l];

      // posiziono il padre del nodo selezionato
      tree[j + 1] = true;
    }

    return *this;
  }

  // restituisce il numero di nodi dell'albero
  unsigned int vertices ( ) const {
    return ( tree.size() - 1 ) >> 1;
  }

  // ritorna il numero di bytes allocati dalla struttura tree
  unsigned int bytes ( ) const {
    return tree.num_blocks() * sizeof( dynamic_bitset<>::block_type );
  }

  // conversione da ztree a ptree<T> (array di node<T>)
  template <typename T>
  void get_tree ( T& root, node<T>* out ) const {
    root = get_tree_r( out, EMPTY, 1, 0, tree.size() );
  }

  template <typename T>
  T get_tree_r ( node<T>* base, T father, T pbeg, int beg, int end ) const {
    if ( end - beg <= 1 ) return EMPTY;

    //printf( "%d %d %d %d %d\n", father, pbeg, pend, beg, end );

    int sep = first_subtree( beg ) + 1;
    T value = pbeg + ( ( sep - beg - 1 ) >> 1 );
    T left  = get_tree_r( base, value, pbeg, beg + 1, sep );
    T right = get_tree_r( base, value, value + 1, sep, end );

    return base[value].set( value, left, right, father );
  }

  // XXX OPERATORS
  ztree& operator ^ ( const unsigned int n ) {
    return rotate( n );
  }

  bool operator == ( const ztree& x ) const {
    return x.tree == tree;
  }

  bool operator < ( const ztree& x ) const {
    return tree < x.tree;
  }

  ztree operator + ( const ztree& x ) const {
    return ztree( *this, x );
  }

  ztree& operator = ( const ztree& x ) {
    tree = x.tree;
    return *this;
  }

  friend ostream& operator << ( ostream& stream, const ztree t ) {
    for ( unsigned int i = 0; i < t.tree.size(); stream << ( t.tree[i++] ? '1' : '0' ) );

    return stream;
  }
};

#endif
