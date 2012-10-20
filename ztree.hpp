
#ifndef _ZTREE_HPP
#define _ZTREE_HPP  1

#include <string>
#include <cstdlib>
#include "node.hpp"
#include <bitset>
#include "random.hpp"
using namespace std;
using namespace tree;



template<unsigned int V>
class ztree {
private:
  // struttura dati che contiene la codifica dell'albero
  bitset<2 * V + 1> tree;

  // restituisce la posizione del nodo i-esimo in preordine
    inline unsigned int preorder_node ( unsigned int n ) const {
      unsigned int i = 0;
      for ( i = 0; i < tree.size() && n != 0; i++ ) {
        n -= tree[i];
      }

      return i - 1;
    }

    // restituisco la posizione del padre del nodo pos
    inline unsigned int father ( unsigned int pos ) const {

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
    inline unsigned int first_subtree ( unsigned int pos ) const {

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

  void random_tree ( const int n, Random& gen ) {
    unsigned int begin = 0;
    fill_randomly_r( begin, n, gen );
  }

  void fill_randomly_r ( unsigned int& out, const int n ) {
    if ( n == 0 ) return;

    tree[out] = true;
    int i = random( ) % n;
    fill_randomly_r( ++out, i );
    fill_randomly_r( ++out, n - i - 1 );
  }

  void fill_randomly_r ( unsigned int& out, const int n, Random& gen ) {
    if ( n == 0 ) return;

    tree[out] = true;
    int i = gen( n );
    fill_randomly_r( ++out, i );
    fill_randomly_r( ++out, n - i - 1 );
  }

public:
  typedef unsigned int node_type;

  // XXX COSTRUTTORI

  // costruisco un albero casuale con n nodi
  ztree( ) {
    random_tree( V );
  }

  ztree( Random& gen ) {
    random_tree( V, gen );
  }

  // costruttore di copia
  ztree ( const ztree& a ) {
    tree = a.tree;
  }

  // costruisce un albero dai suoi sotto-alberi (aggiunge la radice)
//  ztree ( const ztree& a, const ztree& b )
//    : tree( a.tree.size() + 1 + b.tree.size(), false ) {

//    tree[0] = true;
//    for ( unsigned int i = 0; i < a.tree.size(); ++i )
//      tree[1 + i] = a.tree[i];

//    for ( unsigned int i = 0; i < b.tree.size(); ++i )
//      tree[1 + a.tree.size() + i] = b.tree[i];
//  }

  // costruisce un albero da una stringa (generalmente passata da utente)
  ztree ( const string& s ) {
    unsigned int i, o, z;
    o = z = i = 0;

    // scorro la stringa finche trovo 0 e 1 e finche il numero di zeri e' minore o uguale
    while ( ( s.size() == 2 * V + 1 ) &&
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
    if ( ( s.size() != 2 * V + 1 ) || ( z != o + 1 && i != s.size() - 1 ) )
      cerr << "Error in ztree constructor.\n";
  }


  // XXX FUNCTIONS
  string to_string() const {
    string s = tree.to_string();
    for ( string::iterator i = s.begin(), j = s.end() - 1; i < j; swap( *(i++), *(j--) ) );
    return s;
  }


  unsigned long to_ulong ( ) const {
    return tree.to_ulong();
  }

  // ruota il nodo selezionato verso l'alto
  inline ztree& up ( unsigned int ii ) {
    unsigned int i = preorder_node( ii );
    // casi eccezionali: radice, indice grande o negativo, selezionato non un nodo
    if ( i == 0 )
      return *this;

    if ( i >= tree.size() || tree[i] == false ) {
      cout << i << " " << tree.size() << " " << tree[i] << endl;
      cerr << "Error in ztree::up().\n";
      return *this;
    }

    unsigned int j = father( i );
    unsigned int h = first_subtree( i );

    // rotazione a destra
    if ( j == i - 1 ) {
      for ( unsigned int l = j; l <= j + h - i; ++l )
        tree[l] = tree[l + 1];

      // posiziono il padre
      tree[h] = true;
      // il resto, cioe' i figli destro di i e di j restano nella solita posizione
    // rotazione a sinistra
    } else {
      for ( unsigned int l = i - 1; l >= j + 1; --l )
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
    return sizeof( tree );
  }

  // conversione da ztree a ptree<T> (array di node<T>)
  template <typename T>
  void get_tree ( T& root, node<T>* out ) const {
    root = get_tree_r( out, EMPTY, 1, (int) vertices(), 0, tree.size() );
  }

  // restituisce il valore del nodo radice dell'albero corrente
  // father e' il valore del nodo padre
  // pbeg e' il valore da cui parte la numerazione di questo sottoalbero
  // pend il valore massimo
  // [beg, end) sono indici all'interno del dynamic bitset
  template <typename T>
  T get_tree_r ( node<T>* base, T father, T pbeg, T pend, int beg, int end ) const {
    if ( end - beg <= 1 ) return EMPTY;

    //printf( "%d %d %d %d %d\n", father, pbeg, pend, beg, end );

    // restituisce la fine del sottoalbero sinistro radicato in beg
    // (quanti nodi ho a sinistra?)
    int sep = first_subtree( beg ) + 1;
    // calcolo il valore del nodo radice
    T value = pbeg + ( ( sep - beg - 1 ) >> 1 );
    // chiamo ricorsivamente a destra e a sinistra
    T left  = get_tree_r( base, value, pbeg, value - 1, beg + 1, sep );
    T right = get_tree_r( base, value, value + 1, pend, sep, end );

    // XXX da rivedere
    return base[value].set( value, left, right, father, pbeg, pend );
  }

  // XXX OPERATORS
  ztree& operator ^ ( const unsigned int n ) {
    return up( n );
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
