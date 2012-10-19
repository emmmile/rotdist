#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <algorithm>
#include <unordered_set>
#include "config.hpp"
#include "ztree.hpp"
#include "ptree.hpp"
using namespace std;
using namespace tree;



size_t distance ( const ztree<N>& a, const ztree<N>& b, size_t& visited ) {
  unordered_set<unsigned long> queued( 100 );
  vector<ztree<N> > q;
  q.push_back( a );
  queued.insert( a.to_ulong() );
  size_t qindex = 0;
                        // I am in a interval, all nodes at distance d, and generating all ones at distance d+1
  size_t left = 0;      // if qindex surpass this limit I generated all nodes at distance d+1
  size_t right = 1;     // limit for the nodes at distance d+1
  size_t d = 0;         // distance from a in the current interval

  while( qindex < q.size() ) {
    for ( unsigned int i = 1; i <= N; ++i ) {
      ztree<N> newone = q[qindex];
      newone ^ i;

      if ( newone == q[qindex] )
        continue;

      // se l'ho gia' visitato, passo oltre
      if ( queued.count( newone.to_ulong() ) > 0 )
        continue;

      // se l'ho trovato, esco
      if ( newone == b ) {
        visited = queued.size();
        return d + 1;
      }

      q.push_back( newone );
      queued.insert( newone.to_ulong() );
      right++;
    }

    qindex++;

    if ( qindex > left ) {
      left = right - 1;
      d++;
    }
  }

  visited = queued.size();
  return d;
}



int main ( int argv, char** argc ) {
  if ( argv < 2 || atoi( argc[1] ) <= 0 ) return 1;
  int runs = atoi( argc[1] );


  cout << "first\tsecond\topt\tdist\tcent\tvisited\n";
  // genera runs coppie di alberi casuali con n nodi
  for (int i = 0; i < runs; ++i ) {
    ztree<N> a;
    ztree<N> b;
    ptree<int> aa( a ), aaa( a );
    ptree<int> bb( b ), bbb( b );

    size_t visited;
    int toptimal, tdistance, toldistance;
    toptimal = distance(a, b, visited);
    tdistance = aa.distance( bb );
    toldistance = aaa.oldistance( bbb );

    assert( aa == bb );
    assert( aaa == bbb );
    assert( toptimal <= tdistance );
    size_t asd;
    assert( toptimal == distance(b,a,asd));

    cout << a << "\t" << b << "\t" << toptimal << "\t" << tdistance << "\t" << toldistance << "\t" << visited << endl;
  }

	return 0;
}


