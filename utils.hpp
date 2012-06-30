

#ifndef UTILS_HPP
#define UTILS_HPP	1
#include "ptree.hpp"
#include "print.hpp"



// per i benchmark
double ttime ( void ) {
	static struct timeval t;
	static double old = 0.0, newsample = 0.0;
	
	gettimeofday ( &t, NULL );
	
	old = newsample;
	newsample = t.tv_sec + t.tv_usec * 0.000001;
	return newsample - old;
}






template <class T> class ptree;

// ancora per la stampa
template <class T>
string to_string ( const ptree<T>& t, int& width, int extraspace, T* eqinfo = NULL ) {
	string str = t.to_str( eqinfo );
	//cout << s;

	width = 0;
	for ( int i = 0; i < (int) str.size();  ) {
		int tmp = str.find_first_of( '\n', i ) - i;

		if ( tmp > width ) width = tmp;
		i += tmp + 1;
	}

	//cout << width + 1 << endl;
	width++;

	for ( int i = 0; i < (int) str.size(); ) {
		int pos = str.find_first_of( '\n', i );
		int len = pos - i;

		str.insert( pos, width + extraspace - len, ' ' );

		i += width + extraspace + 1;
	}

	return str;
}


template <class T>
string to_string ( ptree<T>& a, ptree<T>& b, T* eqinfo = NULL, T* seqinfo = NULL ) {
	int wa, extraspace = a.vertices();

	string sa = to_string( a, wa, extraspace, eqinfo );
	string sb = b.to_str( seqinfo );

	int j = 0;
	for ( int i = 0; i < (int) sa.size() && j < (int) sb.size(); ) {
		int apos = sa.find_first_of( '\n', i );

		int bpos = sb.find_first_of( '\n', j );
		sa.insert( apos, sb, j, bpos - j );
		i = apos + bpos - j + 1;
		j = bpos + 1;
	}

	// se per caso sb e' piu' lungo di sa...
	for ( ; j < (int) sb.size(); ) {
		int bpos = sb.find_first_of( '\n', j );
		sa.append( wa + extraspace, ' ' );
		sa.append( sb, j, bpos - j );
		sa.append( 1, '\n' );

		j = bpos + 1;
	}

	return sa;
}


#endif
