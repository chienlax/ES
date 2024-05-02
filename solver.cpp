#include "solver.h"
#include <iostream>

Solver::Solver(Instance* instance) : instance(instance) {
    students = instance->students;
    exams = instance->exams;
    enrolment = instance->enrolment;
    days = instance->days;
    slot_in_day = instance->slot_in_day;
    rooms = instance->rooms;
    conflict = instance->conflict;
    num_student_in_exam = instance->num_student_in_exam;
    capacity = instance->capacity;
}

void Solver::run() {
    IloEnv env;

        const int I = exams.size();
        const int J = days.size();
        const int K = slot_in_day.size();
        const int E = rooms.size();
        const int L = students.size();

        IloModel model(env); 

        // Decision Variable
        // X_ijke = 1 if exam i is schedule on day j, slot k, venue e
        IloArray<IloArray<IloArray<IloNumVarArray>>> X(env, I);
        for (int i = 0; i < I; i++) {
            X[i] = IloArray<IloArray<IloNumVarArray>>(env, J);
            for (int j = 0; j < J; j++) {
                X[i][j] = IloArray<IloNumVarArray>(env, K);
                for (int k = 0; k < K; k++) {
                    X[i][j][k] = IloNumVarArray(env, E);
                    for (int e = 0; e < E; e++) {
                        X[i][j][k][e] = IloNumVar(env, 0, 1, ILOINT);
                    }
                }
            }
        }

        // Y_lijk = 1 if student l has exam i on day j, slot k
        IloArray<IloArray<IloArray<IloNumVarArray>>> Y(env, L);
        for (int l = 0; l < L; l++) {
            Y[l] = IloArray<IloArray<IloNumVarArray>>(env, I);
            for (int i = 0; i < I; i++) {
                Y[l][i] = IloArray<IloNumVarArray>(env, J);
                for (int j = 0; j < J; j++) {
                    Y[l][i][j] = IloNumVarArray(env, K);
                    for (int k = 0; k < K; k++) {
                        Y[l][i][j][k] = IloNumVar(env, 0, 1, ILOINT);
                    }
                }
            }
        }

        //Objective function:
        IloNumVar T(env, 1, IloInfinity, ILOINT);
        IloExpr expr(env);
        expr += T;
        model.add(IloMinimize(env, expr));

        // (1): minimizes the exam’s period T
        for (int i = 0; i < I; i++) {
            IloExpr expr1(env);
            for (int j = 0; j < J; j++) {
                for (int k = 0; k < K; k++) {
                    for (int e = 0; e < E; e++) {
                        expr1 += (j + 1) * X[i][j][k][e];
                    }
                }
            }
            model.add(T - expr1 >= 0);
        }

        // (2): every exam is scheduled
        for (int i = 0; i < I; i++) {
            IloExpr expr2(env);
            for (int j = 0; j < J; j++) {
                for (int k = 0; k < K; k++) {
                    for (int e = 0; e < E; e++) {
                        expr2 += X[i][j][k][e];
                    }
                }
            }
            model.add(expr2 == 1);
        }

        // (3): there are no conflicts in the same time slot
        for (int i = 0; i < I; i++) {
            for (int i1 = 0; i1 < I; i1++) {
                if (i != i1) {
                    for (int j = 0; j < J; j++) {
                        for (int k = 0; k < K; k++) {
                            IloExpr expr3(env);
                            for (int e = 0; e < E; e++) {
                                expr3 += (X[i][j][k][e] + X[i1][j][k][e]);
                            }
                            expr3 *= conflict[i][i1];
                            model.add(expr3 <= 1);
                        }
                    }
                }
            }
        }

        // (4): there are no back-to-back scheduling
        for (int i = 0; i < I; i++) {
            for (int i1 = 0; i1 < I; i1++) {
                if (i != i1) {
                    for (int j = 0; j < J; j++) {
                        for (int k = 0; k < K-1; k++) {
                            IloExpr expr4(env);
                            for (int e = 0; e < E; e++) {
                                expr4 += (X[i][j][k][e] + X[i1][j][k+1][e]);
                            }
                            expr4 *= conflict[i][i1];
                            model.add(expr4 <= 1);
                        }
                    }
                }
            }
        }

        // (5): link students to their respective exams
        for (int l = 0; l < L; l++) {
            for (int i = 0; i < I; i++) {
                for (int j = 0; j < J; j++) {
                    for (int k = 0; k < K; k++) {
                        IloExpr expr5(env);
                        for (int e = 0; e < E; e++) {
                            expr5 += X[i][j][k][e];
                        }
                        expr5 *= enrolment[l][i];
                        model.add(expr5 == Y[l][i][j][k]);
                    }
                }
            }
        }

        // (6): constrains the number of exams scheduled in two days for each student: 5
        for (int l = 0; l < L; l++) {
            for (int j = 0; j < J-1; j++) {
                IloExpr expr6(env);
                for (int k = 0; k < K; k++) {
                    for (int i = 0; i < I; i++) {
                        expr6 += (Y[l][i][j][k] + Y[l][i][j + 1][k]);
                    }
                }
                model.add(expr6 <= 5);
            }
        }

        // (7): enforces the capacity limit of each exam venue
        //for (int i = 0; i < I; i++) {
        //    for (int e = 0; e < E; e++) {
        //        IloExpr expr7(env);
        //        for (int j = 0; j < J; j++) {
        //            for (int k = 0; k < K; k++) {
        //                expr7 += num_student_in_exam[i] * X[i][j][k][e];
        //            }
        //        }
        //        model.add(expr7 <= capacity[e]);
        //    }
        //}

        // (8): prevents scheduling multiple concurrent exams in the same venue
        for (int j = 0; j < J; j++) {
            for (int k = 0; k < K; k++) {
                for (int e = 0; e < E; e++) {
                    IloExpr expr8(env);
                    for (int i = 0; i < I; i++) {
                        expr8 += X[i][j][k][e];
                    }
                    model.add(expr8 <= 1);
                }
            }
        }

        IloCplex cplex(model);
        if (!cplex.solve()) {
            cerr << "Failed to solve the problem" << endl;
            throw(-1);
        }

        double objValue = cplex.getObjValue();
        //env.end();
        std::cout << "Objective Value: " << objValue << std::endl;

        cout << "Solution status: " << cplex.getStatus() << endl;

        /*for (int i = 0; i < n; ++i) {
            for (int k = 0; k < p; ++k) {
                cout << "X[" << i << "][" << k << "] = " << cplex.getValue(X[i][k]) << endl;
            }
        }*/
}
//Constraint 0: The last period have to be min
        /*for (int i = 0; i < n; i++) {
            IloExpr expr0(env);
            for (int k = 0; k < p; k++) {
                expr0 += X[i][k] * (k+1);
            }
            model.add(T >= expr0);
        }*/

        //Constraint 1: an exam must be assigned exactly to one time slot
        /*for (int i = 0; i < n; i++) {
            IloExpr expr1(env);
            for (int k = 0; k < p; k++) {
                expr1 += X[i][k];
            }
            model.add(expr1 == 1);
        }*/

        //Constraint 2: no student shall attend two exams scheduled at the same time slot.
        /*for (int k = 0; k < p; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i != j) {
                        IloNumExpr expr(env);
                        for (int h = 0; h < q; h++) {
                            if (enrolment[i][h] * enrolment[j][h] == 1) {
                                model.add(X[i][k] + X[j][k] <= 1);
                            }
                        }
                    }
                }
            }
        }*/

        //Constraint 3: Total students in 1 period cannot exceed the maximum capacity
        /*for (int i = 0; i < p; i++) {
            capacity.push_back(450);
        }

        for (int k = 0; k < p; k++) {
            IloNumExpr expr3(env);
            for (int i = 0; i < n; i++) {
                for (int h = 0; h < q; h++) {
                    expr3 += enrolment[i][h] * X[i][k];
                }
            }
            model.add(expr3 <= capacity[k]);
        }*/

        /*IloExpr s(env);
        model.add(IloMinimize(env, s));*/