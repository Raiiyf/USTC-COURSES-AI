#include "Knapsack.h"

Result Backtrace::solution() {
    // Rearrange item sequence by value rate
    for(int i=0; i < knap.n; i++) {
        float value_rate = knap.value[i] / float(knap.weight[i]);
        auto item = std::make_tuple(i, knap.weight[i], knap.value[i], value_rate);
        A.push_back(item);
    }

    std::sort(A.begin(), A.end(), cmp);

    Solution_value *p = new Solution_value;
    p->solution = "0";
    p->value = 0;
    p->start = std::chrono::high_resolution_clock::now();
    for(int i=0; i < knap.n; i++) 
        p->value_sum.push_back(std::get<2>(A[i]));
    
    for(int i=knap.n - 2; i >= 0 ;i--) {
        p->value_sum[i] += p->value_sum[i + 1];
    }

    aux(0, "", 0, 0, p);

    Result result;
    if(flag && TLE) {
        result.method = "Backtrace TLE";
        result.duration = TIMELIMIT;
    }
    else {
        result.method = "Backtrace";
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - p->start);
        result.duration = duration.count();
    }

    result.value = p->value;
    int temp[knap.n] = {0};
    for(int i=0; i < knap.n; i++) {
        if(p->solution[i] == '1') 
            temp[std::get<0>(A[i])] = 1;
    }
    for(int i=0; i < knap.n; i++) {
        if(temp[i] == 1)
            result.solution += "1";
        else result.solution += "0";
    }

    return result;
}

void Backtrace::aux(int index, std::string cur_solution, int cur_weight, int cur_value, Solution_value *p) {
    // std::cout << cur_solution << " " << cur_value << std::endl;
    if(flag) {
        if(TLE)
            return;

        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - p->start); 
        // If TLE, return directly
        if(duration.count() > TIMELIMIT)
            TLE = true;
    }

    if(index == knap.n - 1) {
        if(cur_weight + std::get<1>(A[knap.n - 1]) <= knap.c) {
            cur_solution += '1';
            cur_value += std::get<2>(A[knap.n - 1]);
            if(cur_value > p->value) {
                p->solution = cur_solution;
                p->value = cur_value;
            }
        }
        else {
            cur_solution += '0';
            if(cur_value > p->value) {
                p->solution = cur_solution;
                p->value = cur_value;
            }
        }
        return;
    }

    if(cur_value + p->value_sum[index] <= p->value)
        return;
    
    if(cur_weight + std::get<1>(A[index]) <= knap.c) 
        aux(index + 1, cur_solution + '1', cur_weight + std::get<1>(A[index]), cur_value + std::get<2>(A[index]), p);
    
    aux(index + 1, cur_solution + '0', cur_weight, cur_value, p);
}
