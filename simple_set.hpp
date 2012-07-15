#ifndef NODE_SET_HPP
#define NODE_SET_HPP
#include <vector>
#include <ostream>
#include "equivalence.hpp"
using namespace std;
namespace tree {

// implementation of a simple set, that has the property of be iterable during
// insertions (not always in case of deletions).
template<class T>
class simple_set {
  vector<T> nodes;
public:
  typedef typename vector<T>::iterator iterator;

  simple_set( T size ) {
    nodes.reserve( size + 1 );
    nodes.push_back( EMPTY );
  }

  simple_set( equivalence_info<T>& e ) {
    nodes.reserve( e.size() + 1 );
    nodes.push_back( EMPTY );
    update( e );
  }

  void update ( equivalence_info<T>& e ) {
    for ( T i = 1; i <= e.size(); ++i ) {
      if ( e[i] == EMPTY || contains( i ) ) continue;

      nodes.push_back( i );
    }
  }

  iterator begin ( ) {
    return nodes.begin() + 1;
  }

  iterator end ( ) {
    return nodes.end();
  }

  bool contains ( T value ) {
    for ( iterator i = begin(); i < end(); ++i )
      if ( *i == value ) return true;

    return false;
  }

  bool insert ( T value ) {
    if ( contains( value ) ) return false;

    nodes.push_back( value );
    return true;
  }

  bool erase ( T value ) {
    for ( iterator i = begin(); i < end(); ++i )
      if ( *i == value ) {
        swap( nodes.back(), *i );
        nodes.pop_back();
        return true;
      }

    return false;
  }

  friend ostream& operator << ( ostream& stream, simple_set<T>& s ) {
    stream << "{ ";
    for ( simple_set<T>::iterator i = s.begin(); i < s.end(); ++i )
      stream << *i << " ";
    return stream << "}";
  }
};


}

#endif // NODE_SET_HPP
