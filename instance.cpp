#include "instance.h"
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

	// Read file
	std::string input;
	std::getline(instream, input);
	for (int i = 0; i < 5000; i++) {
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

	std::cout << "Enrollment size: " << enrollments.size() << std::endl;

	// List of exams: it is sorted by increasing order
	for (auto x : exam_input) {
		exams.push_back(x);
	}
	std::cout << "Number of exam: " << exams.size();
	std::cout << std::endl;

	//list of students
	for (auto x : student_input) {
		students.push_back(x);
	}
	std::cout << "Number of student: " << students.size();
	std::cout << std::endl << std::endl;

	// Exam encoding: 24032024.TXQTVH04.0001 -> 0; 24032024.TXQTVH04.0001: 0 for example
	std::map <std::string, int> mapping_exam;
	for (int i = 0; i < exams.size(); i++) {
		mapping_exam.insert({ exams[i], i });
	}

	/*std::cout << "Exam mapping: \n";
	for (auto x : mapping_exam) {
		std::cout << x.first << " " << x.second << std::endl;
	}*/

	// Student encoding: 19200232 -> 0; 19200354 -> 1 for example
	std::map <std::string, int> mapping_student;
	for (int i = 0; i < students.size(); i++) {
		mapping_student.insert({ students[i], i });
	}

	/*std::cout << "\nStudent mapping: \n";
	for (auto x : mapping_student) {
		std::cout << x.first << " " << x.second << std::endl;
	}*/

	// Enrolment matrix: B_li if student l is enrolled in exam i
	int row = exams.size();
	int col = students.size();
	enrolment.resize(col, std::vector<int>(row, 0));

	for (auto x : enrollments) { //enrollments là danh sách đăng ký môn
		enrolment[mapping_student[x.first]][mapping_exam[x.second]] = 1;
		student_in_exam[x.second].insert(mapping_student[x.first]);
	}

	/*std::cout << "\nStudent enrollment: \n";
	for (int i = 0; i < enrolment.size(); i++) {
		std::cout << "Student " << i << " ";
		for (int j = 0; j < enrolment[i].size(); j++) {
			std::cout << enrolment[i][j] << " ";
		}
		std::cout << std::endl;
	}*/

	//std::cout << "\nStudent in each exam: \n";
	//for (auto x : student_in_exam) {
	//	std::cout << x.first << " ";
	//	for (int y : x.second) {
	//		std::cout << y << " ";
	//	}
	//	std::cout << std::endl;
	//}

	// Build the matrix of number of student in each exam
	for (auto x : student_in_exam) {
		num_student_in_exam[mapping_exam[x.first]] = x.second.size();
	}

	//std::cout << "\nNumber of student in each exam\n";
	//for (auto x : num_student_in_exam) {
	//	std::cout << x.first << " " << x.second << std::endl;
	//}

	// Build the conflix matrix: Cii' = 1 if exam i and i' have the same student.
	conflict.resize(row, std::vector<int>(row, 0));

	for (auto it1 = student_in_exam.begin(); it1 != student_in_exam.end(); ++it1) {
		for (int value : it1->second) {
			for (auto it2 = std::next(it1); it2 != student_in_exam.end(); ++it2) {
				if (it2->second.count(value)) {
					conflict[mapping_exam[it1->first]][mapping_exam[it2->first]] = 1;
					conflict[mapping_exam[it2->first]][mapping_exam[it1->first]] = 1;
				}
			}
		}
	}

	//std::cout << "\nConflict Matrix\n";
	//for (int i = 0; i < conflict.size(); i++) {
	//	for (int j = 0; j < conflict[i].size(); j++) {
	//		std::cout << conflict[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}


	// Making up days, slot_in_day rooms and capacity of each room data
	// 6 days: 3 T7, 3 CN
	for (int i = 0; i < 8; i++) {
		days.push_back(i);
	}

	// 6 slots/day:
	for (int i = 0; i < 6; i++) {
		slot_in_day.push_back(i);
	}

	// 15 rooms
	for (int i = 0; i < 15; i++) {
		rooms.push_back(i);
	}

	for (int i = 0; i < 15; i++) {
		capacity.push_back(40);
	}
}