#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>
std::vector<double> naiveDotProduct(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vec, int n) {
    std::vector<double> result(n, 0.0);
    for (int j = 0; j < n; j++) {
        double dot = 0.0;
        for (int i = 0; i < n; i++) {
            dot += matrix[i][j] * vec[i];
        }
        result[j] = dot;
    }
    return result;
}
void generateRandomData(std::vector<std::vector<double>>& matrix, std::vector<double>& vec, int n) {
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
    const int sizes[] = {100, 500, 1000, 2000,3000,4000,5000,6000,7000,8000,9000,10000};
    std::cout << "矩阵大小\t平凡算法耗时(ms)" << std::endl;
    for (int n : sizes) {
        std::vector<std::vector<double>> matrix(n, std::vector<double>(n));
        std::vector<double> vec(n);
        generateRandomData(matrix, vec, n);
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<double> result = naiveDotProduct(matrix, vec, n);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << n << "x" << n << "\t" << std::fixed << std::setprecision(2) << duration.count() << std::endl;
    }
    return 0;
}