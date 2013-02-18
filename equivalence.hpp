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
  T* __left;
  T* __right;
  bool __allocated;
  T __size;

  equivalence_info<T>* __inverse;

  // constructor for the inverse view, the memory is shared
  equivalence_info( T* l, T* r, equivalence_info* i ) : __inverse( i ) {
    __left  = l;
    __right = r;
    __size = i->__size;
    __allocated = false;
  }

public:
  equivalence_info( uint size ) {
    __left  = new T [size + 1];
    __right = new T [size + 1];
    __allocated = true;
    __size = size;
    __inverse = new equivalence_info( __right, __left, this );
    clear( 0, size + 1 );
  }

  ~equivalence_info ( ) {
    if ( !__allocated ) return;
    delete[] __left;
    delete[] __right;
    delete __inverse;
  }

  inline T operator [] ( T value ) const {
    return __left[value];
  }

  inline T set ( T a, T b ) {
    __left[a]  = b;
    __right[b] = a;
    return b;
  }

  inline void clear ( T beg, T end ) {
    fill( __left  + beg, __left  + end, EMPTY );
    fill( __right + beg, __right + end, EMPTY );
  }

  T size() const {
    return __size;
  }

  inline equivalence_info& inverse ( ) {
    return *__inverse;
  }

  friend ostream& operator << ( ostream& stream, const equivalence_info<T>& e ) {
    for ( T i = 1; i < e.size(); ++i ) printf( "%2d ", e.__left[i] );  printf( "\n" );
    for ( T i = 1; i < e.size(); ++i ) printf( "%2d ", e.__right[i] ); printf( "\n" );
    return stream;
  }
};

}

#endif // EQUIVALENCE_HPP
