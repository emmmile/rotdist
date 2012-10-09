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
	if ( argv < 2 || atoi( argc[1] ) <= 0 ) return 1;
	int n = atoi( argc[1] );

//  ztree a( "111010011100101110010101000101001101000", 19 );
//  ztree b( "111100111100110011100100010010010010100", 19 );
//  treet aa( a );
//  treet bb( b );
//  print<int>( aa, bb );

//  cout << aa.distance( bb ) << endl;



  int runs = 5000;
  int optimal = 0, distance = 0, oldistance = 0;
  treegraph<> opt( n );
  for (int i = 0; i < runs; ++i ) {
    ztree a( n );
    ztree b( n );
//    cout << a << endl << b << endl;


    treet aa( a ), aaa( a );
    treet bb( b ), bbb( b );
  //  cout << opt.distance( a, b ) << endl;
  //  cout << aa.distance( bb ) << endl;
  //  cout << aaa.oldistance( bbb ) << endl;
    optimal += opt.distance(a, b);
    distance += aa.distance( bb );
    oldistance += aaa.oldistance( bbb );
    assert( aa == bb );
    assert( aaa == bbb );
  }


  cout << "OPTIMAL: " << optimal / (double) runs << endl;
  cout << "DISTANCE: " << distance / (double) runs << endl;
  cout << "OLDISTANCE: " << oldistance / (double) runs << endl;

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


