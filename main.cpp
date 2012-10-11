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

  // test specifico su un albero che mi da noia
//  ztree a( "110010110110000", 7 );
//  ztree b( "110111000010100", 7 );
//  treet aa( a );
//  treet bb( b );
//  print<int>( aa, bb );

//  cout << aa.distance( bb ) << endl;
//  print<int>( aa, bb );
//  treegraph<> opt( 7 );
//  cout << opt.distance( a, b ) << endl;


  // genera runs coppie di alberi casuali con n nodi
  // comparando la distanza trovata tramite l'algoritmo che stiamo provando
  // con l'ottimo (alla fine viene stampata una media).
  int optimal = 0, distance = 0, oldistance = 0;
  treegraph<> opt( n );
  for (int i = 0; i < runs; ++i ) {
    ztree a( n );
    ztree b( n );
    treet aa( a ), aaa( a );
    treet bb( b ), bbb( b );

    int toptimal, tdistance, toldistance;
    toptimal = opt.distance(a, b);
    tdistance = aa.distance( bb );
    toldistance = aaa.oldistance( bbb );
//    if ( aa != bb ) {
//        treet first( a );
//              treet second( b );
//              print<int>( first, second );
//        print<int>( aa, bb );
//    }

    assert( aa == bb );
    assert( aaa == bbb );


//    if ( toptimal < tdistance ) {
//      cout << "===================\n";
//      treet first( a );
//      treet second( b );
//      print<int>( first, second );
//      print<int>( aa, bb );

//      getchar();
//    }

    optimal += toptimal;
    distance += tdistance;
    oldistance += toldistance;
  }


  cout << "OPTIMAL: " << optimal / (double) runs << endl;
  cout << "DISTANCE: " << distance / (double) runs << endl;
  cout << "OLDISTANCE: " << oldistance / (double) runs << endl;


	return 0;
}


