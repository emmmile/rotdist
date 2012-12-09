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
//#include <unordered_set>
#include "config.hpp"
#include "algo.hpp"
using namespace std;
using namespace tree;




/*
void testfunctions ( size_t runs ) {
  size_t i;
  Random gen( 0 );// (unsigned long) &i );


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


    if ( toptimal < tdistance )
    cout << a << "\t" << b << "\t" << toptimal << "\t" << tdistance << "\t" << toldistance << "\t" << visited << endl;
  }
}
*/

int main ( int argv, char** argc ) {
  while ( true ) {
    Random gen( 0 );// (unsigned long) &i );
    ztree<N> a( gen );
    ztree<N> b( gen );
    ptree<int> aa( a );
    ptree<int> bb( b );

    //print<int>( aa, bb );
    int one = test( aa, bb );
    //int two = aaa.make_all_equivalent( bbb );
    //if ( one != two ) {
    //print<int>( aa, bb );

    size_t visited;
    cout << one << " vs " << distance(a, b, visited ) << endl;
    //getchar();
    //}
  }



  /*if ( argv != 3 || atoi( argc[1] ) <= 0 || atoi( argc[2] ) <= 0 ) return 1;
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

        return 0;*/
}


