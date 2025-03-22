#include <iostream>
#include <vector>
#include <chrono>
#include <random>
double naiveSum(const std::vector<double>& data) {
    double sum = 0.0;
    for (double value : data) {
        sum += value;
    }
    return sum;
}
void generateRandomData(std::vector<double>& data, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 100.0);
    data.resize(n);
    for (int i = 0; i < n; i++){
        data[i] = dis(gen);
    }
}
int main() {
    const int sizes[] = {10000, 100000, 1000000, 10000000};
    int trials = 3;
    std::cout << "Array Size\tNaive Average Time (ms)" << std::endl;
    for (int n : sizes) {
        double totalTime = 0.0;
        for (int t = 0; t < trials; t++) {
            std::vector<double> data;
            generateRandomData(data, n);
            auto start = std::chrono::high_resolution_clock::now();
            double result = naiveSum(data);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;
            totalTime += duration.count();
        }
        double avgTime = totalTime / trials;
        std::cout << n << "\t\t" << avgTime << std::endl;
    }
    return 0;
}