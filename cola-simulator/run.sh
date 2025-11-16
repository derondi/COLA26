#!/bin/bash

time=$(date "+%Y%m%d_%H:%M:%S")
dir="log/$time"

mkdir -p log/$time

cpu=$(cat /proc/cpuinfo | grep processor | wc -l)
cpu=$(($cpu-1))

# parallel processing
for i in $(seq 0 $cpu) 
do
sleep 0.2
taskset -c $i ./build/LDPCTest > "$dir/cpu_$i.log" & 
done

echo "parallel processing simulation on cpu_0 ~ cpu_$cpu, waiting..."

wait

# summary log
summary_log="$dir/summary.log"

iteration_sum=0
cost_sum=0
frame_error_sum=0
uncorrectable_bit_error_sum=0
decode_iteration_sum=0
decode_page_sum=0
fail_page_sum=0
zero_fail_page_sum=0
one_fail_page_sum=0
two_fail_page_sum=0
three_fail_page_sum=0
four_fail_page_sum=0

for i in $(seq 0 $cpu) 
do
    iteration_sum=$(($iteration_sum + $(grep "iteration:" "$dir/cpu_$i.log" | awk '{print $2}')))
    cost_sum=$(($cost_sum + $(cat "$dir/cpu_$i.log" | grep "finish .* times ldpc simulations" | awk '{print $7}'))) 
    frame_error_sum=$(($frame_error_sum + $(tail -n 11 "$dir/cpu_$i.log" | head -n 1 | awk '{print $5}'))) 
    uncorrectable_bit_error_sum=$(($uncorrectable_bit_error_sum + $(tail -n 9 "$dir/cpu_$i.log" | head -n 1 | awk '{print $6}'))) 
    decode_iteration_sum=$(($decode_iteration_sum + $(tail -n 13 "$dir/cpu_$i.log" | head -n 1 | awk '{print $5}'))) 
    decode_page_sum=$(($decode_page_sum + $(tail -n 7 "$dir/cpu_$i.log" | head -n 1 | awk '{print $5}')))
    fail_page_sum=$(($fail_page_sum + $(tail -n 6 "$dir/cpu_$i.log" | head -n 1 | awk '{print $5}')))
    zero_fail_page_sum=$(($zero_fail_page_sum + $(tail -n 5 "$dir/cpu_$i.log" | head -n 1 | awk '{print $6}')))
    one_fail_page_sum=$(($one_fail_page_sum + $(tail -n 4 "$dir/cpu_$i.log" | head -n 1 | awk '{print $6}')))
    two_fail_page_sum=$(($two_fail_page_sum + $(tail -n 3 "$dir/cpu_$i.log" | head -n 1 | awk '{print $6}')))
    three_fail_page_sum=$(($three_fail_page_sum + $(tail -n 2 "$dir/cpu_$i.log" | head -n 1 | awk '{print $6}')))
    four_fail_page_sum=$(($four_fail_page_sum + $(tail -n 1 "$dir/cpu_$i.log" | awk '{print $6}')))
done

cost=$(echo "$cost_sum $cpu" | awk '{printf "%.2f\n", $1 / ($2 + 1)}')
code_size=$(($(cat "$dir/cpu_0.log" | grep "information_bits:" | awk '{print $2}') + $(cat "$dir/cpu_0.log" | grep "parity_bits:" | awk '{print $2}')))
frame_error_rate=$(echo "$frame_error_sum $iteration_sum" | awk '{printf "%.2e\n", $1 / $2}')
uncorrectable_bit_error_rate=$(echo "$uncorrectable_bit_error_sum $iteration_sum $code_size" | awk '{printf "%.5e\n", $1 / $2 / $3}')
decode_avg_iteration=$(echo "$decode_iteration_sum $iteration_sum" | awk '{if ($2 > 0) printf "%.2f\n", $1 / $2; else print 0}')

head -n 14 "$dir/cpu_0.log" > $summary_log
echo -e "iteration = $iteration_sum\n" >> $summary_log
echo -e "finish $iteration_sum times ldpc simulations, cost $cost s\n" >> $summary_log
echo -e "decode iteration sum= $decode_iteration_sum" >> $summary_log
echo -e "decode avg iteration= $decode_avg_iteration" >> $summary_log
echo -e "frame error number = $frame_error_sum" >> $summary_log
echo -e "frame error rate (FER) = $frame_error_rate" >> $summary_log
echo -e "uncorrectable bit error number = $uncorrectable_bit_error_sum" >> $summary_log
echo -e "uncorrectable bit error rate (UBER) = $uncorrectable_bit_error_rate" >> $summary_log
echo -e "finish page number = $decode_page_sum" >> $summary_log
echo -e "fail page number = $fail_page_sum" >> $summary_log
echo -e "zero fail page number = $zero_fail_page_sum" >> $summary_log
echo -e "one fail page number = $one_fail_page_sum" >> $summary_log
echo -e "two fail page number = $two_fail_page_sum" >> $summary_log
echo -e "three fail page number = $three_fail_page_sum" >> $summary_log
echo -e "four fail page number = $four_fail_page_sum" >> $summary_log
echo "finish simulation, all cpu logs and summary are saved in log/$time"