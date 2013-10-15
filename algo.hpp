#ifndef ALGO_HPP
#define ALGO_HPP

#include "ptree.hpp"
#include "equivalence.hpp"
#include "ztree.hpp"
#include "khash.hh"

//#include <google/sparse_hash_set>
using namespace std;


namespace tree {
////////////////////////////////////////////////////////////////////////////////
// Central algorithm and stuff
////////////////////////////////////////////////////////////////////////////////


// porta il sottolbero radicato in x in una catena sinistra o destra a seconda di type
// (algoritmi LEFT e RIGHT). Restituisce -1 in caso di errore, oppure 0 se l'albero e' nullo
template<class T>
T list ( ptree<T>& a, T i, equivalence_info<T>& eqinfo, rotation_type type = LEFT ) {
  if ( i == EMPTY ) return 0;

  a.get_structure( eqinfo );

  T total = 0;
  while ( i != EMPTY ) {
    T next = i;

    T j = i;
    while ( ( type == RIGHT && ( a[j].left() && eqinfo[a[j].left()] == EMPTY ) ) ||
            ( type == LEFT && ( a[j].right() && eqinfo[a[j].right()] == EMPTY ) ) ) {
      next = ( type == LEFT ) ? a[j].right() : a[j].left();

      a.rotate( j, type );
      ++total;

      j = next;
    }

    // l'ultimo nodo che ho ruotato (oppure i invariato) e' quello da cui devo scendere
    i = ( type == LEFT ) ? a.base()[next].left() : a.base()[next].right();
  }

  return total;
}

// processing basato sull'algoritmo CENTRAL
template<class T>
T central ( ptree<T>& a, ptree<T>& b ) {

  assert( a.size() == b.size() );
  equivalence_info<T> eqinfo( a.size() );
  T total = 0;

  // cerco il nodo con c(x) massimo
  T cmax = 0, rx = 2 * a.size() + 1;
  T selected = a.best_c( b, eqinfo, cmax, rx, greater<T>() );

  if ( cmax == 0 )
    return total;

  // porto il nodo selezionato alla radice in entrambi gli alberi
  total += a.to_root( selected );
  total += b.to_root( selected );

  // applico gli algoritmi left e right ai sottoalberi
  total += list( a, a.base()[selected].right(), eqinfo, LEFT );
  total += list( b, b.base()[selected].right(), eqinfo.inverse(), LEFT );
  total += list( a, a.base()[selected].left(), eqinfo, RIGHT );
  total += list( b, b.base()[selected].left(), eqinfo.inverse(), RIGHT );

  return total;
}

template<class T>
T centralfirststep ( ptree<T>& a, ptree<T>& b, equivalence_info<T>& eqinfo ) {
  //cout << "centralfirststep()" << endl;
  assert( a.size() == b.size() );

  T total = 0;

  // cerco il nodo con c(x) massimo
  T cmax = 0, rx = 2 * a.size() + 1;
  T selected = a.best_c( b, eqinfo, cmax, rx, greater<T>() );

  if ( cmax == 0 || selected == EMPTY )
    return total;

  //cout << "selected node " << selected << " with cmax = " << cmax << endl;

  // porto il nodo selezionato alla radice in entrambi gli alberi
  total += a.to_root( selected );
  total += b.to_root( selected );

  return total;
}


template<class T>
T movebestr ( ptree<T>& a, ptree<T>& b, equivalence_info<T>& eqinfo ) {

  assert( a.size() == b.size() );
  T total = 0;

  // cerco il nodo con r(x) minimo
  T rx = 2 * a.size() + 1;
  T selected = a.best_r( b, eqinfo, rx, less<T>() );

  if ( rx == 0 || selected == EMPTY )
    return total;

  //cout << "selected node " << selected << " with rx = " << rx << endl;

  // porto il nodo selezionato alla radice in entrambi gli alberi
  total += a.to_root( selected );
  total += b.to_root( selected );

  return total;
}


////////////////////////////////////////////////////////////////////////////////
// New algorithm and stuff
////////////////////////////////////////////////////////////////////////////////


template<class T>
bool has_equivalent ( ptree<T>& a, ptree<T>& b ) {
  assert( a.size() == b.size() );

  equivalence_info<T> eqinfo( a.size() );
  return a.equal_subtrees( b, eqinfo ) != 0;
}



template<class T>
T newalgo_r ( ptree<T>& a, ptree<T>& b, equivalence_info<T>& eqinfo ) {
  assert( a.size() == b.size() );

  if ( a.size() == 0 || a.size() == 1 )
    return 0;


  T total = 0;
  a.equal_subtrees( b, eqinfo );              // aggiorno gli intervalli
  total += k_equivalent(a, b, 1, eqinfo);        // stacco nodi k-equivalenti
  a.equal_subtrees( b, eqinfo );              // riaggiorno

  total += centralfirststep( a, b, eqinfo );  // porto un nodo a radice
  a.equal_subtrees( b, eqinfo );              // riaggiorno (in particolare i figli)

  ptree<T> al = a.left();
  ptree<T> bl = b.left();
  ptree<T> ar = a.right();
  ptree<T> br = b.right();
  return total + newalgo_r( al, bl, eqinfo ) + newalgo_r( ar, br, eqinfo );
}



template<class T>
T newalgo ( ptree<T>& a, ptree<T>& b ) {
  //cout << "newalgo()\n";
  equivalence_info<T> eqinfo( a.size() );
  return newalgo_r( a, b, eqinfo );
}





template<class T>
T mix_r ( ptree<T>& a, ptree<T>& b, equivalence_info<T>& eqinfo ) {
  assert( a.size() == b.size() );


  if ( a.size() == 0 || a.size() == 1 )
    return 0;

  T total = 0;
  a.equal_subtrees( b, eqinfo );              // aggiorno gli intervalli
  total += k_equivalent(a, b, 1, eqinfo);     // stacco nodi k-equivalenti
  a.equal_subtrees( b, eqinfo );              // riaggiorno

  total += movebestr( a, b, eqinfo );         // porto un nodo a radice
  a.equal_subtrees( b, eqinfo );              // riaggiorno (in particolare i figli)


  ptree<T> al = a.left();
  ptree<T> bl = b.left();
  ptree<T> ar = a.right();
  ptree<T> br = b.right();
  return total + mix_r( al, bl, eqinfo ) + mix_r( ar, br, eqinfo );
}



template<class T>
T mix ( ptree<T>& a, ptree<T>& b ) {
  equivalence_info<T> eqinfo( a.size() );
  return mix_r( a, b, eqinfo );
}







template<class T>
bool k_equivalent_r ( ptree<T>& a, ptree<T>& b, T k, equivalence_info<T>& eqinfo, T before ) {
  if ( k == 0 ) {
    T after = a.equal_subtrees( b, eqinfo );

    T threshold = 1;
    if ( after - before >= threshold )
      return true; // keep
    else
      return false;
  }

  for ( T i = a.minimum(); i <= a.maximum(); ++i ) {
    if ( i == a.root() ) continue;

    if ( eqinfo[i] != EMPTY ) continue;

    T father = a[i].father();

    a.up( i );
    bool keep = k_equivalent_r( a, b, k-1, eqinfo, before );


    if ( keep )
      return true; // keep
    a.rotate( father, i );
  }

  return false;
}


template<class T>
T k_equivalent ( ptree<T>& a, ptree<T>& b, T k, equivalence_info<T>& eqinfo ) {
  T total = 0;

  for ( T t = 1; t <= k; ) {
    T before = a.equal_subtrees( b, eqinfo );

    bool something = k_equivalent_r( a, b, t, eqinfo, before );
    if ( !something )
      assert ( a.equal_subtrees(b, eqinfo) == before );

    if ( !something )
      something = k_equivalent_r( b, a, t, eqinfo.inverse(), before );

    if ( !something ) // increase t
      ++t;
    else {
      total += t; // t remain the same (try to search again)
    }
  }


  return total;
}



























template<class T>
class unordered_set : public khset_t<T> {};

template<unsigned int U>
size_t distance ( const ztree<U>& a, const ztree<U>& b, size_t& visited ) {
  visited = 0;

  if ( a == b ) return 0;
  unordered_set<unsigned long> queued;
  //google::sparse_hash_set<unsigned long> queued;
  deque<ztree<U> > q;
  q.push_back( a );
  queued.insert( (int) a.to_ulong() );

  // During BFS I scan sequentially all nodes at distance d, exploring the next level d+1.
  // I set two extremes for two sets:
  //   [0,left)       contains the nodes at distance d, d-1, d-2.. 0 from the source node a
  //   [left,right)   contains the nodes at distance d+1 from a
  // When [0,left) = [0,0) means that I have exhausted all the nodes at distance d, d-1..
  // that means I generated all the nodes on the next level, so I can increase the distance
  // from the source.
  int left = 1;      // Initially I have node a at distance 0 from a, so [0,left) must contain only node 0
  int right = 1;     // The right limit is the left limit: I have no known node at distance 1
  size_t d = 0;      // distance from a in the current level
  bool found = false;

  while( q.size() != 0 ) {
   size_t occupied = q.size() * sizeof( ztree<U> ) + queued.size() * sizeof( unsigned long );
   if ( occupied > visited ) visited = occupied;
    // select first node in the deque and generates its outcoming star
    for ( unsigned int i = 1; i <= U; ++i ) {
      ztree<U> newone = q.front();
      newone ^ i;

      // if I already queued (or visited) this node I simply skip it
      if ( queued.find( newone.to_ulong() ) != queued.end() )
        continue;

      // if I found it, exit from the loops
      if ( newone == b ) {
        found = true;
        break;
      }

      // otherwise put the new node in the deque and to the hashtable
      q.push_back( newone );
      queued.insert( newone.to_ulong() );
      right++;
    }

    if ( found ) break;

    // effectively pop the first element, after visiting him
    q.pop_front();
    --right;
    // start processing elements at the next level?
    if ( --left == 0 ) {
      left = right;
      ++d;
    }
  }

  if (!found) {
      cerr << "Fatal error.\n";
      //cout << queued.count( a.to_ulong() ) << " " << queued.count( b.to_ulong() ) << endl;
      exit( 1 );
  }

  //visited = queued.size();
  return d + 1;
}

} // namespace tree

#endif // ALGO_HPP
