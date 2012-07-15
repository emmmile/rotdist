#ifndef NODE_SET_HPP
#define NODE_SET_HPP
#include <vector>
#include <ostream>
#include <unordered_set>
#include "equivalence.hpp"
using namespace std;
namespace tree {

// implementation of a simple set, that has maximum size equal to the number of
// nodes in a tree
template<class T>
class node_set {
  vector<T> nodes;
  T __size;

public:
  node_set( T size ) {
    __size = 0;
    nodes.reserve( size + 1 );
    nodes.push_back( EMPTY );
  }

  node_set( equivalence_info<T>& e ) {
    nodes.reserve( e.size() + 1 );
    nodes.push_back( EMPTY );
    update( e );
  }

  // adds to this the elements of e not contained in s
  void update ( equivalence_info<T>& e ) {
    for ( T i = 1; i <= e.size(); ++i ) {
      if ( e[i] == EMPTY ) continue;

      nodes.push_back( i );
      ++__size;
    }
  }

  // adds to this the elements of e not contained in s
  void update ( node_set& s, equivalence_info<T>& e ) {
    for ( T i = 1; i <= e.size(); ++i ) {
      if ( e[i] == EMPTY || s.contains( i ) ) continue;

      nodes.push_back( i );
      ++__size;
    }
  }

  bool contains ( T value ) {
    for ( typename vector<T>::iterator i = nodes.begin() + 1; i < nodes.end(); ++i ) {
      if ( *i == value ) return true;
    }

    return false;
  }

  T next ( ) {
    --__size;
    T out = nodes.back();
    nodes.pop_back();
    return out;
  }

  bool add ( T value ) {
    if ( contains( value ) ) return false;

    nodes.push_back( value );
    ++__size;
    return true;
  }

  T size ( ) {
    return __size;
  }

  friend ostream& operator << ( ostream& stream, node_set<T>& s ) {
    stream << "{ ";
    for ( typename vector<T>::iterator i = s.nodes.begin() + 1; i < s.nodes.end(); ++i )
      stream << *i << " ";
    return stream << "}";
  }
};



template<class T>
class unordered_node_set {
  unordered_set<T> nodes;

public:
  unordered_node_set( T size ) {
    nodes.reserve( 2 * size );
  }

  unordered_node_set( equivalence_info<T>& e ) {
    for ( T i = 1; i <= e.size(); ++i ) {
      if ( e[i] == EMPTY ) continue;

      nodes.insert( i );
    }
  }

  // adds to this the elements of e not contained in s
  void update ( unordered_node_set& s, equivalence_info<T>& e ) {
    for ( T i = 1; i <= e.size(); ++i ) {
      if ( e[i] == EMPTY || s.contains(i) ) continue;

      nodes.insert( i );
    }
  }

  bool contains ( T value ) {
    return ( nodes.find( value ) != nodes.end() );
  }

  T next ( ) {
    T out = *nodes.begin();
    nodes.erase( out );
    return out;
  }

  void add ( T value ) {
    nodes.insert( value );
  }

  T size ( ) {
    return nodes.size();
  }

  friend ostream& operator << ( ostream& stream, node_set<T>& s ) {
    stream << "{ ";

    return stream << "}";
  }
};



}

#endif // NODE_SET_HPP
