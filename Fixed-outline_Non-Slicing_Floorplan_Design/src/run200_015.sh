#!/bin/bash

for i in {0..10000}
do
    ../bin/hw3 ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_015.floorplan 0.15 $i >> n200_015.txt
done