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
//#include "treegraph.hpp"
using namespace std;
using namespace tree;

typedef ptree<int> treet;

int main ( int argv, char** argc ) {
	if ( argv < 2 || atoi( argc[1] ) <= 0 ) return 1;
	int n = atoi( argc[1] );

  for ( int i = 0; i < 20000; i++ ) {
    treet a ( n );
    treet b ( n );
    treet c ( a );
    treet d ( b );
    treet first( a );
    treet second( b );

    int dist, simp;
    if ( (dist = a.distance(b)) > (simp = c.simplify(d)) + 5 ) {
      print<int>( first, second );
      first.make_equivalent( second );
      second.make_equivalent( first );
      print<int>( first, second );
      cout << "SIMPLIFY took " << simp << ", while ROT took " << dist << " rotations.\n";
    }
  }

	return 0;
}


