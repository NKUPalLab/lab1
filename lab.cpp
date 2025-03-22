#include <iostream>
#include <vector>
#include <ctime>
#include <chrono>
#include <random>
#include <iomanip>

// 平凡算法：逐列访问元素计算内积
std::vector<double> naiveDotProduct(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vector, int n) {
    std::vector<double> result(n, 0.0);
    
    // 对每一列进行处理
    for (int j = 0; j < n; j++) {
        double dot = 0.0;
        // 计算第j列与向量的内积
        for (int i = 0; i < n; i++) {
            dot += matrix[i][j] * vector[i];
        }
        result[j] = dot;
    }
    
    return result;
}

// 缓存优化算法：利用矩阵按行存储的特性
std::vector<double> cacheFriendlyDotProduct(const std::vector<std::vector<double>>& matrix, const std::vector<double>& vector, int n) {
    std::vector<double> result(n, 0.0);
    
    // 初始化结果向量
    for (int j = 0; j < n; j++) {
        result[j] = 0.0;
    }
    
    // 按行处理矩阵，依次累加到结果向量的对应位置
    for (int i = 0; i < n; i++) {
        double vectorValue = vector[i]; // 将向量元素缓存到局部变量
        for (int j = 0; j < n; j++) {
            result[j] += matrix[i][j] * vectorValue;
        }
    }
    
    return result;
}

// 生成随机矩阵和向量用于测试
void generateRandomData(std::vector<std::vector<double>>& matrix, std::vector<double>& vector, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(1.0, 100.0);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = dis(gen);
        }
        vector[i] = dis(gen);
    }
}

// 比较两个结果向量是否相等（考虑浮点误差）
bool compareResults(const std::vector<double>& result1, const std::vector<double>& result2, double epsilon = 1e-10) {
    if (result1.size() != result2.size()) return false;
    
    for (size_t i = 0; i < result1.size(); i++) {
        if (std::abs(result1[i] - result2[i]) > epsilon) return false;
    }
    return true;
}

int main() {
    // 设置矩阵大小
    const int sizes[] = {100, 500, 1000, 2000};
    
    std::cout << "矩阵大小\t平凡算法耗时(ms)\t缓存优化算法耗时(ms)\t加速比\n";
    std::cout << "-----------------------------------------------------\n";
    
    for (int n : sizes) {
        // 初始化矩阵和向量
        std::vector<std::vector<double>> matrix(n, std::vector<double>(n));
        std::vector<double> vector(n);
        
        // 生成随机数据
        generateRandomData(matrix, vector, n);
        
        //测试平凡算法
        auto start1 = std::chrono::high_resolution_clock::now();
        std::vector<double> result1 = naiveDotProduct(matrix, vector, n);
        auto end1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration1 = end1 - start1;
        
        // 测试缓存优化算法
        auto start2 = std::chrono::high_resolution_clock::now();
        std::vector<double> result2 = cacheFriendlyDotProduct(matrix, vector, n);
        auto end2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration2 = end2 - start2;
        
        // 验证结果是否相同
        // bool resultsMatch = compareResults(result1, result2);
        
        // // 输出性能比较
        // double speedup = duration1.count() / duration2.count();
        // std::cout << n << "x" << n << "\t" 
        //           << std::fixed << std::setprecision(2) << duration1.count() << "\t\t" 
        //           << duration2.count() << "\t\t\t" 
        //           << speedup << (resultsMatch ? "" : " (结果不匹配!)") << "\n";
    }
    
    return 0;
}