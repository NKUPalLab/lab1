#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

std::vector<double> newOptimizedDotProduct(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vec, int n) {
    std::vector<double> result(n, 0.0);
    for (int i = 0; i < n; i++) {
        double v = vec[i];
        int j = 0;
        // Process two columns per iteration
        for (; j <= n - 2; j += 2) {
            result[j]     += matrix[i][j]   * v;
            result[j + 1] += matrix[i][j+1] * v;
        }
        // Handle any remaining column if n is odd.
        if (j < n) {
            result[j] += matrix[i][j] * v;
        }
    }
    return result;
}

void generateRandomData(std::vector<std::vector<double>>& matrix, std::vector<double>& vec, int n) {
    matrix.resize(n, std::vector<double>(n));
    vec.resize(n);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(1.0, 100.0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dis(gen);
        }
        vec[i] = dis(gen);
    }
}

int main() {
    const int sizes[] = {100, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};

    std::cout << "矩阵大小\tUnrolling算法（循环展开）耗时(ms)" << std::endl;

    for (int n : sizes) {
        std::vector<std::vector<double>> matrix;
        std::vector<double> vec;
        generateRandomData(matrix, vec, n);

        auto start = std::chrono::high_resolution_clock::now();
        std::vector<double> result = newOptimizedDotProduct(matrix, vec, n);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << n << "x" << n << "\t" 
                  << std::fixed << std::setprecision(2) << duration.count() << std::endl;
    }
    return 0;
}
