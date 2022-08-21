#!/bin/bash

for i in {0..10000}
do
    ../bin/hw3 ../testcases/n100.hardblocks ../testcases/n100.nets ../testcases/n100.pl ../output/n100_010.floorplan 0.10 $i >> n100_010.txt
done