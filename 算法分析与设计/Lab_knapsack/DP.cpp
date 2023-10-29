#include "Knapsack.h"

Result DP::solution() {
    auto start = std::chrono::high_resolution_clock::now();
    int n = knap.n;
    int c = knap.c;
    int m[n][c + 1] = {0}; // m[i][j]: choosing item from i to n - 1, with sack capacity j
    int i,j;

    std::chrono::high_resolution_clock::time_point now;
    std::chrono::microseconds duration;
    // Only the last item can be chosen
    for(j=knap.weight[n - 1]; j <= c; j++) 
        m[n - 1][j] = knap.value[n - 1];

    for(i=n - 2; i > 0; i--) {
        for(j=0; j <= std::min(knap.weight[i] - 1, c); j++)
            m[i][j] = m[i + 1][j];
        for(j=knap.weight[i]; j <= c; j++) {
            m[i][j] = std::max(m[i + 1][j], m[i + 1][j - knap.weight[i]] + knap.value[i]);
        }

        if(flag) {
            now = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
            if(duration.count() > TIMELIMIT && i > 1) {
                // Interrupt handler
                int min_c = c;
                for(j=c; j >= 0; j--) {
                    // Find the max value with minimun contains of subquestion (i, c)
                    if(m[i][j] >= m[i][min_c]) 
                        min_c = j;
                }

                Result mid_result;
                mid_result.method = "DP TLE";
                mid_result.value = m[i][min_c];
                mid_result.duration = TIMELIMIT;
                int cur_contain = min_c;
                for(int k = i; k < n - 1; k++) {
                    if(m[k][cur_contain] == m[k + 1][cur_contain]) 
                        mid_result.solution += '0';
                    else {
                        mid_result.solution += '1';
                        cur_contain -= knap.weight[k];
                    }
                }
                if(m[n - 1][cur_contain]) mid_result.solution += '1';
                else mid_result.solution += '0';

                int rest_contain = c - min_c;
                for(int k=i - 1; k >= 0; k--) {
                    if(rest_contain - knap.weight[k] > 0) {
                        mid_result.value += knap.value[k];
                        mid_result.solution = '1' + mid_result.solution;
                        rest_contain -= knap.weight[k];
                    }
                    else mid_result.solution = '0' + mid_result.solution;
                }
                return mid_result;
            }
        }
    }

    m[0][c] = std::max(m[1][c], m[1][c - knap.weight[0]] + knap.value[0]);

    Result result;
    result.method = "DP";
    result.value = m[0][c];

    int cur_contain = c;
    result.solution = "";
    for(i = 0; i < n - 1; i++) {
        if(m[i][cur_contain] == m[i + 1][cur_contain]) 
            result.solution += '0';
        else {
            result.solution += '1';
            cur_contain -= knap.weight[i];
        }
    }

    if(m[n - 1][cur_contain]) result.solution += '1';
    else result.solution += '0';

    auto stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    result.duration = duration.count();
    return result;
}


