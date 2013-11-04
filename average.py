#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

# http://stackoverflow.com/questions/7551219/deleting-columns-from-a-file-with-awk-or-from-command-line-on-linux
import sys
import math

def statistics ( f ):
  sopt = vopt = salg = valg = smix = vmix = scen = vcen = 0
  num = 0
  for l in open( f ):
    if l.startswith( "first" ) or len( l ) < 2:
      continue
    
    num += 1
      
    #print( l.strip() )    
    
    fields = l.split()    
    opt = fields[2]
    alg = fields[3]
    cen = fields[4]
    
    sopt += int( opt )
    vopt += int( opt ) * int( opt )
    salg += int( alg )
    valg += int( alg ) * int( alg )
    #smix += int( mix )
    #vmix += int( mix ) * int( mix )
    scen += int( cen )
    vcen += int( cen ) * int( cen )      
    
    if num >= 1000:
      break

  
  print( f )  
  print( "algo\tavg\tvar" )
  print( "opt\t{0:.3f}\t{1:.3f}".format( sopt / float( num ), math.sqrt( vopt / float( num ) - ( sopt / float( num ) ) ** 2 ) ) )
  print( "new\t{0:.3f}\t{1:.3f}".format( salg / float( num ), math.sqrt( valg / float( num ) - ( salg / float( num ) ) ** 2 ) ) )
  #print( "mix\t{0:.3f}\t{1:.3f}".format( smix / float( num ), math.sqrt( vmix / float( num ) - ( smix / float( num ) ) ** 2 ) ) )
  print( "cen\t{0:.3f}\t{1:.3f}".format( scen / float( num ), math.sqrt( vcen / float( num ) - ( scen / float( num ) ) ** 2 ) ) )
    

if __name__ == '__main__':
	f = sys.argv[1]
	statistics( f )

