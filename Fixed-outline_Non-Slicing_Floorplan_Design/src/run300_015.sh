#!/bin/bash

for i in {0..10000}
do
    ../bin/hw3 ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_015.floorplan 0.15 $i >> n300_015.txt
done