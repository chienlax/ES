#include "solver.h"
#include <iostream>

Solver::Solver(Instance* instance) : instance(instance) {
    students = instance->students;
    exams = instance->exams;
    time_period = instance->time_period;
    enrolment = instance->enrolment;
}

void Solver::run() {
    IloEnv env;

        int n = exams.size();
        //std::cout << "Exam size:" << n << std::endl;
        int q = students.size();
        //std::cout << "Students size:" << q << std::endl;
        int p = time_period.size();

        IloModel model(env); 

        // Decision Variable
        IloArray<IloNumVarArray> X(env, n);
        for (int i = 0; i < n; i++) {
            X[i] = IloNumVarArray(env, p);
            for (int j = 0; j < p; j++) {
                X[i][j] = IloNumVar(env, 0, 1, ILOINT);
            }
        }

        //Constraint 1: an exam must be assigned exactly to one time slot
        for (int i = 0; i < n; i++) {
            IloExpr expr1(env);
            for (int k = 0; k < p; k++) {
                expr1 += X[i][k];
            }
            model.add(expr1 == 1);
        }

        
        //Constraint 2: no student shall attend two exams scheduled at the same time slot.
        for (int k = 0; k < p; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        IloNumExpr expr(env);
                        for (int h = 0; h < q; h++) {
                            expr += X[i][k] * X[j][k] * enrolment[i][h] * enrolment[j][h];
                        }
                        model.add(expr == 0);
                    }
                }
            }
        }
        IloExpr s(env);
        model.add(IloMinimize(env, s));
        IloCplex cplex(model);
        if (!cplex.solve()) {
            cerr << "Failed to solve the problem" << endl;
            throw(-1);
        }

        cout << "Solution status: " << cplex.getStatus() << endl;

        for (int i = 0; i < n; ++i) {
            for (int k = 0; k < p; ++k) {
                cout << "X[" << i << "][" << k << "] = " << cplex.getValue(X[i][k]) << endl;
            }
        }

}