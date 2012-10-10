#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>
#include <boost/bimap.hpp>

#include "ztree.hpp"
#include "ptree.hpp"
#include "treegraph.hpp"
using namespace std;
using namespace tree;

typedef ptree<int> treet;

int main ( int argv, char** argc ) {
  if ( argv < 3 || atoi( argc[1] ) <= 0 || atoi( argc[2] ) <= 0 ) return 1;
	int n = atoi( argc[1] );
  int runs = atoi( argc[2] );

  ztree a( "110010110110000", 7 );
  ztree b( "110111000010100", 7 );
  treet aa( a );
  treet bb( b );
  print<int>( aa, bb );

  cout << aa.distance( bb ) << endl;
  print<int>( aa, bb );
  treegraph<> opt( 7 );
  cout << opt.distance( a, b ) << endl;


//  int optimal = 0, distance = 0, oldistance = 0;
//  treegraph<> opt( n );
//  for (int i = 0; i < runs; ++i ) {
//    ztree a( n );
//    ztree b( n );
//    treet aa( a ), aaa( a );
//    treet bb( b ), bbb( b );

//    int toptimal, tdistance, toldistance;
//    toptimal = opt.distance(a, b);
//    tdistance = aa.distance( bb );
//    toldistance = aaa.oldistance( bbb );
//    assert( aa == bb );
//    assert( aaa == bbb );


//    if ( toptimal < tdistance ) {
//      treet first( a );
//      treet second( b );
//      print<int>( first, second );
//      print<int>( aa, bb );

//      getchar();
//    }

//    optimal += toptimal;
//    distance += tdistance;
//    oldistance += toldistance;


//    cout << "===================\n";
//  }


//  cout << "OPTIMAL: " << optimal / (double) runs << endl;
//  cout << "DISTANCE: " << distance / (double) runs << endl;
//  cout << "OLDISTANCE: " << oldistance / (double) runs << endl;

//  for ( int i = 0; i < 20; i++ ) {
//    treet a ( n ); treet b ( n );
//    treet c ( a ); treet d ( b );

//    int dist, simp;
//    dist = a.distance(b);
//    simp = c.simplify(d);
//    assert( a == b );
//    assert( c == d );

//    cout << simp << "\t" << dist << endl;
//  }

	return 0;
}


