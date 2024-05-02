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
    std::vector<int> days;
    std::vector<int> slot_in_day;
    std::vector<int> rooms;
    std::vector<std::vector<int>> conflict;
    std::map<std::string, std::set<int>> student_in_exam;
    std::map<int, int> num_student_in_exam;
    std::vector<std::vector<int>> enrolment;
    std::vector<int> capacity;
    std::multimap<std::string, std::string> enrollments;

    Instance(std::string instanceFile);

private:
    void read_input(const std::string inputFile);
};

#endif //ES_INSTANCE_H