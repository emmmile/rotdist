#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <deque>
#include <algorithm>
#include <thread>
#include <unordered_set>
#include "config.hpp"
#include "ztree.hpp"
#include "ptree.hpp"
using namespace std;
using namespace tree;



size_t distance ( const ztree<N>& a, const ztree<N>& b, size_t& visited ) {
  visited = 0;

  if ( a == b ) return 0;
  unordered_set<unsigned long> queued( 100 );
  deque<ztree<N> > q;
  q.push_back( a );
  queued.insert( a.to_ulong() );

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
    // select first node in the deque and generates its outcoming star
    for ( unsigned int i = 1; i <= N; ++i ) {
      ztree<N> newone = q.front();
      newone ^ i;

      // if I already queued (or visited) this node I simply skip it
      if ( queued.count( newone.to_ulong() ) > 0 )
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
      cout << queued.count( a.to_ulong() ) << " " << queued.count( b.to_ulong() ) << endl;
      exit( 1 );
  }

  visited = queued.size();
  return d + 1;
}



void testfunctions ( size_t runs ) {
  size_t i;
  Random gen( (int) &i );


  for (i = 0; i < runs; ++i ) {
    ztree<N> a( gen );
    ztree<N> b( gen );
    ptree<int> aa( a ), aaa( a );
    ptree<int> bb( b ), bbb( b );

    size_t visited;
    int toptimal, tdistance, toldistance;
    toptimal = distance(a, b, visited );
    tdistance = aa.distance( bb );
    toldistance = aaa.oldistance( bbb );

    assert( aa == bb );
    assert( aaa == bbb );
    assert( toptimal <= tdistance );

    cout << a << "\t" << b << "\t" << toptimal << "\t" << tdistance << "\t" << toldistance << "\t" << visited << endl;
  }
}


int main ( int argv, char** argc ) {
  if ( argv < 3 || atoi( argc[1] ) <= 0 || atoi( argc[2] ) <= 0 ) return 1;
  size_t threads = atoi( argc[1] );
  size_t runs = atoi( argc[2] );

  cout << "first\tsecond\topt\tdist\tcent\tvisited\n";
  vector<thread> t;

  //Launch a group of threads
  for (int i = threads; i > 0; --i) {
    size_t part = runs / i;
    t.push_back( thread( testfunctions, part ) );
    runs -= part;
  }

  //Join the threads with the main thread
  for( auto& i : t ) i.join();

	return 0;
}


