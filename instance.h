#pragma once
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


class Instance {
public:
    std::string instanceName;
    std::unordered_map<std::string, std::vector<std::string>> students;
    std::unordered_map<std::string, std::string> enrollments;
    std::unordered_map<std::string, std::vector<std::string>> exams;

    Instance(std::string instanceFile);

private:
    void read_input(const std::string inputFile);
};

#endif //ES_INSTANCE_H