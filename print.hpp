#ifndef PRINT_HPP
#define PRINT_HPP   1
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ostream>
#include <string>
#include <sstream>
#include <iomanip>
#include "node.hpp"
#include "ptree.hpp"
#include "equivalence.hpp"
using namespace std;
namespace tree {


template <class T> class ptree;






template <class T>
class print {

  struct asciinode {
    asciinode *left, *right;

    //length of the edge from this node to its children
    int edge_length;
    //int height;

    int value;
  };



  int logdec ( int n ) {
    int i = 1;
    while ( n >= 10 ) n /= 10, i++;

    return i;
  }



  static const int max_height = 1000;
  static const int infinity = 1 << 20;

  int lprofile[max_height];
  int rprofile[max_height];

  //adjust gap between left and right nodes
  static const int gap = 3;

  //used for printing next node in the same level,
  //this is the x coordinate of the next char printed
  int print_next;
  stringstream out;
  uint width;

  asciinode * build_ascii_tree_recursive(node<T>* t, node<T>* base )
  {
    asciinode * node;

    if (t == NULL ) return NULL;

    node = (asciinode*) malloc(sizeof(asciinode));
    node->left = build_ascii_tree_recursive(t->left() ? t->left() + base : NULL, base);
    node->right = build_ascii_tree_recursive(t->right() ? t->right() + base : NULL, base);

    node->value = t->value();

    return node;
  }


  //Copy the tree into the ascii node structre
  asciinode * build_ascii_tree(node<T>* t, node<T>* base)
  {
    asciinode *node;
    if (t == NULL) return NULL;
    node = build_ascii_tree_recursive(t, base);
    return node;
  }

  void free_ascii_tree( asciinode* u ) {
    if ( u == NULL ) return;

    free_ascii_tree( u->left );
    free_ascii_tree( u->right );
    free( u );
  }







  //The following function fills in the lprofile array for the given tree.
  //It assumes that the center of the label of the root of this tree
  //is located at a position (x,y).  It assumes that the edge_length
  //fields have been computed for this tree.
  void compute_lprofile(asciinode *node, int x, int y, bool isleft )
  {
    int i;//, isleft;
    if (node == NULL) return;
    //isleft = (node->parent_dir == -1);
    lprofile[y] = min(lprofile[y], x-((logdec( node->value )-isleft)/2));
    if (node->left != NULL)
    {
      for (i=1; i <= node->edge_length && y+i < max_height; i++)
      {
        lprofile[y+i] = min(lprofile[y+i], x-i);
      }
    }
    compute_lprofile(node->left, x-node->edge_length-1, y+node->edge_length+1, true);
    compute_lprofile(node->right, x+node->edge_length+1, y+node->edge_length+1, false);
  }

  void compute_rprofile(asciinode *node, int x, int y, bool notleft)
  {
    int i;//, notleft;
    if (node == NULL) return;
    //notleft = (node->parent_dir != -1);
    rprofile[y] = max(rprofile[y], x+((logdec( node->value )-notleft)/2));
    if (node->right != NULL)
    {
      for (i=1; i <= node->edge_length && y+i < max_height; i++)
      {
        rprofile[y+i] = max(rprofile[y+i], x+i);
      }
    }
    compute_rprofile(node->left, x-node->edge_length-1, y+node->edge_length+1, false);
    compute_rprofile(node->right, x+node->edge_length+1, y+node->edge_length+1, true);
  }

  //This function fills in the edge_length and
  //height fields of the specified tree
  int compute_edge_lengths(asciinode *node) {
    if (node == NULL) return 0;
    if ( node->right == NULL && node->left == NULL ) {
      node->edge_length = 0;
      return 1;
    }

    int hmin, i, delta;
    int hl = compute_edge_lengths(node->left);
    int hr = compute_edge_lengths(node->right);



    if (node->left != NULL) {
      //for (i=0; i< hl; i++)
      //  rprofile[i] = -INFINITY;

      compute_rprofile(node->left, 0, 0, false);
      //hmin = hl;
    } else   hmin = 0;



    if (node->right != NULL) {
      //for (i=0; i< hr; i++)
      //  lprofile[i] = INFINITY;

      compute_lprofile(node->right, 0, 0, false);
      //hmin = min(hr, hmin);
    } else   hmin = 0;
    hmin = min( hr, hl );



    delta = 4;
    for (i=0; i<hmin; i++)
    {
      delta = max(delta, gap + 1 + rprofile[i] - lprofile[i]);
    }

    //If the node has two children of height 1, then we allow the
    //two leaves to be within 1, instead of 2
    if ( ( hl == 1 || hr == 1) && delta >= 4 )
      delta--;

    node->edge_length = ((delta+1)/2) - 1;

    //now fill in the height of node
    //XXX calcola l'altezza compresa quella degli archi (prende la massima fra destra e sinistra
    hl = node->left  ? hl + node->edge_length + 1 : 1;
    hr = node->right ? hr + node->edge_length + 1 : 1;

    return max( hl, hr );
  }













  //This function prints the given level of the given tree, assuming
  //that the node has the given x cordinate.

