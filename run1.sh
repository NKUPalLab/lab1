#!/bin/bash
g++ -O2 naive.cpp -o naive -std=c++11
g++ -O2 cache_friendly.cpp -o cache_friendly -std=c++11
g++ -O2 unrolling.cpp -o unrolling -std=c++11
rm -f result.txt

EVENTS="L1-dcache-loads,L1-dcache-load-misses,l1d_cache_wr,l2d_cache,l2d_cache_refill,cycles,instructions,branches,branch-misses,cache-references,cache-misses,task-clock"

echo "Naive Dot Product Performance (with extended analysis):" >> result.txt
perf stat -e "$EVENTS" ./naive 2>&1 | tee -a result.txt

echo "" >> result.txt

echo "Cache-Friendly Dot Product Performance (with extended analysis):" >> result.txt
perf stat -e "$EVENTS" ./cache_friendly 2>&1 | tee -a result.txt

echo "" >> result.txt
echo "Performance tests completed. Results saved in result.txt"

echo "Unrolling Dot Product Performance (with extended analysis):" >> result.txt
perf stat -e "$EVENTS" ./unrolling 2>&1 | tee -a result.txt

echo "" >> result.txt
echo "Performance tests completed. Results saved in result.txt"