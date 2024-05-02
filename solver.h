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
    std::vector<int> days;
    std::vector<int> slot_in_day;
    std::vector<int> rooms;
    std::vector<std::vector<int>> conflict;
    std::map<std::string, std::set<int>> student_in_exam;
    std::map<int, int> num_student_in_exam;
    std::vector<int> capacity;
    std::vector<std::vector<int>> enrolment;
    std::multimap<std::string, std::string> enrollments;

    void run();
};

#endif