#ifndef EQUIVALENCE_HPP
#define EQUIVALENCE_HPP
#include <iostream>
using namespace std;


namespace tree {


template<class T = unsigned int>
class equivalence_info {
private:
	T* info;
	T* inv;
	bool allocated;
  T _size;

	equivalence_info( T* a, T* b ) {
		this->info = a;
		this->inv = b;
		allocated = false;
	}

public:
	equivalence_info( uint size ) {
		info = new T [size + 1];
		inv = new T [size + 1];
		fill( info, info + size + 1, EMPTY );
		fill( inv, inv + size + 1, EMPTY );
		allocated = true;
    this->_size = size;
	}

	~equivalence_info ( ) {
		if ( !allocated ) return;
		delete[] info;
		delete[] inv;
	}

	inline T operator [] ( T value ) const {
		return info[value];
	}

	inline T set ( T a, T b ) {
		info[a] = b;
		inv[b] = a;
		return b;
	}

  T size() const {
    return _size;
  }

	inline equivalence_info inverse ( ) {
		return equivalence_info( inv, info );
	}
};


}

#endif // EQUIVALENCE_HPP
