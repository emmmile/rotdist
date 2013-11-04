#!/bin/bash


for i in {11..50}
do 

  g++ -DNODES=$i -o main.o -c -Wall -O3 -pipe -funroll-loops -std=c++0x -Irandom main.cpp
  g++ -o main main.o -lpthread
  
  ./main 1 500 > $i.txt
  ./average.py $i.txt >> results.txt
done


