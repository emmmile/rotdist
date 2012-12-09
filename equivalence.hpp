#ifndef EQUIVALENCE_HPP
#define EQUIVALENCE_HPP
#include <iostream>
using namespace std;
namespace tree {


// this class is similar to a simplified bimap
// the left view is contained in *this
// the right view instead in this->inverse()
// when one view is updated, also the other is.
template<class T = unsigned int>
class equivalence_info {
private:
  T* left;
  T* right;
  bool allocated;
  T dim;

  equivalence_info<T>* __inverse;

  // constructor for the inverse view, the memory is shared
  equivalence_info( T* l, T* r, equivalence_info* i ) : __inverse( i ) {
    this->left  = l;
    this->right = r;
    allocated = false;
  }

public:
  equivalence_info( uint size ) {
    left  = new T [size + 1];
    right = new T [size + 1];
    fill( left,  left + size + 1,  EMPTY );
    fill( right, right + size + 1, EMPTY );
    allocated = true;
    dim = size;
    __inverse = new equivalence_info( right, left, this );
  }

  ~equivalence_info ( ) {
    if ( !allocated ) return;
    delete[] left;
    delete[] right;
    delete __inverse;
  }

  inline T operator [] ( T value ) const {
    return left[value];
  }

  inline T set ( T a, T b ) {
    left[a]  = b;
    right[b] = a;
    return b;
  }

  T size() const {
    return dim;
  }

  inline equivalence_info& inverse ( ) {
    return *__inverse;
  }
};



}

#endif // EQUIVALENCE_HPP
