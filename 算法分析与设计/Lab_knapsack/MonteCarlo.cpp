#include "Knapsack.h"

Result Montecarlo::solution() {
    auto start = std::chrono::high_resolution_clock::now();
    std::default_random_engine e(time(0));
    std::uniform_real_distribution<double> u(0, 1);
    Result result;
    result.value = 0;
    int result_weight = 0;
    node temp;
    std::chrono::_V2::system_clock::time_point now;

    // Rearrange item sequence by value rate
    std::vector<std::tuple<int, int, int, float>> A; // index, weight, value, rate
    for(int i=0; i < knap.n; i++) {
        float value_rate = knap.value[i] / float(knap.weight[i]);
        auto item = std::make_tuple(i, knap.weight[i], knap.value[i], value_rate);
        A.push_back(item);
    }
    std::sort(A.begin(), A.end(), cmp);

    while(loop_time) {
        if(flag) {
            now = std::chrono::_V2::system_clock::now();
            if(std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() > TIMELIMIT) {
                TLE = true;
                break;
            }
        }
        
        temp.cur_solution = "";
        temp.cur_value = 0;
        temp.cur_weight = 0;
        for(int i=0; i < knap.n; i++) {
            if(temp.cur_weight + std::get<1>(A[i]) <= knap.c && u(e) > 0.5) {
                temp.cur_solution += "1";
                temp.cur_value += std::get<2>(A[i]);
                temp.cur_weight += std::get<1>(A[i]);
            }
            else {
                temp.cur_solution += "0";
            }
        }

        if(temp.cur_value >= result.value) {
            result.value = temp.cur_value;
            result.solution = temp.cur_solution;
            result_weight = temp.cur_weight;
        }

        loop_time--;
    }

    // Utilizing surplus sack room 
    for(int i=0; i < knap.n; i++) {
        if(result.solution[i] != '1' && result_weight + std::get<1>(A[i]) <= knap.c) {
            result.solution[i] = '1';
            result.value += std::get<2>(A[i]);
            result_weight += std::get<1>(A[i]);
        }
    }

    int solution_temp[knap.n] = {0};
    for(int i=0; i < knap.n; i++) {
        if(result.solution[i] == '1') 
            solution_temp[std::get<0>(A[i])] = 1;
    }
    result.solution = "";
    for(int i=0; i < knap.n; i++) {
        if(solution_temp[i] == 1)
            result.solution += "1";
        else result.solution += "0";
    }

    if(TLE) {
        result.method = "MonteCarlo TLE";
        result.duration = TIMELIMIT;
    }
    else {
        result.method = "MonteCarlo";
        now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
        result.duration = duration.count();
    }
    return result;
}
