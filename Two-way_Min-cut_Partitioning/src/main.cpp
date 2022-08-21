#include "FMSolver/FMSolver.hpp"
#include "GlobalTimer/GlobalTimer.hpp"
#include "Parser/Parser.hpp"
#include <cstdio>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Input File Error!\n";
        exit(1);
    }
    int seed, time;
    char file = (argv[2][-8]);

    if(file == '1')
    {
        time = 10 * 60 - 10;
        seed = 261;
    }
    else if(file == '2')
    {
        time = 10 * 60 - 10;
        seed = 904;
    }
    else if(file == '3')
    {
        time = 6 * 60 - 10;
        seed = 887;
    }
    else if(file == '4')
    {
        time = 10 * 60 - 80;
        seed = 519;
    }
    else
    {
        time = 10 * 60 - 50;
        seed = 226;
    }
    
    GlobalTimer globalTimer(time);
    globalTimer.startTimer("runtime");
    Parser parser;

    globalTimer.startTimer("parse input");
    auto input = parser.parse(argv);
    globalTimer.stopTimer("parse input");

    globalTimer.startTimer("FM process");
    FMSolver fmsolver(input, globalTimer);
    fmsolver.solve(seed);
    globalTimer.stopTimer("FM process");

    globalTimer.startTimer("parse output");
    fmsolver.write_result(argv[3]);
    globalTimer.stopTimer("parse output");

    globalTimer.stopTimer("runtime");
    globalTimer.printTime("parse input");
    globalTimer.printTime("FM process");
    globalTimer.printTime("parse output");
    globalTimer.printTime("runtime");

    return 0;
}