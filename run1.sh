#!/bin/bash
g++ -O2 naive.cpp -o naive -std=c++11
g++ -O2 cache_friendly.cpp -o cache_friendly -std=c++11
g++ -O2 unrolling.cpp -o unrolling -std=c++11
# 清空旧的结果文件
rm -f result.txt

# 定义要监测的 perf 事件
EVENTS="L1-dcache-loads,L1-dcache-load-misses,l1d_cache_wr,l2d_cache,l2d_cache_refill,cycles,instructions,branches,branch-misses,cache-references,cache-misses,task-clock"

# 运行 naive 算法性能测试
echo "Naive Dot Product Performance (with extended analysis):" >> result.txt
perf stat -e "$EVENTS" ./naive 2>&1 | tee -a result.txt

echo "" >> result.txt

# 运行缓存优化算法性能测试
echo "Cache-Friendly Dot Product Performance (with extended analysis):" >> result.txt
perf stat -e "$EVENTS" ./cache_friendly 2>&1 | tee -a result.txt

echo "" >> result.txt
echo "Performance tests completed. Results saved in result.txt"

echo "Unrolling Dot Product Performance (with extended analysis):" >> result.txt
perf stat -e "$EVENTS" ./unrolling 2>&1 | tee -a result.txt

echo "" >> result.txt
echo "Performance tests completed. Results saved in result.txt"