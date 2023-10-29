#include "Knapsack.h"

#define coutw(n) (std::cout << std::setw(n))

std::uniform_int_distribution<int> v(1, MAX_VALUE); 
std::uniform_int_distribution<int> w(1, MAX_WEIGHT); 
std::default_random_engine e(0);

Problem GenerateSetting(int N, int C) {
    Problem knap;
    knap.n = N;
    knap.c = C;
    for(int i=0; i < N; i++) {
        knap.weight.push_back(w(e));
        knap.value.push_back(v(e));
    }
    return knap;
}

void print_problem(Problem knap) {
    std::cout << std::left;
    std::cout << "Problem Settings:" << std::endl;
    std::cout << "N: " << knap.n << std::endl;

    std::cout << "Weight:\t";
    for(int i = 0; i < knap.n; i ++) {
        coutw(3) << knap.weight[i] << ' ';
    }
    std::cout << std::endl;

    std::cout << "Value:\t";
    for(int i = 0; i < knap.n; i ++) {
        coutw(3) << knap.value[i] << ' ';
    }
    std::cout << std::endl;

    std::cout << "Container: " << knap.c << std::endl;
}

void print_result(Result result) {
    coutw(20) << result.method;
    std::cout << "Value: " << result.value << '\t';
    std::cout << "Time: " << std::setw(15) << result.duration << '\t';
    std::cout << "Solution: " << result.solution << std::endl;
}

int main(){
    Problem knap = GenerateSetting(20, 100); // N C
    print_problem(knap);
    Result result;
    std::cout << "==========SOLUTION==========" <<std::endl;

    Divide divide(knap);
    result = divide.solution();
    print_result(result);

    DP dp(knap);
    result = dp.solution();
    print_result(result);
    
    Greedy greedy(knap);
    result = greedy.solution();
    print_result(result);

    Backtrace backtrace(knap);
    result = backtrace.solution();
    print_result(result);

    Branchbound branchbound(knap);
    result = branchbound.solution();
    print_result(result);

    Montecarlo montecarlo(knap, 1e6);
    result = montecarlo.solution();
    print_result(result);
}
