#!/usr/bin/env python
# -*- coding: utf-8 -*-


import sys
import math

def statistics ( f ):
  sopt = vopt = salg = valg = scen = vcen = 0
  num = 0
  for l in open( f ):
    num += 1
    if num == 1:
      continue
    (opt, alg, cen), rest = l.split()[2:5], l.split()[5:]
    sopt += int( opt )
    vopt += int( opt ) * int( opt )
    salg += int( alg )
    valg += int( alg ) * int( alg )
    scen += int( cen )
    vcen += int( cen ) * int( cen )

  print( f )  
  print( "algo\taverage\t\tvariance" )
  print( "opt\t{0}\t{1}".format( sopt / float( num ), math.sqrt( vopt / float( num ) - ( sopt / float( num ) ) ** 2 ) ) )
  print( "new\t{0}\t{1}".format( salg / float( num ), math.sqrt( valg / float( num ) - ( salg / float( num ) ) ** 2 ) ) )
  print( "cen\t{0}\t{1}".format( scen / float( num ), math.sqrt( vcen / float( num ) - ( scen / float( num ) ) ** 2 ) ) )
    

if __name__ == '__main__':
	f = sys.argv[1]
	statistics( f )

