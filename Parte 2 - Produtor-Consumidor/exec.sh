# #!/bin/bash

# Valores para N
N_VALUES=(1 10 100 1000)

# Valores para NP e NC
THREAD_COMBINATIONS=("1 1" "1 2" "1 4" "1 8" "2 1" "4 1" "8 1")

# Compilação do programa
gcc -pthread -o produtor_consumidor produtor_consumidor.c

# Função para calcular o tempo médio
calculate_average_time() {
  local total_time=$1
  local num_executions=$2
  local average_time=$(echo "scale=4; $total_time / $num_executions" | bc)
  echo $average_time
}

# Loop para registro dos tempos - roda o programa 10 vezes para cada combinação N, NP, NC
# for n in "${N_VALUES[@]}"; do
#   # Loop para cada combinação de NP e NC
#   for combination in "${THREAD_COMBINATIONS[@]}"; do
#     np=$(echo $combination | cut -d' ' -f1)
#     nc=$(echo $combination | cut -d' ' -f2)
#     times_filename="saidas/tempos/tempos_n${n}_np${np}_nc${nc}.csv"
#     echo "N, NP, NC, Tempo\n" >> $times_filename
#     # Executa o programa NUM_EXECUTIONS vezes
#     for ((i=1; i<=10; i++)); do
#       ./produtor_consumidor $n $np $nc $times_filename ""
#     done
#   done
# done

# Loop para registro da ocupação da memória compartilhada para combinação N, NP, NC
# for n in "${N_VALUES[@]}"; do
#   # Loop para cada combinação de NP e NC
#   for combination in "${THREAD_COMBINATIONS[@]}"; do
#     np=$(echo $combination | cut -d' ' -f1)
#     nc=$(echo $combination | cut -d' ' -f2)
#     occupation_filename="saidas/ocupacoes/ocupacao_n${n}_np${np}_nc${nc}.csv"
#     echo "Ocupacao, Tempo\n" >> $occupation_filename
#     ./produtor_consumidor $n $np $nc "" $occupation_filename
#   done
# done

# Cálculo das médias dos tempos e registro em arquivo csv
# output_file="saidas/tempos/tempos_medios.csv"
# echo "N, NP/NC, Tempo Médio">> $output_file
# for N in "${N_VALUES[@]}"; do
#   # Loop para cada combinação de NP e NC
#   for combination in "${THREAD_COMBINATIONS[@]}"; do
#     NP=$(echo $combination | cut -d' ' -f1)
#     NC=$(echo $combination | cut -d' ' -f2)
#     input_file="saidas/tempos/tempos_n${N}_np${NP}_nc${NC}.csv"
#     sum=0
#     count=0
#     while IFS=, read -r n np nc time; do
#       if [[ $n == "N" ]]; then
#         continue
#       fi
#       sum=$(echo "$sum + $time" | bc)
#       # Increment the count
#       count=$((count + 1))
#       average=$(echo "scale=6; $sum / $count" | bc)
#       if [ $count = 9 ]; then
#         npnc=$(echo "${np}/${nc}" | tr -d ' ')
#         echo "${n}, (${npnc}), ${average}" >> $output_file
#       fi
#     done < "$input_file"
#   done
# done

# Execução de script gnuplot para gerar gráficos para os arquivos de ocupação gerados
gnuplot plot_occupations.gp
gnuplot plot_avg_times.gp
