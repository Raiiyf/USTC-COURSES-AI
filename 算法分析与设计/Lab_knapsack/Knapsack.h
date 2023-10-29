#ifndef KNAPSACK_H
#define KNAPSACK_H

#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
#include <queue>

#define MAX_WEIGHT 100
#define MAX_VALUE 100
#define TIMELIMIT 1e7

// Problem settings
struct Problem {
    int n;
    int c;
    std::vector<int> weight;
    std::vector<int> value;
};

// Result info
struct Result {
    std::string method;
    std::string solution;
    int value;
    double duration;
};

// Auxilairy struct that Stores global info
struct Solution_value {
    std::string solution;
    int value;
    std::chrono::high_resolution_clock::time_point start;

    // Divide & conquer
    // currently determined index
    int item_index;

    // Backtrace
    // value gain if snatch all the items indexs from i to n - 1
    std::vector<int> value_sum;
};

struct node {
    std::string cur_solution;
    int cur_weight;
    int cur_value;
    int index; // index of item to be discussed 
};

bool cmp(std::tuple<int, int, int, float>, std::tuple<int, int, int, float>);

class DP {
public:
    Problem knap;
    bool flag;

    DP(Problem Knap, bool Flag=true) {
        knap = Knap;
        flag = Flag;
    }
    Result solution();
};

class Greedy {
public:
    Problem knap;

    Greedy(Problem Knap) {
        knap = Knap;
    }
    Result solution();
};

class Divide {
public:
    Problem knap;
    bool flag;

    Divide(Problem Knap, bool Flag=true) {
        knap = Knap;
        flag = Flag;
    }
    Result solution();
    std::tuple<int, std::string> divide_aux(int, int);
    Result solutionwithTLE();
    std::tuple<int, std::string> divide_aux_TLE(int, int, Solution_value*);
};

class Backtrace {
public:
    Problem knap;
    bool flag;
    bool TLE;
    std::vector<std::tuple<int, int, int, float>> A; // index, weight, value, rate

    Backtrace(Problem Knap, bool Flag=true) {
        knap = Knap;
        flag = Flag;
        TLE = false;
    }
    Result solution();
    void aux(int, std::string, int, int, Solution_value*);
};

class Branchbound {
public:
    Problem knap;
    bool flag;
    bool TLE;

    Branchbound(Problem Knap, bool Flag=true) {
        knap = Knap;
        flag = Flag;
        TLE = false;
    }
    Result solution();
};

class Montecarlo {
public:
    Problem knap;
    double loop_time;
    bool flag;
    bool TLE;

    Montecarlo(Problem Knap, double iter, bool Flag=true) {
        knap = Knap;
        loop_time = iter;
        flag = Flag;
        TLE = false;
    }
    Result solution();

};

#endif
