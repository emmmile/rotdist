#!/bin/bash


for i in {11..18}
do 

  #g++ -DN=$i -o main.o -c -Wall -O3 -pipe -funroll-loops -std=c++0x -Irandom main.cpp
  #g++ -o main main.o -lpthread
  
  #./main 4 1000 > $i.txt
  ./average.py results/$i.txt >> results.txt
done


