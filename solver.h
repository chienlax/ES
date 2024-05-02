#pragma once

#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include "instance.h"
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN
typedef IloArray<IloNumVarArray> NumVar2D;

class Solver {
public:
	Solver(Instance*);
    Instance* instance;

    std::vector<std::string> students;
    std::vector<std::string> exams;
    std::vector<int> time_period;
    std::vector<std::vector<int>> enrolment;

    void run();
};

#endif