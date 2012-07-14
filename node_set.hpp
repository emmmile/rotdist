#ifndef NODE_SET_HPP
#define NODE_SET_HPP
#include <vector>
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

  iterator begin ( ) {
    return nodes.begin() + 1;
  }

  iterator end ( ) {
    return nodes.end();
  }

  void update ( equivalence_info<T>& e ) {
    for ( T i = i; i <= e.size(); ++i ) {
      if ( e[i] == EMPTY || contains( i ) ) continue;

      nodes.push_back( i );
    }

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
  }
};


}

#endif // NODE_SET_HPP