  void print_level(ostream& stream, asciinode *node, int x, int level, bool isleft, equivalence_info<T>* eqinfo = NULL)
  {
    int i;//, isleft;
    if (node == NULL) return;
    //isleft = (node->parent_dir == -1);
    if (level == 0)
    {
      for (i=0; i<(x-print_next-((logdec( node->value )-isleft)/2)); i++)
      {
        stream << ' ';
      }
      print_next += i;
      stream << node->value;
      print_next += logdec( node->value );
    }
    else if (node->edge_length >= level)
    {
      if (node->left != NULL)
      {
        for (i=0; i<(x-print_next-(level)); i++)
        {
          stream << ' ';
        }
        print_next += i;
        if ( !eqinfo || ( eqinfo && (*eqinfo)[node->left->value] == EMPTY ) ||
                node->edge_length != level )
          stream << "/";
        else  stream << "X";
        print_next++;
      }
      if (node->right != NULL)
      {
        for (i=0; i<(x-print_next+(level)); i++)
        {
          stream << ' ';
        }
        print_next += i;
        if ( !eqinfo || ( eqinfo && (*eqinfo)[node->right->value] == EMPTY ) ||
            node->edge_length != level )
          stream << "\\";
        else  stream << "X";
        print_next++;
      }
    }
    else
    {
      print_level(stream, node->left, x-node->edge_length-1, level-node->edge_length-1, true, eqinfo );
      print_level(stream, node->right, x+node->edge_length+1, level-node->edge_length-1, false, eqinfo );
    }
  }


  //prints ascii tree for given Tree structure
  void print_tree(ostream& stream, node<T>* t, node<T>* base, equivalence_info<T>* eqinfo = NULL ) {
    asciinode *proot;
    int xmin, i;
    //if (t == NULL) return;

    // XXX questa costruisce solo la struttura dell'albero, non fa' niente
    proot = build_ascii_tree(t, base);


    memset( lprofile, infinity, max_height );
    memset( rprofile, -infinity, max_height );

    // calcola le lunghezze degli archi nn capisco bene in che modo
    int h = compute_edge_lengths(proot);
    for (i=0; i< h && i < max_height; i++)
    {
      lprofile[i] = infinity;
    }
    compute_lprofile(proot, 0, 0, 0);


    // calcola la coordinata x rispetto a cui disegnare
    xmin = 0;
    for (i = 0; i < h && i < max_height; i++)
      if ( lprofile[i] < xmin ) xmin = lprofile[i];


    for (i = 0; i < h; i++)
    {
      print_next = 0;
      print_level(stream, proot, -xmin, i, 0, eqinfo );
      stream << "\n";
    }
    if (h >= max_height)
    {
      printf("(This tree is taller than %d, and may be drawn incorrectly.)\n", max_height);
    }


    free_ascii_tree( proot );
    pad();
  }

  void pad ( ) {
    int maxbuf = 512;
    vector<string> lines;

    while( !out.eof() ) {
      char line[maxbuf];
      out.getline( line, maxbuf );
      lines.push_back( string(line) );
      if ( strlen(line) > width ) width = strlen(line);
    }

    out.str(std::string());
    out.clear();

    for ( uint i = 0; i < lines.size(); ++i )
      out << setw(width) << left << lines[i] << endl;
  }

  string adjust( const string& line, uint width ) {
    stringstream tmp;
    tmp << setw(width) << right << line;
    return tmp.str();
  }

  void merge ( print& other ) {
    stringstream final;
    int maxbuf = 512;
    string delim = "   ";

    while( !out.eof() ) {
      char tline[maxbuf];
      char sline[maxbuf];
      out.getline( tline, maxbuf );

      if ( !other.out.eof() ) {
        other.out.getline( sline, maxbuf );
        //final.width( width + other.width + delim.length() );
        final << adjust(tline,width) << delim << adjust(sline,other.width) << endl;
      } else {
        final << tline << endl;
      }
    }

    while( !other.out.eof() ) {
      char sline[maxbuf];
      other.out.getline( sline, maxbuf );
      final.width( width + other.width + delim.length() );
      final << sline << endl;
    }

    out.str(std::string());
    out.clear();
    out << final.str();
  }

  print( const ptree<T>& t, equivalence_info<T>& info ): width( 0 ) {
    print_tree( out, &(t[t.root()]), t.base(), &info );
  }

public:
  print( const ptree<T>& t, ostream& stream = cout ) : width( 0 ) {
    print_tree( out, &(t[t.root()]), t.base() );
  }

  print( const ptree<T>& t, const ptree<T>& s, ostream& stream = cout ) : width( 0 ){

    print_tree( out, &(t[t.root()]), t.base() );
    print<T> second( s );
    merge( second );
    stream << out.str();
  }

  print( const ptree<T>& t, const ptree<T>& s, equivalence_info<T>& eqinfo, ostream& stream = cout ) : width( 0 ){
    //equivalence_info<T> eqinfo( t.size() );
    t.equal_subtrees( s, eqinfo );


    print_tree( out, &(t[t.root()]), t.base(), &eqinfo );
    print<T> second( s, eqinfo.inverse() );
    merge( second );
    stream << out.str();
  }

  friend ostream& operator << ( ostream& stream, const print<T>& t ) {
    return stream << t.out.str();
  }
};



}

#endif
