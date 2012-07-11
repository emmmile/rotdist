
#ifndef _NODE_HPP
#define _NODE_HPP	1
#include <iostream>
#define EMPTY		0

using namespace std;

namespace tree {


template <class T>
class node {
private:
	T val;	// valore
	T ls;	// valore a sinitra
	T rs;	// valore a destra
	T fa;	// valore del padre
public:

	node ( ) {

	}
	
	node ( T v, T l, T r, T f ) 
		: val( v ), ls( l ), rs( r ), fa( f ) {
	}	

	T value ( ) const {
		return val;
	}

	T left ( ) const {
		return ls;
	}

	T right ( ) const {
		return rs;
	}

	T father ( ) const {
		return fa;
	}
	
	void set_value ( T value ) {
		val = value;
	}
	
	void set_father ( T value ) {
		fa = value;
	}
	
	void set_left ( T value ) {
		ls = value;
	}
	
	void set_right ( T value ) {
		rs = value;
	}

	T set ( T v, T l, T r, T f ) {
		val = v;
		ls = l;
		rs = r;
		fa = f;
		return val;
	}

	void simple_set ( T l, T r, T f ) {
		ls = l;
		rs = r;
		fa = f;
	}

	bool operator < ( const node<T>& t ) const {
		if ( val == t.val ) {
			if ( ls == t.ls ) {
				if ( rs == t.rs ) {
					return fa < t.fa;
				} else return rs < t.rs;
			} else return ls < t.ls;
		} else return val < t.val;
	}
	
	bool operator == ( const node<T>& t ) const {
		return ( ls == t.ls ) && ( rs == t.rs ) && ( fa == t.fa ) && ( val == t.val );
	}

	friend ostream& operator << ( ostream& s, const node<T>& t ) {
		s << "value = " << t.val << ": [l = " << t.ls << "], [r = " << t.rs << "], [f = " << t.fa << "]";
		return s;
	}
};

}

#endif

