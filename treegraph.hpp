
#ifndef _TREEGRAPH_HPP
#define _TREEGRAPH_HPP  1
#include <iostream>
#include <queue>
#include <map>
#include <boost/bimap.hpp>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/visitors.hpp>
#include "ztree.hpp"
#include "ptree.hpp"

#define VERBOSE   0
using namespace std;
using namespace boost;
using namespace tree;



struct max_value {
  unsigned int max;
  max_value( ) : max( 0 ) { }
  void operator() ( unsigned int x ) { if ( x > max ) max = x; }
};


// la classe T indica il tipo di albero, la classe E il tipo di etichetta da usare per gli 
// archi e per la numerazione dei vertici
template <class T = ztree, typename E = unsigned int>
class treegraph {
  // definisco un grafo che utilizza un vector per le liste di adiacenza e
  // un vector per la lista dei nodi
  typedef adjacency_list<vecS, vecS, undirectedS> graph_type;
  // definisco il tipo dell'arco
  typedef pair<E, E> edge_type;
  // definisco il tipo dell'array usato per il calcolo delle distanze
  typedef graph_traits<graph_type>::vertices_size_type dist_type;

private:
  E n;
  std::queue<T> q;
  bimap<T, E> m;
  graph_type g;
  dist_type* dist;

public:
  treegraph ( E num ) : n( num ), m( ), dist( NULL ) {
    build( );
    dist = new dist_type [num_vertices( g )];
  }

  ~treegraph ( ) {
    if ( dist ) delete[] dist;
  }

  E num_trees ( ) {
    return num_vertices( g );
  }


  // costruzione del grafo
  void build ( ) {
    // prendo un albero casuale come albero 0.. che generalita' ragazzi.. ^^
    T zero( n );
    q.push( zero );
    set<edge_type> edges;
    m.insert( typename bimaps::bimap<T,E>::value_type( zero, m.size() ) );

    while ( !q.empty() ) {
      T t = q.front();
      q.pop();

      E tindex = m.left.find( t )->second;

      // genero tutti i possibili alberi a partire da quello selezionato
      for ( typename T::node_type i = 1; i <= n; i++ ) {
        T s = t;
        // per non perdere di generalita' utilizzo l'operatore ^, implementato
        // sia in ptree che in ztree
        s ^ i;

        if ( s == t ) continue;

        // se non c'e' lo aggiungo
        if ( m.left.find( s ) == m.left.end() ) {
          m.insert( typename bimaps::bimap<T,E>::value_type( s, m.size() ) );
          q.push( s );
        }

        // prendo l'indice dell'albero s
        E index = m.left.find( s )->second;


        // aggiungo l'arco fra t e s
        if ( VERBOSE ) cout << "Adding edge " << index << ", " << tindex << endl;

        if ( index <= tindex )
          edges.insert( edge_type( index, tindex ) );
        else   edges.insert( edge_type( tindex, index ) );
      }
    }

    g = graph_type( edges.begin(), edges.end(), m.size() );

    //if ( VERBOSE )
    //cout << "\nGenerated " << m.size() << " different trees." << endl;
  }

  void full_search ( ) {
    cout << "Search path lenghts on all trees: 0%\r";
    cout.flush( );

    E max = 0;
    int perc = 0;

    for ( E i = 0; i < num_vertices( g ); i++ ) {
      std::fill_n( dist, num_vertices( g ), 0 );

      breadth_first_search( g, vertex( i, g ),
        visitor( make_bfs_visitor( record_distances( dist, on_tree_edge() ) ) ) );

      max_value result = for_each( dist, dist + num_vertices( g ), max_value() );
      if ( VERBOSE )
        cout << "Max lenght for paths starting from tree " << i << " is " << result.max << ".\n";

      if ( result.max > max ) max = result.max;

      // stampa una percentuale per avere un idea del punto a cui sono...
      if ( int( i * 100.0f / num_vertices( g ) ) >= int( perc + 1 ) ) {
        perc++;
        cout << "Search path lenghts on all trees: " << perc << "%\r";
        cout.flush( );
      }
    }

    cout << "\nMax lenght for all paths between trees of " << n << " nodes is " << max << ".\n";
  }

