#!/bin/bash

for i in {0..10000}
do
    ../bin/hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_015.floorplan 0.15 $i >> n100_015.txt
done