#include "instance.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>

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

	std::unordered_set<std::string> exam_input, student_input;

	//read file
	std::string input;
	std::getline(instream, input);
	for (int i = 0; i < 100; i++) {
		std::getline(instream, input);
		std::vector<std::string> lines;
		std::stringstream ss(input);
		std::string line;

		while (std::getline(ss, line, ',')) {
			lines.push_back(line);
		}
		
		enrollments.insert({ lines[1], lines[5] });
		
		exam_input.insert(lines[5]);
		student_input.insert(lines[1]);
	}

	std::cout << "Enrollment size = " << enrollments.size() << std::endl;

	int i = 0;
	for (auto x : enrollments) {
		std::cout << i << "|" << x.first << " " << x.second << std::endl;
		i++;
	}

	//list of exams
	for (auto x : exam_input) {
		//std::cout << x << " ";
		exams.push_back(x);
	}
	std::cout << std::endl;

	//list of students
	for (auto x : student_input) {
		//std::cout << x << " ";
		students.push_back(x);
	}
	std::cout << std::endl;

	//exam encoding: 24032024.TXQTVH04.0001 -> 0; 24032024.TXQTVH04.0001: 0 for example
	std::map <std::string, int> mapping_exam;
	for (int i = 0; i < exams.size(); i++) {
		mapping_exam.insert({ exams[i], i });
	}

	/*for (auto x : mapping_exam) {
		std::cout << x.first << " " << x.second << std::endl;
	}*/

	//student encoding: 19200232 -> 0; 19200354 -> 1 for example
	std::map <std::string, int> mapping_student;
	for (int i = 0; i < students.size(); i++) {
		mapping_student.insert({ students[i], i });
	}

	/*for (auto x : mapping_student) {
		std::cout << x.first << " " << x.second << std::endl;
	}*/

	//enrolment matrix
	int row = exams.size();
	int col = students.size();
	enrolment.resize(row, std::vector<int>(col, 0));

	for (auto x : enrollments) { //enrollments là danh sách đăng ký môn
		enrolment[mapping_exam[x.second]][mapping_student[x.first]] = 1;
	}

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			std::cout << enrolment[i][j] << " ";
		}
		std::cout << std::endl;
	}

	//time_period
	for (int i = 0; i < 6; i++) {
		time_period.push_back(i);
	}
}