  void full_minmax ( ) {
    cout << "Calculating minmax lenghts.\n";

    double min = 100 * n;

    for ( E i = 0; i < num_vertices( g ); i++ ) {
      std::fill_n(dist, num_vertices( g ), 0 );

      breadth_first_search( g, vertex( i, g ),
        visitor( make_bfs_visitor( record_distances( dist, on_tree_edge() ) ) ) );

      max_value result = for_each( dist, dist + num_vertices( g ), max_value() );
      if ( result.max <= min ) {
        min = result.max;
        cout << "Found tree distant " << min << " from any other tree.\n";
      }
    }
  }

  void get_paths ( T a, T b ) {
    if ( a.size() != b.size() || a.size() != n ) return;

    E start = m.project_right( m.left.find( a ) )->first;
    E end = m.project_right( m.left.find( b ) )->first;

    std::fill_n(dist, num_vertices( g ), 0 );
    breadth_first_search( g, vertex( start, g ), visitor( make_bfs_visitor( record_distances( dist, on_tree_edge() ) ) ) );
    E total = dist[end];

    for ( E i = 0; i < num_vertices( g ); i++ ) {
      if ( dist[i] != 1 ) continue;

      graph_traits<graph_type>::vertices_size_type tmp [ num_vertices( g ) ];
      std::fill_n(tmp, num_vertices( g ), 0 );
      breadth_first_search( g, vertex( i, g ), visitor( make_bfs_visitor( record_distances( tmp, on_tree_edge() ) ) ) );

      if ( tmp[end] == total - 1 ) {
        printf( "Found one tree with distance %d\n", total - 1 );
        //cout << ptree<E>( m.project_left( m.right.find( i ) )->first ) << endl;
      }
    }
  }

  void random_average ( E num ) {
    cout << "Calculating average lenghts starting from " << num << " random trees.\n";

    E random_vertex;
    double average = 0.0;

    for ( E i = 0; i < num; i++ ) {
      std::fill_n(dist, num_vertices( g ), 0 );
      random_vertex = random() % num_vertices( g );

      breadth_first_search( g, vertex( random_vertex, g ),
        visitor( make_bfs_visitor( record_distances( dist, on_tree_edge() ) ) ) );

      double sum = 0.0;
      for ( E j = 0; j < num_vertices( g ); j++ ) sum += dist[j];
      sum /= num_vertices( g ) - 1;

      average += sum;
    }

    cout << "Average is " << average / num << ".\n";
  }


  // calcolo della distanza ottima fra a e b
  E distance ( const T a, const T b ) {
    if ( a.vertices() != b.vertices() || a.vertices() != n ) return -1;

    E j = m.project_right( m.left.find( a ) )->first;
    E i = m.project_right( m.left.find( b ) )->first;

    std::fill_n(dist, num_vertices( g ), 0 );
    breadth_first_search( g, vertex( i, g ), visitor( make_bfs_visitor( record_distances( dist, on_tree_edge() ) ) ) );

    return dist[j];
  }


  void random_max_path_search ( E num ) {
    cout << "Search path lenghts starting from " << num << " random trees.\n";

    E random_vertex;
    E max = 0;

    for ( E i = 0; i < num; i++ ) {
      std::fill_n(dist, num_vertices( g ), 0 );
      random_vertex = random() % num_vertices( g );


      breadth_first_search( g, vertex( random_vertex, g ),
        visitor( make_bfs_visitor( record_distances( dist, on_tree_edge() ) ) ) );

      max_value result = for_each( dist, dist + num_vertices( g ), max_value() );
      if ( result.max > max ) {
        max = result.max;
        cout << "Found path(s) of lenght " << max << ".\n";
      }

      for ( E i = 0; i < num_vertices( g ); i++ ) {
        if ( dist[i] == max ) {

          if ( VERBOSE ) {
            ptree<typename T::node_type> first( m.project_left( m.right.find( i ) )->first );
            ptree<typename T::node_type> second( m.project_left( m.right.find( random_vertex ) )->first );

            //cout << first.to_string_with( second ) << endl << endl;
            //cout << first << endl;
            //cout << second << endl;
          }
        }
      }
    }
  }
};

#endif

