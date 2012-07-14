#ifndef NODE_SET_HPP
#define NODE_SET_HPP
#include <vector>
#include <ostream>
#include "equivalence.hpp"
using namespace std;
namespace tree {

// implementation of a simple set, that has maximum size equal to the number of
// nodes in a tree
template<class T>
class node_set {
  vector<T> nodes;
public:
  typedef typename vector<T>::iterator iterator;

  node_set( T size ) {
    nodes.reserve( size + 1 );
    nodes.push_back( EMPTY );
  }

  node_set( equivalence_info<T>& e ) {
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
    for ( iterator i = begin(); i < end(); ++i ) {
      if ( *i == value ) return true;
    }

    return false;
  }

  bool add ( T value ) {
    if ( contains( value ) ) return false;

    nodes.push_back( value );
    return true;
  }

  friend ostream& operator << ( ostream& stream, node_set<T>& s ) {
    stream << "{ ";
    for ( node_set<T>::iterator i = s.begin(); i < s.end(); ++i )
      stream << *i << " ";
    return stream << "}";
  }
};


}

#endif // NODE_SET_HPP
