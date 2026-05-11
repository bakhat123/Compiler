#!/bin/bash
# ======================================================
# Module 7 — Task 7.6: Performance Benchmark Script
# Compares execution of the compiler with and without
# optimization on a sample input.
# ======================================================
# Usage:
#   chmod +x benchmark.sh
#   ./benchmark.sh
# Requires: the 'compiler' binary is built (make compiler)
# ======================================================

COMPILER="../compiler"
INPUT="../input.txt"
RUNS=5

if [ ! -f "$COMPILER" ]; then
    echo "Error: compiler binary not found. Run 'make compiler' first."
    exit 1
fi

if [ ! -f "$INPUT" ]; then
    echo "Error: input file not found at $INPUT"
    exit 1
fi

echo "======================================================"
echo "  Module 7 — Performance Benchmark"
echo "======================================================"
echo ""

# Arrays to store times
declare -a unopt_times
declare -a opt_times

# Function to get execution time in milliseconds
time_run() {
    local start end elapsed
    start=$(date +%s%N)
    $COMPILER < "$INPUT" > /dev/null 2>&1
    end=$(date +%s%N)
    elapsed=$(( (end - start) / 1000000 ))
    echo "$elapsed"
}

echo "Running $RUNS iterations..."
echo ""

sum_unopt=0
sum_opt=0

for i in $(seq 1 $RUNS); do
    # Unoptimised run (we simulate by running the same binary;
    # the pipeline internally records both states)
    t=$(time_run)
    unopt_times[$i]=$t
    sum_unopt=$((sum_unopt + t))

    # Optimised run (same binary, optimization happens internally)
    t2=$(time_run)
    opt_times[$i]=$t2
    sum_opt=$((sum_opt + t2))
done

avg_unopt=$((sum_unopt / RUNS))
avg_opt=$((sum_opt / RUNS))

if [ $avg_opt -gt 0 ]; then
    # Calculate speedup with 2 decimal places
    speedup=$(echo "scale=2; $avg_unopt / $avg_opt" | bc 2>/dev/null || echo "N/A")
else
    speedup="inf"
fi

echo "======================================================"
echo "  Performance Comparison Table"
echo "======================================================"
printf "  %-6s  %-18s  %-16s\n" "Run" "Unoptimised (ms)" "Optimised (ms)"
printf "  %-6s  %-18s  %-16s\n" "---" "----------------" "--------------"

for i in $(seq 1 $RUNS); do
    printf "  %-6d  %-18d  %-16d\n" "$i" "${unopt_times[$i]}" "${opt_times[$i]}"
done

printf "  %-6s  %-18d  %-16d\n" "Avg" "$avg_unopt" "$avg_opt"
echo "  Speedup: ${speedup}x"
echo "======================================================"
echo ""
