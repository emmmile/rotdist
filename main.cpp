#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <cassert>
#include <deque>
#include <fstream>
#include <algorithm>
#include <thread>
#include <mutex>
//#include <unordered_set>
#include "config.hpp"
#include "algo.hpp"
using namespace std;
using namespace tree;

mutex output_mutex;



void test_algorithms ( size_t runs, int index ) {
  size_t i;
  Random gen( index );// (unsigned long) &i );
  //double optimal = 0, algonew = 0;


  for (i = 0; i < runs; ++i ) {
    ztree<N> a( gen );
    ztree<N> b( gen );
    ptree<int> aa( a ), aaa( a );
    ptree<int> bb( b ), bbb( b );

    if ( has_equivalent(aa,bb) ) { --i; continue; } // only very bad trees

    size_t visited;
    int toptimal, tdistance, tcentral;
    toptimal = distance(a, b, visited );
    tdistance = newalgo( aa, bb );
    tcentral = newbetteralgo( aaa, bbb );
    //algonew += tdistance;
    //optimal += toptimal;

    assert( aa == bb );
    //assert( aaa == bbb );
    assert( toptimal <= tdistance );

    output_mutex.lock();
    //if ( toptimal < tdistance )
    cout << a << "\t" << b << "\t" << toptimal << "\t" << tdistance << "\t" << tcentral << "\t" << visited << endl;
    output_mutex.unlock();  
}

  //cout << "OPTIMAL AVG = " << optimal / runs << endl;
  //cout << "NEWALGO AVG = " << algonew / runs << endl;
}


int main ( int argv, char** argc ) {
  /*ifstream ifs( "17.new.txt" );
  string temp;

  getline( ifs, temp ); //skip first
  while ( getline( ifs, temp ) ) {
    int index = temp.find_first_of('\t');

    string a = temp.substr( 0, index );
    string b = temp.substr( index + 1, 35 );

    ztree<17> aa( a );
    ztree<17> bb( b );

    ptree<int> aaa( aa );
    ptree<int> bbb( bb );

    if ( !has_equivalent(aaa, bbb) ) {
      cout << temp.substr( 0, temp.find_last_of("\t") ) << "\t";
      cout << central( aaa, bbb ) << endl;
    }
  }

  return 0;*/

  if ( argv != 3 || atoi( argc[1] ) <= 0 || atoi( argc[2] ) <= 0 ) return 1;
  size_t threads = atoi( argc[1] );
  size_t runs = atoi( argc[2] );

  cout << "first\tsecond\topt\tdist\tcentral\tvisited\n";
  vector<thread> t;

  //Launch a group of threads
  for (int i = threads; i > 0; --i) {
    size_t part = runs / i;
    int j = i;
    t.push_back( thread( test_algorithms, part, j ) );
    runs -= part;
  }

  //Join the threads with the main thread
  for( auto& i : t ) i.join();

    return 0;
}


