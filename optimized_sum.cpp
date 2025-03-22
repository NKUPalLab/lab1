#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <arm_neon.h> // ARM NEON 指令集
double recursivePairwiseSum(const std::vector<double>& data, int left, int right) {
    if (left == right)
        return data[left];
    if (right - left == 1)
        return data[left] + data[right];
    int mid = left + (right - left) / 2;
    double leftSum, rightSum;
    if (right - left > 10000) {
        std::thread leftThread([&]() { 
            leftSum = recursivePairwiseSum(data, left, mid); 
        });
        rightSum = recursivePairwiseSum(data, mid + 1, right);
        leftThread.join();
    } else {
        leftSum = recursivePairwiseSum(data, left, mid);
        rightSum = recursivePairwiseSum(data, mid + 1, right);
    }
    return leftSum + rightSum;
}
double twoWayChainedSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    double sum1 = 0.0;
    double sum2 = 0.0; 
    size_t size = data.size();
    for (size_t i = 0; i < size - (size % 2); i += 2) {
        sum1 += data[i];      
        sum2 += data[i + 1];  
    }
    if (size % 2 == 1) {
        sum1 += data[size - 1];
    }
    return sum1 + sum2;
}

double fourWayChainedSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    
    double sum1 = 0.0; 
    double sum2 = 0.0; 
    double sum3 = 0.0; 
    double sum4 = 0.0; 
    size_t size = data.size();
    for (size_t i = 0; i < size - (size % 4); i += 4) {
        sum1 += data[i];
        sum2 += data[i + 1];
        sum3 += data[i + 2];
        sum4 += data[i + 3];
    }
    size_t remainder = size % 4;
    size_t startIdx = size - remainder;
    for (size_t i = 0; i < remainder; ++i) {
        sum1 += data[startIdx + i];
    }
    return (sum1 + sum2) + (sum3 + sum4);
}
double neonPairwiseSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    const size_t size = data.size();
    size_t i = 0;
    double sum = 0.0;
    float64x2_t sumVector = vdupq_n_f64(0.0);
    for (; i + 1 < size; i += 2) {
        float64x2_t dataVector = vld1q_f64(&data[i]);
        sumVector = vaddq_f64(sumVector, dataVector);  
    }
    double results[2];
    vst1q_f64(results, sumVector);
    sum = results[0] + results[1];
    if (i < size) {
        sum += data[i];
    }
    return sum;
}
double optimizedSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    return recursivePairwiseSum(data, 0, data.size() - 1);
}
void generateRandomData(std::vector<double>& data, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 100.0);
    
    data.resize(n);
    for (int i = 0; i < n; i++) {
        data[i] = dis(gen);
    }
}
int main() {
    const int sizes[] = {10000, 100000, 1000000, 10000000};
    int trials = 3; 
    std::cout << "数组大小\t递归优化\t\t两路链式\t四路链式\t\tNEON优化" << std::endl;
    std::cout << "\t\t时间(ms)\t\t时间(ms)\t时间(ms)\t\t时间(ms)" << std::endl;
    for (int n : sizes) {
        double totalTimeRecursive = 0.0;
        double totalTimeChained = 0.0;
        double totalTimeFourWay = 0.0;
        double totalTimeNEON = 0.0;
        double result1 = 0.0, result2 = 0.0, result3 = 0.0, result4 = 0.0;
        for (int t = 0; t < trials; t++) {
            std::vector<double> data;
            generateRandomData(data, n);
            auto start1 = std::chrono::high_resolution_clock::now();
            result1 = optimizedSum(data);
            auto end1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration1 = end1 - start1;
            totalTimeRecursive += duration1.count();

            auto start2 = std::chrono::high_resolution_clock::now();
            result2 = twoWayChainedSum(data);
            auto end2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration2 = end2 - start2;
            totalTimeChained += duration2.count();

            auto start3 = std::chrono::high_resolution_clock::now();
            result3 = fourWayChainedSum(data);
            auto end3 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration3 = end3 - start3;
            totalTimeFourWay += duration3.count();
            
            auto start4 = std::chrono::high_resolution_clock::now();
            result4 = neonPairwiseSum(data);
            auto end4 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration4 = end4 - start4;
            totalTimeNEON += duration4.count();
        }
        
        double avgTimeRecursive = totalTimeRecursive / trials;
        double avgTimeChained = totalTimeChained / trials;
        double avgTimeFourWay = totalTimeFourWay / trials;
        double avgTimeNEON = totalTimeNEON / trials;
        
        std::cout << n << "\t\t" << avgTimeRecursive << "\t\t\t" 
                 << avgTimeChained << "\t\t" << avgTimeFourWay << "\t\t\t"
                 << avgTimeNEON << std::endl;
    }
    
    return 0;
}