#include "Knapsack.h"

Result Divide::solution() {
    if(flag) 
        return solutionwithTLE();

    auto start = std::chrono::high_resolution_clock::now();

    Result result;
    auto tuple = divide_aux(0, knap.c);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    result.method = "Divide";
    result.value = std::get<0>(tuple);
    result.solution = std::get<1>(tuple);
    result.duration = duration.count();

    return result;
}

// Choosing items from i ... n - 1 with sack capacity c
std::tuple<int, std::string> Divide::divide_aux(int i, int capacity) {
    // Marginal case
    if(i == knap.n - 1 && capacity < knap.weight[i]) 
        return std::make_tuple(0, "0");
    
    if(i == knap.n - 1 && capacity >= knap.weight[i])
        return std::make_tuple(knap.value[i], "1");
    
    auto result_0 = divide_aux(i + 1, capacity);
    // Can not choose item i
    if(capacity < knap.weight[i])
        return std::make_tuple(std::get<0>(result_0), "0" + std::get<1>(result_0));

    auto result_1 = divide_aux(i + 1, capacity - knap.weight[i]);

    // Compare between two subquestions
    if(std::get<0>(result_0) > std::get<0>(result_1) + knap.value[i]) 
        return std::make_tuple(std::get<0>(result_0), "0" + std::get<1>(result_0));
    else
        return std::make_tuple(std::get<0>(result_1) + knap.value[i], "1" + std::get<1>(result_1));
}

Result Divide::solutionwithTLE() {
    Solution_value *p = new Solution_value;
    p->solution = "";
    p->value = 0;
    p->item_index = knap.n;
    p->start = std::chrono::high_resolution_clock::now();

    Result result;
    auto tuple = divide_aux_TLE(0, knap.c, p);
    if(std::get<1>(tuple) != "TLE") {
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - p->start);
        result.method = "Divide";
        result.value = std::get<0>(tuple);
        result.solution = std::get<1>(tuple);
        result.duration = duration.count();

        return result;
    }

    int cur_index = p->item_index;
    int j = 0;
    int cur_capacity = 0;

    for(int i = cur_index; i < knap.n; i++) {
        if(p->solution[j] == '1') 
            cur_capacity += knap.weight[i];
        j++;
    }

    for(int i = cur_index - 1; i >= 0; i--) {
        if(cur_capacity + knap.weight[i] < knap.c) {
            p->solution = '1' + p->solution;
            p->value += knap.value[i];
            cur_capacity += knap.weight[i];
        }
        else p->solution = '0' + p->solution;
    }

    result.method = "Divide TLE";
    result.duration = TIMELIMIT;
    result.solution = p->solution;
    result.value = p->value;
    return result;
}

std::tuple<int, std::string> Divide::divide_aux_TLE(int i, int capacity, Solution_value *p) {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - p->start); 
    // If TLE, return directly
    if(duration.count() > TIMELIMIT)
        return std::make_tuple(-1, "TLE");
    
    if(i == knap.n - 1 && capacity < knap.weight[i]) 
        return std::make_tuple(0, "0");
    
    if(i == knap.n - 1 && capacity >= knap.weight[i])
        return std::make_tuple(knap.value[i], "1");

    auto result_0 = divide_aux_TLE(i + 1, capacity, p);
    if(std::get<1>(result_0) == "TLE")
        return result_0;
    // Can not choose item i
    if(capacity < knap.weight[i])
        return std::make_tuple(std::get<0>(result_0), "0" + std::get<1>(result_0));
    
    auto result_1 = divide_aux_TLE(i + 1, capacity - knap.weight[i], p);
    if(std::get<1>(result_1) == "TLE")
        return result_1;

    // Compare between two subquestions
    if(std::get<0>(result_0) > std::get<0>(result_1) + knap.value[i]) {
        int cur_value = std::get<0>(result_0);
        std::string cur_solution = "0" + std::get<1>(result_0);
        if(cur_value > p->value) {
            p->value = cur_value;
            p->solution = cur_solution;
            p->item_index = i;
        }
        return std::make_tuple(cur_value, cur_solution);
    }
    else {
        int cur_value = std::get<0>(result_1) + knap.value[i];
        std::string cur_solution = "1" + std::get<1>(result_1);
        if(cur_value > p->value) {
            p->value = cur_value;
            p->solution = cur_solution;
            p->item_index = i;
        }
        return std::make_tuple(cur_value, cur_solution);
    }
}
