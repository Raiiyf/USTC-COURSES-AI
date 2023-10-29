#include "Knapsack.h"

bool cmp(std::tuple<int, int, int, float>a, std::tuple<int, int, int, float>b) {
    return std::get<3>(a) > std::get<3>(b);
}

// greed on value rate
Result Greedy::solution() {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::tuple<int, int, int, float>> A; // index weight value rate
    for(int i=0; i < knap.n; i++) {
        float value_rate = knap.value[i] / float(knap.weight[i]);
        auto item = std::make_tuple(i, knap.weight[i], knap.value[i], value_rate);
        A.push_back(item);
    }

    std::sort(A.begin(), A.end(), cmp);

    int cur_weight = 0;
    int cur_value = 0;
    int result_index[knap.n] = {0};
    for(int i=0; i < knap.n; i++) {
        if(cur_weight + std::get<1>(A[i]) <= knap.c) {
            result_index[std::get<0>(A[i])] = 1;
            cur_weight += std::get<1>(A[i]);
            cur_value += std::get<2>(A[i]);
        }
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    Result result;
    result.method = "Greedy";
    result.value = cur_value;
    result.duration = duration.count();
    result.solution = "";
    for(int i=0; i < knap.n; i++) {
        if(result_index[i]) result.solution += '1';
        else result.solution += '0';
    }

    return result;
}
