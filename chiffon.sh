#!/bin/bash # announce which shell the script use

# get nums from command lines
while getopts m:n:l: flag
do
    case "${flag}" in
    m) M=${OPTARG};; # hosts
    n) N=${OPTARG};; # players
    l) L=${OPTARG};; # lucky number
    esac
done
#create FIFOs
for ((i = 0; i <= M; i++)); do # build
    mkfifo "fifo_$i.tmp"
    exec {FD}<> "fifo_$i.tmp"
done
# conbine 
declare -A combination
count=1
for ((a = 1; a <= N; a++)); do
    for ((b = a + 1; b <= N; b++)); do
        for ((c = b + 1; c <= N; c++)); do
            for ((d = c + 1; d <= N; d++)); do
                for ((e = d + 1; e <= N; e++)); do
                    for ((f = e + 1; f <= N; f++)); do
                        for ((g = f + 1; g <= N; g++)); do
                            for ((h = g + 1; h <= N; h++)); do
                                combination[$count, 1]=${a}
                                combination[$count, 2]=${b}
                                combination[$count, 3]=${c}
                                combination[$count, 4]=${d}
                                combination[$count, 5]=${e}
                                combination[$count, 6]=${f}
                                combination[$count, 7]=${g}
                                combination[$count, 8]=${h}
                                count=$(($count+1))
                            done
                        done
                    done
                done
            done
        done
    done
done
count=$(($count-1))
# distribute to hosts
maxhost=${M}
declare -A record
declare -i host_id
for ((i = 0; i <= 12; i++)); do
    record[$i]=0
done
if (($maxhost > $count)); then # hosts more than or equal to games
    maxhost=$(($count))
    for ((i = 1; i <= $maxhost; i++)); do
        #echo "${combination[$i, 1]} ${combination[$i, 2]} ${combination[$i, 3]} ${combination[$i, 4]} ${combination[$i, 5]} ${combination[$i, 6]} ${combination[$i, 7]} ${combination[$i, 8]}"
        echo "${combination[$i, 1]} ${combination[$i, 2]} ${combination[$i, 3]} ${combination[$i, 4]} ${combination[$i, 5]} ${combination[$i, 6]} ${combination[$i, 7]} ${combination[$i, 8]}" > "fifo_${i}.tmp" &
        ./host -m ${i} -d 0 -l ${L} &
    done
    index=0 # every host return
    while (($index < $count)); do
        check=0 # 9 lines
        while read -r first second; do
            if (($check == 0)); then
                host_id=$(($first))
                check=$(($check+1))
            else
                record[$first]=$((record[$first]+$second))
                check=$(($check+1))
                if (($check == 9)); then
                    index=$(($index+1))
                    break
                fi
            fi
        done < fifo_0.tmp
    done
    for ((i = 0; i <= M; i++)); do
        echo "-1 -1 -1 -1 -1 -1 -1 -1" > "fifo_$i.tmp" &
    done
else # hosts less than games
    for ((i = 1; i <= $maxhost; i++)); do #pre-distribute
        # echo "${combination[$i, 0]} ${combination[$i, 1]} ${combination[$i, 2]} ${combination[$i, 3]} ${combination[$i, 4]} ${combination[$i, 5]} ${combination[$i, 6]} ${combination[$i, 7]}"
        echo "${combination[$i, 1]} ${combination[$i, 2]} ${combination[$i, 3]} ${combination[$i, 4]} ${combination[$i, 5]} ${combination[$i, 6]} ${combination[$i, 7]} ${combination[$i, 8]}" > "fifo_${i}.tmp" &
        ./host -m ${i} -d 0 -l ${L} &
    done
    index=0
    rest_combination=$(($maxhost+1))
    while (($index < $count)); do
        check=0
        while read -r first second; do
            if (($check == 0)); then
                host_id=$(($first))
                #echo "${combination[$rest_combination, 0]} ${combination[$rest_combination, 1]} ${combination[$rest_combination, 2]} ${combination[$rest_combination, 3]} ${combination[$rest_combination, 4]} ${combination[$rest_combination, 5]} ${combination[$rest_combination, 6]} ${combination[$rest_combination, 7]}"
                echo "${combination[$rest_combination, 1]} ${combination[$rest_combination, 2]} ${combination[$rest_combination, 3]} ${combination[$rest_combination, 4]} ${combination[$rest_combination, 5]} ${combination[$rest_combination, 6]} ${combination[$rest_combination, 7]} ${combination[$rest_combination, 8]}" > "fifo_${host_id}.tmp" &
                check=$(($check+1))
                rest_combination=$(($rest_combination+1))
            else
                record[$first]=$((record[$first]+$second))
                check=$(($check+1))
                if (($check == 9)); then
                    index=$(($index+1))
                    break
                fi
            fi
        done < "fifo_0.tmp"
    done
    for ((i = 0; i <= M; i++)); do
        echo "-1 -1 -1 -1 -1 -1 -1 -1" > "fifo_$i.tmp" &
    done
fi
declare -A ranking
for ((i = 1;i <= N; i++)); do
    temp=$N
    for ((j = 1; j <= N; j++)); do
        if (( record[$i] >= record[$j] )); then
            temp=$(($temp-1))
        fi
    done
    ranking[$i]=$(($temp+1))
done
#for ((i = 1; i <= N; i++)); do
#    echo "$((${record[$i]}))"
#done
for ((i = 1; i <= N; i++)); do
    echo "$i $((${ranking[$i]}))"
done
for ((i = 0; i <= M; i++)); do
    rm "fifo_${i}.tmp"
done
for ((i = 1; i <= M; i++)); do
    wait
done

exit 0