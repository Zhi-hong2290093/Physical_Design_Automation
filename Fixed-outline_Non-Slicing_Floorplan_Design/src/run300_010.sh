#!/bin/bash

for i in {0..10000}
do
    ../bin/hw3 ../testcases/n300.hardblocks ../testcases/n300.nets ../testcases/n300.pl ../output/n300_010.floorplan 0.10 $i >> n300_010.txt
done