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
#include "node_set.hpp"
//#include "treegraph.hpp"
using namespace std;
using namespace tree;



int main ( int argv, char** argc ) {
	if ( argv < 2 || atoi( argc[1] ) <= 0 ) return 1;
	int n = atoi( argc[1] );

//	cout << sizeof( bimap<int,int> ) << endl;

  for ( int i = 0; i < 1000; i++ ) {
		ptree<int> aa ( n );
    ptree<int> bb ( n );
    cout << "==================================\n";


    aa.simplify( bb );

    if ( aa != bb ) {
      cout << aa << endl;
      cout << bb << endl;
      getchar();
    }
  }
	
//	treegraph<> g ( n );

//	for ( int i = 0; i < 100000; i++ ) {
//		ztree a ( n );
//		ztree b ( n );
//		ptree<int> aa ( a );
//		ptree<int> bb ( b );
//		//cout << aa.to_str() << endl;

//		int opt = g.distance( a, b );
//		int mix = aa.mixed( bb );

//		if ( opt != mix ) {
//			printf( "%d vs %d\n", opt, mix );
//			cout << to_string( aa, bb ) << endl;
//			getchar();
//		}

//#if DEBUG
//		cout << "=========================================================================\n";
//#endif
//	}

	return 0;
}


