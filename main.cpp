

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

#include "ztree.hpp"
#include "ptree.hpp"
#include "utils.hpp"
//#include "treegraph.hpp"
using namespace std;
using namespace tree;





//// stampa il grafico n vs PTREE_OPERATION vs ZTREE_OPERATION
//#define ROTATION_TEST	1
//#define RANDOM_TEST	1
//#define ASSIGN_TEST	1
//void benchmark ( int n ) {
//	int len = 1200000;
//	ptree<int> z ( n );
//	ztree t ( n );
	
//	//cout << sizeof( z ) << endl;
//	//cout << z << endl;
//	//z.to_root( 4 );
//	//cout << z << endl;
	
//	printf( "%d ", n );
//	#if ROTATION_TEST
//	//cout << "Test rotazioni (ricerca inclusa) su alberi di " << n << " nodi.\n";
//	//cout << len << " rotazioni casuali su un ptree<int>: ";
//	cout.flush( );
//	ttime( );
//	for ( int i = 0; i < len; i++ )
//		z.up( random() % n + 1 );
	
//	printf( "%.3lf ", ttime( ) );
//	//cout << z << endl;
	
//	//cout << len << " rotazioni casuali su un ztree: ";
//	cout.flush( );
//	ttime( );
//	for ( int i = 0; i < len; i++ )
//		t.rotate( random() % n + 1 );
	
//	printf( "%.3lf\n", ttime( ) );
//	//cout << z << endl;
//	#endif

//	#if RANDOM_TEST
//	//cout << "\nTest di generazione di alberi casuali di " << n << " nodi.\n";
//	//cout << ( len /= 10 ) << " generazioni casuali di ptree<int>: ";
//	cout.flush();
//	ttime( );
//	for ( int i = 0; i < len / 10; i++ )
//		ptree<int> z( n );
	
	
//	printf( "%.3lf ", ttime( ) );
	
	
//	//cout << len << " generazioni casuali di ztree: ";
//	cout.flush();
//	ttime( );
//	for ( int i = 0; i < len / 10; i++ )
//		ztree t( n );
	
	
//	printf( "%.3lf\n", ttime( ) );
//	#endif

//	#if ASSIGN_TEST
//	//cout << "\nTest di assegnamento di alberi di " << n << " nodi.\n";
//	//cout << ( len *= 10 ) << " assegnamenti di ptree<int>: ";
//	cout.flush();
	
//	ttime( );
//	ptree<int> zz( n );
//	for ( int i = 0; i < len; i++ )
//		zz = z;
	
//	printf( "%.3lf ", ttime( ) );
	
//	//cout << len << " assegnamenti di ztree: ";
//	cout.flush();
//	ttime( );
//	ztree tt( n );
//	for ( int i = 0; i < len; i++ )
//		tt = t;
	
//	printf( "%.3lf %d\n", ttime( ), tt.bytes() );
	
//	#endif
//}










///*

//// stampa i dati per il grafico n vs CENTRAL vs SIMPLIFY
//void test_rotation_algorithms ( int start, int n, int iterations, int step ) {
//	for ( int i = start; i <= n; i += step ) {
//		//double catalan = exp2( 2 * i ) / ( sqrt( M_PI * i ) * ( i + 1 ) );
//		//cout << (int) catalan << endl;
		
//		int simplify = 0;
//		int central = 0;
//		int centralp = 0;
//		for ( int j = iterations; j > 0; j-- ) {
//			ptree<int> zz ( i );
//			ptree<int> vv ( i );
//			ptree<int> zzz ( zz );
//			ptree<int> vvv ( vv );
//			ptree<int> zzzz ( zz );
//			ptree<int> vvvv ( vv );
			
		
//			if ( !( zz == zzz ) || !( vv == vvv ) ) {
//				cout << "Operator = failed...\n";
//				exit( 1 );
//			}
		
//			//cout << "OPTIMAL, SIMPLIFY, CENTRAL > ";
		
//			int simp = zz.simplify( vv );
//			simplify += simp;
		
