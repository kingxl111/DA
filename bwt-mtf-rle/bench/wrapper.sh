#!/bin/bash


echo "Генерация данных..."
python3 gen.py

echo "Сборка приложения..."
g++ v3.cpp -o main

echo "Запуск compress для 1.in..."
./main compress 1.in 1.out
echo "Размер сжатого файла: " && wc -c 1.out  && echo "Размер исходного файла: " && wc -c 1.in
echo "==========================="
echo "Запуск compress для 2.in..."
./main compress 2.in 2.out
echo "Размер сжатого файла: " && wc -c 2.out  && echo "Размер исходного файла: " && wc -c 2.in
echo "==========================="
echo "Запуск compress для 3.in..."
./main compress 3.in 3.out
echo "Размер сжатого файла: " && wc -c 3.out  && echo "Размер исходного файла: " && wc -c 3.in
echo "==========================="
echo "Запуск compress для 4.in..."
./main compress 4.in 4.out
echo "Размер сжатого файла: " && wc -c 4.out  && echo "Размер исходного файла: " && wc -c 4.in
echo "==========================="
echo " "
echo "///////////////////////////"
echo " "
echo "Запуск decompress для 1.in..."
./main decompress 1.out 11.in
echo "Проверка целостности файла 1 после разжатия: " && diff 1.in 11.in
echo "==========================="
echo "Запуск decompress для 2.in..."
./main decompress 2.out 22.in
echo "Проверка целостности файла 2 после разжатия: " && diff  2.in 22.in
echo "==========================="
echo "Запуск decompress для 3.in..."
./main decompress 3.out 33.in
echo "Проверка целостности файла 3 после разжатия: " && diff  3.in 33.in
echo "==========================="
echo "Запуск decompress для 4.in..."
./main decompress 4.out 44.in
echo "Проверка целостности файла 4 после разжатия: " && diff  4.in 44.in
echo "==========================="
echo " "


rm main *.in *.out