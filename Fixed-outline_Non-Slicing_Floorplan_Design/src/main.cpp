#include "GlobalTimer/GlobalTimer.hpp"
#include "Parser/Parser.hpp"
#include "SASolver/SASolver.hpp"
#include <iostream>
#include <cstdio>
#include <string>

int main(int argc, char *argv[])
{
    int time = 600;

    if( argc != 7)
    {
        std::cerr<<"Numbers of input file : ERROR!\n";
        exit(1);
    }

    GlobalTimer globalTimer(time);
    globalTimer.startTimer("runtime");
    Parser parser;

    globalTimer.startTimer("parse input");
    auto input = parser.parse(argv);
    globalTimer.stopTimer("parse input");

    globalTimer.startTimer("sa process");
    SASolver solver(input, globalTimer);
    solver.solve();
    globalTimer.stopTimer("sa process");

    globalTimer.startTimer("parse output");
    solver.write_result(argv[4]);
    globalTimer.stopTimer("parse output");

    globalTimer.stopTimer("runtime");
    globalTimer.printTime("parse input");
    globalTimer.printTime("sa process");
    globalTimer.printTime("parse output");
    globalTimer.printTime("runtime");

    return 0;
}