//			int cent = zzz.central( vvv );
//			central += cent;
			
//			int cenp = zzzz.central_preprocessing( vvvv );
//			centralp += cenp;
		
//			if ( !( zz == vv ) ) {
//				cout << "SIMPLIFY failed...\n";
//				exit( 1 );
//			}
		
//			if ( !( zzz == vvv ) ) {
//				cout << "CENTRAL failed...\n";
//				exit( 1 );
//			}
//		}
	
//		printf( "%d %d %d %d\n", i, simplify, central, centralp );
//		//cout << double( optimal ) << " " << double( simplify ) << " " << double( central ) / iterations << endl;
//	}
//}




//// stampa il grafico n vs OPTIMAL vs CENTRAL vs SIMPLIFY
//void test_algorithms ( int start, int n, int iterations ) {
//	// STAMPA STATISTICHE
	
//	for ( int i = start; i <= n; i++ ) {
//		treegraph<> g( i );
//		int optimal = 0;
//		int simplify = 0;
//		int central = 0;
//		//cout << random( ) << endl;
//		for ( int j = iterations; j > 0; j-- ) {
//			ztree z	( i );
//			ztree v ( i );
//			ptree<int> zz ( z );
//			ptree<int> vv ( v );
//			ptree<int> zzz ( z );
//			ptree<int> vvv ( v );
		
//			if ( !( zz == zzz ) || !( vv == vvv ) ) {
//				cout << "Operator = failed...\n";
//				exit( 1 );
//			}
		
//			//cout << "OPTIMAL, SIMPLIFY, CENTRAL > ";
		
//			int tmp = 0;
//			tmp = g.distance( z, v );
//			optimal += tmp;
//			//int opt = tmp;
//			//cout << tmp << " / ";
		
//			tmp = zz.simplify( vv );
//			simplify += tmp;
//			//cout << tmp << " / ";
//			///if ( tmp > ( 2 * i - 6 ) ) {
//			//	printf( "opt = %d, sim = %d, bound = %d\n", opt, tmp, 2 * i - 6 );
//			//	getchar();
//			//}
		
//			tmp = zzz.central_preprocessing( vvv );
//			central += tmp;
//			//cout << tmp << "\n";
			
			
		
//			if ( !( zz == vv ) ) {
//				cout << "SIMPLIFY failed...\n";
//				exit( 1 );
//			}
		
//			if ( !( zzz == vvv ) ) {
//				cout << "CENTRAL failed...\n";
//				exit( 1 );
//			}
//		}
	
//		printf( "%d %d %d %d\n", i, optimal, simplify, central );
//		//cout << double( optimal ) << " " << double( simplify ) << " " << double( central ) / iterations << endl;
//	}
//}






//// stampa il grafico n vs OPTIMAL_COUNT vs GAP1_COUNT vs GAP2_COUNT vs TOTAL
//void test_simplify ( int start, int n, int iterations ) {
//	for ( int i = n; i >= start; i-- ) {
		
//		map<int, int> sresults;
//		//map<int, int> oresults;
//		//treegraph<> g ( i );
//		for ( int j = iterations; j > 0; j-- ) {
//			//ztree z	( i );
//			//ztree v ( i );
//			ptree<int> zz ( i );
//			ptree<int> vv ( i );
		
//			//int optimal = g.distance( z, v );
		
//			//oresults[ g.distance( z, v ) ]++;
//			sresults[ zz.central_preprocessing( vv ) ]++;
			
//			if ( !( zz == vv ) ) {
//				cout << "SIMPLIFY failed...\n";
//				exit( 1 );
//			}
			
//			if ( iterations % 20 == 0 ) {
//				cout.flush();
//				cout << 100.0 - 100.0 * j / double( iterations ) << "%\r";
//			}
//		}
	
