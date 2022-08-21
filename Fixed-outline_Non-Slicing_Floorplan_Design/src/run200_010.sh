#!/bin/bash

for i in {0..10000}
do
    ../bin/hw3 ../testcases/n200.hardblocks ../testcases/n200.nets ../testcases/n200.pl ../output/n200_010.floorplan 0.10 $i >> n200_010.txt
done