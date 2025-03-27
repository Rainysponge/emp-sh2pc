#!/bin/bash
cd build

# 初始化变量
total_send_rounds_1=0
total_recv_rounds_1=0
total_send_bytes_1=0
total_recv_bytes_1=0
total_send_rounds_2=0
total_recv_rounds_2=0
total_send_bytes_2=0
total_recv_bytes_2=0
total_computation_time_1=0
total_reading_time_1=0
total_computation_time_2=0
total_reading_time_2=0


# 运行 100 次循环
total=20 
PROGRESS_WIDTH=20
echo "==================================AES BEGIN========================================="
for ((i=1; i<=total; i++)); do
    # 计算进度条
    percent=$((i * 100 / total))
    progress=$(( (i * PROGRESS_WIDTH) / total ))
    remaining=$((PROGRESS_WIDTH - progress))
    bar=$(printf "%-${PROGRESS_WIDTH}s" "$(printf '█%.0s' $(seq 1 $progress))")
    echo -ne "\r[ ${bar// / } ] $percent% ($i/$total) Running iteration $i...          "

    # 启动进程
    port=$((1234 + i))
    # echo -ne "\r[$bar] $percent% ($i/$total) Running iteration $i..."
    ./bin/test_circuit_file_aes 1 1234 > output1.log&
    ./bin/test_circuit_file_aes 2 1234 > output2.log 2>&1
    wait

    # 解析 party 1 数据（清理分号）
    computation_time_1=$(grep "Time for Computation:" output1.log | awk '{print $4}' | tr -d ';')
    reading_time_1=$(grep "Time for Reading File and Creating Circuits:" output1.log | awk '{print $8}' | tr -d ';')
    send_rounds_1=$(grep "party 1: send rounds:" output1.log | awk '{print $5}' | tr -d ';')
    recv_rounds_1=$(grep "recv rounds:" output1.log | awk '{print $8}' | tr -d ';')
    send_bytes_1=$(grep "party 1: send bytes:" output1.log | awk '{print $5}' | tr -d ';')
    recv_bytes_1=$(grep "recv bytes:" output1.log | awk '{print $5}' | tr -d ';')
    
    # 解析 party 2 数据（清理分号）
    send_rounds_2=$(grep "party 2: send rounds:" output2.log | awk '{print $5}' | tr -d ';')
    recv_rounds_2=$(grep "recv rounds:" output2.log | awk '{print $8}' | tr -d ';')
    send_bytes_2=$(grep "party 2: send bytes:" output2.log | awk '{print $5}' | tr -d ';')
    recv_bytes_2=$(grep "recv bytes:" output2.log | awk '{print $5}' | tr -d ';')
    computation_time_2=$(grep "Time for Computation:" output2.log | awk '{print $4}' | tr -d ';')
    reading_time_2=$(grep "Time for Reading File and Creating Circuits:" output2.log | awk '{print $8}' | tr -d ';')

    # 累加数据
    total_send_rounds_1=$((total_send_rounds_1 + send_rounds_1))
    total_recv_rounds_1=$((total_recv_rounds_1 + recv_rounds_1))
    total_send_bytes_1=$((total_send_bytes_1 + send_bytes_1))
    total_recv_bytes_1=$((total_recv_bytes_1 + recv_bytes_1))
    total_computation_time_1=$((total_computation_time_1 + computation_time_1))
    total_reading_time_1=$((total_reading_time_1 + reading_time_1))

    total_send_rounds_2=$((total_send_rounds_2 + send_rounds_2))
    total_recv_rounds_2=$((total_recv_rounds_2 + recv_rounds_2))
    total_send_bytes_2=$((total_send_bytes_2 + send_bytes_2))
    total_recv_bytes_2=$((total_recv_bytes_2 + recv_bytes_2))
    total_computation_time_2=$((total_computation_time_2 + computation_time_2))
    total_reading_time_2=$((total_reading_time_2 + reading_time_2))
done

convert_to_kb() {
    local bytes=$1
    if [ "$bytes" -gt 1024 ]; then
        echo "$((bytes / 1024)) KB"
    else
        echo "$bytes B"
    fi
}

convert_to_ms() {
    local times=$1
    if [ "$times" -gt 13000 ]; then
        echo "$((times / 1000)) ms"
    else
        echo "$times ns"
    fi
}


echo "Emp-tool AES Total Results after 100 Iterations:"
echo "Party 1:"
echo "  Send Rounds: $total_send_rounds_1"
echo "  Recv Rounds: $total_recv_rounds_1"
echo "  Send Bytes: $(convert_to_kb $total_send_bytes_1)"
echo "  Recv Bytes: $(convert_to_kb $total_recv_bytes_1)"
echo "  Time for Reading File and Creating Circuits: $(convert_to_ms $total_reading_time_1)"
echo "  Time for Computation: $(convert_to_ms $total_computation_time_1)"
echo "Party 2:"
echo "  Send Rounds: $total_send_rounds_2"
echo "  Recv Rounds: $total_recv_rounds_2"
echo "  Send Bytes: $(convert_to_kb $total_send_bytes_2)"
echo "  Recv Bytes: $(convert_to_kb $total_recv_bytes_2)"
echo "  Time for Reading File and Creating Circuits: $(convert_to_ms $total_reading_time_2)"
echo "  Time for Computation: $(convert_to_ms $total_computation_time_2)"

{
    echo "Emp-tool AES Total Results after 100 Iterations:"
    echo "Party 1:"
    echo "  Send Rounds: $total_send_rounds_1"
    echo "  Recv Rounds: $total_recv_rounds_1"
    echo "  Send Bytes: $(convert_to_kb $total_send_bytes_1)"
    echo "  Recv Bytes: $(convert_to_kb $total_recv_bytes_1)"
    echo "  Time for Reading File and Creating Circuits: $(convert_to_ms $total_reading_time_1)"
    echo "  Time for Computation: $(convert_to_ms $total_computation_time_1)"
    echo "Party 2:"
    echo "  Send Rounds: $total_send_rounds_2"
    echo "  Recv Rounds: $total_recv_rounds_2"
    echo "  Send Bytes: $(convert_to_kb $total_send_bytes_2)"
    echo "  Recv Bytes: $(convert_to_kb $total_recv_bytes_2)"
    echo "  Time for Reading File and Creating Circuits: $(convert_to_ms $total_reading_time_2)"
    echo "  Time for Computation: $(convert_to_ms $total_computation_time_2)"
} > aes_result.log

rm output1.log
rm output2.log
echo "Results saved to ./build/aes_result.log"