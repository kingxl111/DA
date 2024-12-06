#!/bin/bash


echo "Генерация данных в 1.in..."
echo "compress" > 1.in
python3 gen.py >> 1.in

echo "Запуск compress..."
start_time=$(date +%s.%N)  
g++ main.cpp -o compress
./compress < 1.in > 1.out
end_time=$(date +%s.%N) 
compress_time=$(echo "$end_time - $start_time" | bc)
echo "Время работы compress: $compress_time секунд"

echo "Создание файла 2.in..."
echo "decompress" > 2.in
cat 1.out >> 2.in

echo "Запуск decompress..."
start_time=$(date +%s.%N)  
g++ main.cpp -o decompress
./decompress < 2.in > 2.out
end_time=$(date +%s.%N)  
decompress_time=$(echo "$end_time - $start_time" | bc)
echo "Время работы decompress: $decompress_time секунд"

rm compress decompress 