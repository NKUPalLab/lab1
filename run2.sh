#!/bin/bash
g++ -O0 naive_sum.cpp -o naive_sum -std=c++11
g++ -O0 optimized_sum.cpp -o optimized_sum -std=c++11
rm -f results2.txt

EVENTS="L1-dcache-loads,L1-dcache-load-misses,l1d_cache_wr,l2d_cache,l2d_cache_refill,cycles,instructions,branches,branch-misses,cache-references,cache-misses,task-clock"

echo "Naive Sum Performance:" >> results2.txt
perf stat -e $EVENTS ./naive_sum 2>&1 | tee -a results2.txt

echo "" >> results2.txt

echo "Optimized Recursive Sum Performance:" >> results2.txt
perf stat -e $EVENTS ./optimized_sum 2>&1 | tee -a results2.txt

echo "" >> results2.txt
echo "Performance testing completed. Results are saved in results2.txt"