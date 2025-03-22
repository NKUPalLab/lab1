#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <arm_neon.h> // ARM NEON 指令集

// 递归两两相加算法
double recursivePairwiseSum(const std::vector<double>& data, int left, int right) {
    // 基本情况：单个元素
    if (left == right)
        return data[left];
    
    // 基本情况：两个元素
    if (right - left == 1)
        return data[left] + data[right];
    
    // 分治：计算中点
    int mid = left + (right - left) / 2;
    
    // 对大规模问题使用并行计算
    double leftSum, rightSum;
    if (right - left > 10000) {
        // 使用线程并行计算左右子树
        std::thread leftThread([&]() { 
            leftSum = recursivePairwiseSum(data, left, mid); 
        });
        
        // 当前线程计算右子树
        rightSum = recursivePairwiseSum(data, mid + 1, right);
        
        // 等待左子树计算完成
        leftThread.join();
    } else {
        // 对小规模问题直接顺序计算
        leftSum = recursivePairwiseSum(data, left, mid);
        rightSum = recursivePairwiseSum(data, mid + 1, right);
    }
    
    // 合并结果
    return leftSum + rightSum;
}

// 两路链式累加
double twoWayChainedSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    
    // 使用两个独立累加器减少依赖
    double sum1 = 0.0; // 第一个累加器
    double sum2 = 0.0; // 第二个累加器
    size_t size = data.size();
    
    // 双路并行累加
    for (size_t i = 0; i < size - (size % 2); i += 2) {
        sum1 += data[i];       // 处理偶数索引元素
        sum2 += data[i + 1];   // 处理奇数索引元素
    }
    
    // 处理可能的剩余元素
    if (size % 2 == 1) {
        sum1 += data[size - 1];
    }
    
    // 合并两个累加器的结果
    return sum1 + sum2;
}

// 四路链式累加
double fourWayChainedSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    
    // 使用四个独立累加器进一步减少依赖
    double sum1 = 0.0; // 第一个累加器
    double sum2 = 0.0; // 第二个累加器
    double sum3 = 0.0; // 第三个累加器
    double sum4 = 0.0; // 第四个累加器
    size_t size = data.size();
    
    // 四路并行累加
    for (size_t i = 0; i < size - (size % 4); i += 4) {
        sum1 += data[i];
        sum2 += data[i + 1];
        sum3 += data[i + 2];
        sum4 += data[i + 3];
    }
    
    // 处理剩余元素
    size_t remainder = size % 4;
    size_t startIdx = size - remainder;
    
    for (size_t i = 0; i < remainder; ++i) {
        sum1 += data[startIdx + i];
    }
    
    // 两两合并结果
    return (sum1 + sum2) + (sum3 + sum4);
}

// ARM64 NEON向量化累加
double neonPairwiseSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    
    const size_t size = data.size();
    size_t i = 0;
    double sum = 0.0;
    
    // 使用NEON寄存器处理双精度浮点数
    float64x2_t sumVector = vdupq_n_f64(0.0); // 初始化向量寄存器为0
    
    // 每次处理2个双精度浮点数
    for (; i + 1 < size; i += 2) {
        float64x2_t dataVector = vld1q_f64(&data[i]); // 从内存加载数据
        sumVector = vaddq_f64(sumVector, dataVector);  // 向量加法
    }
    
    // 提取NEON寄存器中的结果
    double results[2];
    vst1q_f64(results, sumVector);
    sum = results[0] + results[1];
    
    // 处理剩余元素
    if (i < size) {
        sum += data[i];
    }
    
    return sum;
}

// 封装递归算法调用
double optimizedSum(const std::vector<double>& data) {
    if (data.empty())
        return 0.0;
    return recursivePairwiseSum(data, 0, data.size() - 1);
}

// 生成随机测试数据
void generateRandomData(std::vector<double>& data, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 100.0);
    
    data.resize(n);
    for (int i = 0; i < n; i++) {
        data[i] = dis(gen);
    }
}

// 测试各种算法并比较性能
int main() {
    // 定义测试数据大小
    const int sizes[] = {10000, 100000, 1000000, 10000000};
    int trials = 3; // 每个大小重复测试的次数
    
    // 打印表头
    std::cout << "测试在ARM64架构上的性能" << std::endl;
    std::cout << "数组大小\t递归优化\t\t两路链式\t四路链式\t\tNEON优化" << std::endl;
    std::cout << "\t\t时间(ms)\t\t时间(ms)\t时间(ms)\t\t时间(ms)" << std::endl;
    
    // 对每个数据大小进行测试
    for (int n : sizes) {
        // 计时变量
        double totalTimeRecursive = 0.0;
        double totalTimeChained = 0.0;
        double totalTimeFourWay = 0.0;
        double totalTimeNEON = 0.0;
        
        // 结果变量
        double result1 = 0.0, result2 = 0.0, result3 = 0.0, result4 = 0.0;
        
        // 重复测试以获得平均时间
        for (int t = 0; t < trials; t++) {
            std::vector<double> data;
            generateRandomData(data, n);
            
            // 测试递归两两相加
            auto start1 = std::chrono::high_resolution_clock::now();
            result1 = optimizedSum(data);
            auto end1 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration1 = end1 - start1;
            totalTimeRecursive += duration1.count();
            
            // 测试两路链式累加
            auto start2 = std::chrono::high_resolution_clock::now();
            result2 = twoWayChainedSum(data);
            auto end2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration2 = end2 - start2;
            totalTimeChained += duration2.count();
            
            // 测试四路链式累加
            auto start3 = std::chrono::high_resolution_clock::now();
            result3 = fourWayChainedSum(data);
            auto end3 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration3 = end3 - start3;
            totalTimeFourWay += duration3.count();
            
            // 测试NEON优化累加
            auto start4 = std::chrono::high_resolution_clock::now();
            result4 = neonPairwiseSum(data);
            auto end4 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration4 = end4 - start4;
            totalTimeNEON += duration4.count();
            
            // 验证结果一致性
            double epsilon = 0.0001 * n; // 允许小的误差
            if (std::abs(result1 - result2) > epsilon || 
                std::abs(result1 - result3) > epsilon ||
                std::abs(result1 - result4) > epsilon) {
                std::cout << "递归: " << result1 << std::endl;
                std::cout << "两路链式: " << result2 << std::endl;
                std::cout << "四路链式: " << result3 << std::endl;
                std::cout << "NEON: " << result4 << std::endl;
            }
        }
        
        // 计算平均时间
        double avgTimeRecursive = totalTimeRecursive / trials;
        double avgTimeChained = totalTimeChained / trials;
        double avgTimeFourWay = totalTimeFourWay / trials;
        double avgTimeNEON = totalTimeNEON / trials;
        
        // 输出结果
        std::cout << n << "\t\t" << avgTimeRecursive << "\t\t\t" 
                 << avgTimeChained << "\t\t" << avgTimeFourWay << "\t\t\t"
                 << avgTimeNEON << std::endl;
    }
    
    return 0;
}