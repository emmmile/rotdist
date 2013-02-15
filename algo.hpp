#ifndef ALGO_HPP
#define ALGO_HPP

#include "ptree.hpp"
#include "equivalence.hpp"
#include "ztree.hpp"
#include "khash.hh"


namespace tree {
////////////////////////////////////////////////////////////////////////////////
// Central algorithm and stuff
////////////////////////////////////////////////////////////////////////////////


// porta il sottolbero radicato in x in una catena sinistra o destra a seconda di type
// (algoritmi LEFT e RIGHT). Restituisce -1 in caso di errore, oppure 0 se l'albero e' nullo
template<class T>
T list ( ptree<T>& a, T i, equivalence_info<T>& eqinfo, rotation_type type = LEFT ) {
  if ( i == EMPTY ) return 0;

  T total = 0;
  while ( i != EMPTY ) {
    T next = i;

    T j = i;
    while ( ( type == RIGHT && a.phi( j, eqinfo ) != 0 ) ||
            ( type == LEFT && a.gamma( j, eqinfo ) != 0 ) ) {
      next = ( type == LEFT ) ? a.base()[j].right() : a.base()[j].left();

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
  T selected = EMPTY, cmax = 0, rx = a.size();
  a.best_c( b, eqinfo, a.root(), cmax, selected, rx, greater<T>() );

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
  assert( a.size() == b.size() );

  T total = 0;

  // cerco il nodo con c(x) massimo
  T selected = EMPTY, cmax = 0, rx = a.size();
  a.best_c( b, eqinfo, a.root(), cmax, selected, rx, greater<T>() );


  if ( cmax == 0 )
    return total;

  //cout << "selected = " << selected << endl;

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
  T selected = EMPTY, rx = 2 * a.size() + 1;
  a.best_r( b, eqinfo, a.root(), rx, selected, less<T>() );

  if ( rx == 0 )
    return total;

  if ( selected == 0 )
    print<T>(a, b,eqinfo);

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
  total += k_equivalent(a, b, eqinfo);        // stacco nodi k-equivalenti
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
  equivalence_info<T> eqinfo( a.size() );
  return newalgo_r( a, b, eqinfo );
}





template<class T>
T newbetteralgo_r ( ptree<T>& a, ptree<T>& b, equivalence_info<T>& eqinfo ) {
  assert( a.size() == b.size() );

  if ( a.size() == 0 || a.size() == 1 )
    return 0;


  T total = 0;
  a.equal_subtrees( b, eqinfo );              // aggiorno gli intervalli
  total += k_equivalent(a, b, eqinfo);        // stacco nodi k-equivalenti
  a.equal_subtrees( b, eqinfo );              // riaggiorno

  total += movebestr( a, b, eqinfo );         // porto un nodo a radice
  a.equal_subtrees( b, eqinfo );              // riaggiorno (in particolare i figli)

  ptree<T> al = a.left();
  ptree<T> bl = b.left();
  ptree<T> ar = a.right();
  ptree<T> br = b.right();
  return total + newbetteralgo_r( al, bl, eqinfo ) + newbetteralgo_r( ar, br, eqinfo );
}



template<class T>
T newbetteralgo ( ptree<T>& a, ptree<T>& b ) {
  equivalence_info<T> eqinfo( a.size() );
  return newbetteralgo_r( a, b, eqinfo );
}






template<class T>
T handle_k_equivalence_r ( ptree<T>& a, ptree<T>& b, T k, equivalence_info<T>& eqinfo, T before, T kin ) {
  if ( k == 0 ) {
    T after = a.equal_subtrees( b, eqinfo );


    // this is the condition for the operation to be kept
    // holds if the initial k is less than 4, that is deleting 2 subtrees
    // with up to 3 rotations reduce the rotations bound
    T threshold = (kin == 1 ? 1 : 1);
    if ( after - before >= threshold ) return 1;
    else return 0;
  }

  for ( T i = a.minimum(); i <= a.maximum(); ++i ) {
    if ( i == a.root() ) continue;

    a.equal_subtrees( b, eqinfo );
    if ( eqinfo[i] != EMPTY ) continue;

    T father = a[i].father();

    a.up( i );
    T rots = handle_k_equivalence_r( a, b, k-1, eqinfo, before, kin );


    if ( rots == 0 ) {
      // the rotation has to be reverted (the condition is not satisfied)
      a.rotate( father, i );
    } else {
      // else I want to keep
      if ( kin == k ) {
        //cout << "kept rotation with " << i << endl;
        return rots;
      } else
        return rots + 1;
    }
  }

  return 0;
}



// esegue una singola k-equivalenza, assumendo non ci siano nodi equivalenti con
// 1, 2, ..., k-1 rotazioni.
template<class T>
T handle_k_equivalence ( ptree<T>& a, ptree<T>& b, T k, equivalence_info<T>& eqinfo ) {
  assert( k > 0 );

  T before = a.equal_subtrees( b, eqinfo );

  if ( before == a.size() - 1 ) return 0;

  T total = handle_k_equivalence_r( a, b, k, eqinfo, before, k );
  if ( total != 0 ) return total;

    total = handle_k_equivalence_r( b, a, k, eqinfo.inverse(), before, k );
  if ( total != 0 ) return total;

  return 0;
}



template<class T>
T k_equivalent ( ptree<T>& a, ptree<T>& b, equivalence_info<T>& eqinfo ) {
  T total = 0;
  T k = 1;

  while ( k <= 1 ) {
    T op = handle_k_equivalence(a,b,k,eqinfo);

    if ( op == 0 )
      k++;
    else {
      if ( op == 2 ) {
        //cout << "step with k = 2\n";
        //print<T>(a,b);
      }
      if ( op == 3 ) {
        cout << "step with k = 3\n";
        //print<T>(a,b);
      }
      total += op;
      k = 1;
    }
  }

  return total;
}






























template<class T>
class unordered_set : public khset_t<T> {};


size_t distance ( const ztree<N>& a, const ztree<N>& b, size_t& visited ) {
  visited = 0;

  if ( a == b ) return 0;
  unordered_set<unsigned long> queued;
  deque<ztree<N> > q;
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
   size_t occupied = q.size() * sizeof( ztree<N> ) + queued.size() * sizeof( unsigned long );
   if ( occupied > visited ) visited = occupied;
    // select first node in the deque and generates its outcoming star
    for ( unsigned int i = 1; i <= N; ++i ) {
      ztree<N> newone = q.front();
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
