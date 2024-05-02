#pragma once

#ifndef ES_INSTANCE_H
#define ES_INSTANCE_H
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <string>
#include <set>

class Instance {
public:
    //std::string instanceName;
    std::vector<std::string> students;
    std::vector<std::string> exams;
    std::vector<int> time_period;
    std::vector<std::vector<int>> enrolment;
    std::multimap<std::string, std::string> enrollments;

    Instance(std::string instanceFile);

private:
    void read_input(const std::string inputFile);
};

#endif //ES_INSTANCE_H