//		printf( "%d nodes:\n", i );
//		int limit = 2 * i - ( i <= 4 ? 4 : i <= 10 ? 5 : 6 );
//		for ( int j = 0; j <= limit; j++ )
//			printf( "%d %d\n", j, sresults[j] / 10 );
//		//cout << double( optimal ) << " " << double( simplify ) << " " << double( central ) / iterations << endl;
//	}
//}

//*/


int main ( int argv, char** argc ) {
	if ( argv < 2 || atoi( argc[1] ) <= 0 ) return 1;
	int n = atoi( argc[1] );


	for ( int i = 0; i < 1000; i++ ) {
		ptree<int> aa ( n );
		ptree<int> bb ( n );

		cout << aa.simplify( bb ) << endl;

		//cout << aa << endl;
		//cout << bb << endl;

		if ( aa != bb )
			cout << "DEVI MORI'.\n" << endl;
	}

	
	/*treegraph<> g ( n );

	for ( int i = 0; i < 100000; i++ ) {
		ztree a ( n );
		ztree b ( n );
		ptree<int> aa ( a );
		ptree<int> bb ( b );
		//cout << aa.to_str() << endl;

		int opt = g.distance( a, b );
		int mix = aa.mixed( bb );

		if ( opt != mix ) {
			printf( "%d vs %d\n", opt, mix );
			cout << to_string( aa, bb ) << endl;
			getchar();
		}

#if DEBUG
		cout << "=========================================================================\n";
#endif
	}
	
	*/

	


	
	// TEST ALGORITMO SIMPLIFY (stampa il comportamento di simplify vs optimal)
	//test_simplify( 3, n, 1000000 );

	// BENCHMARK STRUTTURE DATI
	//for ( int i = 3; i <= n; i < 10 ? i++ : i < 30 ? i += 2 : i+= 5 )
	//for ( int i = 3; i <= n; i++ )
	//	benchmark( i );

	// STAMPA STATISTICHE
	//test_algorithms( 3, n, 1000 );
	//test_rotation_algorithms( 5, n, 10000, 5 );

	
	
	// TEST POSSIBILE MIGLIORAMENTO ALGORITMO
	/*
	srandom( 1239412341 );
	treegraph g( n );
	int optimal = 0;
	int simplify = 0;
	for ( int j = 0; j < 100000; j++ ) {
		printf( "%d\n", j );
		ztree z	( n );
		ztree v ( n );
		ptree<int> zz ( z );
		ptree<int> vv ( v );
	
		int tmp = 0;
		tmp = g.distance( z, v );
		optimal += tmp;
		//cout << tmp << " / ";
	
		int opt = tmp;
		bool stop;
		tmp = zz.simplify( vv, &stop );
		//printf( "distance = %d, %d.\n", tmp, stop );
		
		
		// XXX non si verifica mai quindi che l'algoritmo non raggiunge l'ottimo
		// se le scelte sono sempre fatte con cx = 1
		//if ( tmp != opt && !stop ) {
		//	printf( "distance = %d, %d\n", tmp, stop );
		//	getchar();
		//}
		int best = tmp;
		if ( best != opt ) {
			printf( "distance = %d vs %d, %d.\n", tmp, opt, stop );
			cout << double_print( ptree<int>( z ), ptree<int>( v ) ) << endl;
			
			int attempts = 0;
			while( best != opt ) {
				ptree<int> aa( z );
				ptree<int> bb( v );
				tmp = aa.simplify( bb );
				if ( tmp < best ) { best = tmp; }
				//printf( "distance = %d.\n", tmp );
				attempts++;
			}
			
			if ( best != opt ) {
				cout << "SIMPLIFY fallisce sui seguenti alberi (ottiene " << best << " contro " << opt << "):\n" << double_print( ptree<int>( z ), ptree<int>( v ) ) << endl;
				//getchar();
			}
		}
		
		
		simplify += best;
		//cout << tmp << " / ";
	
	
		if ( !( zz == vv ) ) {
			cout << "SIMPLIFY failed...\n";
			exit( 1 );
		}
	}
	
	printf( "%d %d %d\n", n, optimal, simplify );*/
	
	return 0;
}


