#include "Knapsack.h"

Result Branchbound::solution () {
    auto start = std::chrono::high_resolution_clock::now();
    // Get the greedy solution
    Greedy greedy(knap);
    Result result = greedy.solution();

    std::vector<int> value_sum;
    for(int i=0; i < knap.n; i++) 
        value_sum.push_back(knap.value[i]);
    for(int i=knap.n - 2; i >= 0 ;i--) {
        value_sum[i] += value_sum[i + 1];
    }

    std::queue<node> Q;
    node root;
    root.cur_solution = "";
    root.cur_value = 0;
    root.cur_weight = 0;
    root.index = 0;
    Q.push(root);

    node temp, child_0, child_1;
    std::chrono::_V2::system_clock::time_point now;
    while(!Q.empty()) {
        if(flag) {
            now = std::chrono::_V2::system_clock::now();
            if(std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() > TIMELIMIT) {
                TLE = true;
                break;
            }
        }

        temp = Q.front();
        Q.pop();
        if(temp.index == knap.n - 1) {
            // Marginal case
            if(temp.cur_weight + knap.weight[temp.index] <= knap.c) {
                if(temp.cur_value + knap.value[temp.index] > result.value) {
                    result.solution = temp.cur_solution + "1";
                    result.value = temp.cur_value + knap.value[temp.index];
                }
            }
            else {
                if(temp.cur_value > result.value) {
                    result.solution = temp.cur_solution + "0";
                    result.value = temp.cur_value;
                }                
            }
            continue;
        }

        if(temp.cur_value + value_sum[temp.index] <= result.value)
            continue;

        if(temp.cur_weight + knap.weight[temp.index] <= knap.c) {
            child_1.cur_solution = temp.cur_solution + "1";
            child_1.cur_weight = temp.cur_weight + knap.weight[temp.index];
            child_1.cur_value = temp.cur_value + knap.value[temp.index];
            child_1.index = temp.index + 1;
            Q.push(child_1);
        }

        child_0.cur_solution = temp.cur_solution + "0";
        child_0.cur_weight = temp.cur_weight;
        child_0.cur_value = temp.cur_value;
        child_0.index = temp.index + 1;
        Q.push(child_0);
    }

    if(TLE) {
        result.method = "Branch&Bound TLE";
        result.duration = TIMELIMIT;
    }
    else {
        result.method = "Branch&Bound";
        now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
        result.duration = duration.count();
    }

    return result;
}
