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
	for (int i = 0; i < 3000; i++) {
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

	// Find what exam has more than 40 students and split it into smaller exam
	// Tmp map to store student in their respective exam
	// Cái map này có thể thay cho enrollment luôn, làm thành 1 cái map
	// lưu exam và sinh viên thi tương ứng thì tức đấy chính là enrollment của thằng đó luôn
	
	for (auto x : enrollments) { //enrollments là danh sách đăng ký môn từ file csv
		StudentInEachExam[x.second].insert(x.first);
	}

	/*for (auto it : StudentInEachExam) {
		std::cout << it.first << ": " << it.second.size() << std::endl;
	}*/

	// If an exam has more than 40 student, split it into 2, for fuck sake, and put it into a new map
	std::vector<std::string> nameOfExamToDelete;
	std::map<std::string, std::set<std::string>> newExamSet;

	for (auto it : StudentInEachExam) {
		std::set<std::string> oldStudentSet = it.second; // og student set
		if (oldStudentSet.size() > 40) {
			std::vector <std::set<std::string>> newStudentSet; // vector chứa student của 2 exam mới
			std::set<std::string> tmp;
			auto it2 = it.second.begin();

			int i = 0;
			while (it2 != it.second.end() && i < 40) {
				tmp.insert(*it2); //tmp đang chứa 40 học sinh đầu
				advance(it2, 1);
				i++;
			}

			newStudentSet.push_back(tmp);
			tmp.clear();

			while (it2 != it.second.end()) {
				tmp.insert(*it2); //tmp đang chứa phần còn lại
				advance(it2, 1);
			}

			newStudentSet.push_back(tmp);
			tmp.clear();

			std::string oldExamName = it.first;
			nameOfExamToDelete.push_back(it.first);

			for (int j = 0; j < newStudentSet.size(); j++) {
				std::string newExamName = oldExamName + "." + std::to_string(j + 1);
				newExamSet[newExamName] = newStudentSet[j];
			}
		}
	}

	// Delete the existed exam that exceed 40 students:
	for (int i = 0; i < nameOfExamToDelete.size(); i++) {
		auto it = StudentInEachExam.find(nameOfExamToDelete[i]);
		if (it != StudentInEachExam.end()) {
			StudentInEachExam.erase(it);
		}
	}

	// Push the new set of exam into the map:
	for (auto it : newExamSet) {
		StudentInEachExam.insert({ it.first, it.second });
	}

	std::cout << "\nAfter splitting: \n" << "Number of exam: " << StudentInEachExam.size() << std::endl << std::endl;

	/*std::cout << "\nNumber of student in each exam after split: \n";

	for (auto it : StudentInEachExam) {
		std::cout << it.first << ": " << it.second.size() << std::endl;
	}*/

	// Exam encoding: 24032024.TXQTVH04.0001 -> 0; 24032024.TXQTVH04.0001: 0 for example
	std::map <std::string, int> mapping_exam;
	int count = 0;
	for (auto it : StudentInEachExam) {
		mapping_exam.insert({ it.first, count++ });
	}

	//for (int i = 0; i < exams.size(); i++) {
	//	mapping_exam.insert({ exams[i], i });
	//}

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

	// Trước khi mò tiếp tạo 1 cái map mới map student với exam mới tương ứng:
	std::multimap<std::string, std::string> new_enrollments;
	for (auto it : StudentInEachExam) {
		std::set<std::string> student_set = it.second;
		for (auto it1 : student_set) {
			new_enrollments.insert({it1, it.first});
		}
	}

	// Enrolment matrix: B_li if student l is enrolled in exam i

	int row = StudentInEachExam.size();
	int col = students.size();
	enrolment.resize(col, std::vector<int>(row, 0));

	for (auto x : new_enrollments) { //new_enrollments là danh sách đăng ký môn, với ds môn mới
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

	/*std::cout << "\nStudent in each exam: \n";
	for (auto x : student_in_exam) {
		std::cout << x.first << " ";
		for (int y : x.second) {
			std::cout << y << " ";
		}
		std::cout << std::endl;
	}*/

	// Build the matrix of number of student in each exam
	for (auto x : student_in_exam) {
		num_student_in_exam[mapping_exam[x.first]] = x.second.size();
	}

	/*std::cout << "\nNumber of student in each exam\n";
	for (auto x : num_student_in_exam) {
		std::cout << x.first << " " << x.second << std::endl;
	}*/

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
	// 4 days: 3 T7, 3 CN
	for (int i = 0; i < 4; i++) {
		days.push_back(i);
	}

	// 6 slots/day:
	for (int i = 0; i < 6; i++) {
		slot_in_day.push_back(i);
	}

	// 10 rooms
	for (int i = 0; i < 10; i++) {
		rooms.push_back(i);
	}

	for (int i = 0; i < 15; i++) {
		capacity.push_back(40);
	}
}