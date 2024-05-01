#include "instance.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
//#include <boost/filesystem.hpp>

Instance::Instance(std::string instanceFile) {
	read_input(instanceFile);
}

void Instance::read_input(std::string inputFile)
{
	std::ifstream instream(inputFile);
	if (!instream.is_open()) {
		std::cout << "Unable to open file\n";
		exit(0);
	}

	std::string input;
	std::getline(instream, input);
	for (int i = 0; i<10; i++){
		std::getline(instream, input);
		std::vector<std::string> lines;
		std::stringstream ss(input);
		std::string line;

		while (std::getline(ss, line, ',')) {
			lines.push_back(line);
		}
		
		enrollments.insert({ lines[1], lines[5] });
		for (auto x : enrollments) {
			std::cout << x.first << " " << x.second << std::endl;
		}
	}



}