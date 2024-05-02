#include "instance.h"
#include "solver.h"

int main() {
	Instance instance("C:\\Users\\ORLab\\Downloads\\DSSV.csv");
	Solver solver(&instance);
	solver.run();
}