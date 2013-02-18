#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-


import sys
import math

def statistics ( f ):
  sopt = vopt = salg = valg = smix = vmix = scen = vcen = 0
  num = 0
  for l in open( f ):
    num += 1
    if num == 1:
      continue
    (opt, alg, mix, cen), rest = l.split()[2:6], l.split()[6:]
    sopt += int( opt )
    vopt += int( opt ) * int( opt )
    salg += int( alg )
    valg += int( alg ) * int( alg )
    smix += int( mix )
    vmix += int( mix ) * int( mix )
    scen += int( cen )
    vcen += int( cen ) * int( cen )

  print( f )  
  print( "algo\tavg\tvar" )
  print( "opt\t{0:.2f}\t{1:.2f}".format( sopt / float( num ), math.sqrt( vopt / float( num ) - ( sopt / float( num ) ) ** 2 ) ) )
  print( "new\t{0:.2f}\t{1:.2f}".format( salg / float( num ), math.sqrt( valg / float( num ) - ( salg / float( num ) ) ** 2 ) ) )
  print( "mix\t{0:.2f}\t{1:.2f}".format( smix / float( num ), math.sqrt( vmix / float( num ) - ( smix / float( num ) ) ** 2 ) ) )
  print( "cen\t{0:.2f}\t{1:.2f}".format( scen / float( num ), math.sqrt( vcen / float( num ) - ( scen / float( num ) ) ** 2 ) ) )
    

if __name__ == '__main__':
	f = sys.argv[1]
	statistics( f )

