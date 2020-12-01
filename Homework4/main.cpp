//Дадугин Егор Артемович
//БПИ191
//Вариант 12
//Определить индексы i, j, для которых существует наиболее длинная последовательность
// А[i] < A[i+1] < A[i+2] < A[i+3] < ... < A[j].
// Входные данные: массив чисел А, произвольной длины большей 1000. Количество потоков является входным параметром.

#include <iostream>
#include "omp.h"
#include <vector>

int threadsNumber;

void findLIS(std::vector<int> a, int *d) {
#pragma opm parallel num_threads(treadsNumber){
    int n = a.size();
#pragma omp for
    for (int i = 0; i < n; ++i) {
        d[i] = 1;
        for (int j = 0; j < i; ++j) {
            if (a[j] > a[i] and d[j] + 1 > d[i]) {
                d[i] = d[j] + 1;
            }
        }
    }
}


int main() {
    int maxThreadsNumber = 1000;
    int d[1000];
    std::cout << "Enter the number of threads: ";
    std::cin >> threadsNumber;
    if (threadsNumber > maxThreadsNumber) {
        threadsNumber = maxThreadsNumber;
    }
    std::vector<int> array(1000);
    for (int i = 0; i < array.size(); ++i) {
        array[i] = rand() % 10 + 1;
        std::cout << array[i] << std::endl;
    }

    findLIS(array, d);
    int length = 0, endIndex = 0;
    for (int i = 0; i < 1000; ++i) {
        if (d[i] > length) {
            length = d[i];
            endIndex = i;
        }
    }

    std::cout << "Start index: " << endIndex - length + 1 << std::endl;
    std::cout << "End index: " << endIndex;
    return 0;
}
