//Дадугин Егор Артемович
//БПИ191
//Вариант 12
//Определить индексы i, j, для которых существует наиболее длинная последовательность
// А[i] < A[i+1] < A[i+2] < A[i+3] < ... < A[j].
// Входные данные: массив чисел А, произвольной длины большей 1000. Количество потоков является входным параметром.

#include <iostream>
#include <vector>
#include <thread>

void
func(std::vector<int> &array, int currentInd, int lastInd, int &finalI, int &finalJ, int &maxLength, int &prevLength,
     int &currentI) {
    int currentLength = 1;
    if (currentInd - 1 >= 0)
        if (array[currentInd - 1] < array[currentInd]) {
            currentLength = prevLength + 1;
        } else
            currentI = currentInd;
    for (int i = currentInd; i < lastInd - 1; ++i) {
        if (array[i] < array[i + 1]) {
            currentLength++;
        } else if (maxLength <= currentLength) {
            maxLength = currentLength;
            finalI = currentI;
            finalJ = i;
            currentI = i + 1;
            currentLength = 1;
        }
        if (i == array.size() - 2) {
            if (maxLength <= currentLength) {
                maxLength = currentLength;
                finalI = currentI;
                finalJ = i+1;
                currentI = i + 1;
                currentLength = 1;
            }
        }
    }
    prevLength = currentLength;
}

int main() {
    int threadsNumber, maxThreadsNumber = 1000;
    int finalI = -1, finalJ = -1, maxLength = 0, currentInd = 0, lastInd, prevLength = 0, currentI = 0;
    std::cout << "Enter the number of threads: ";
    std::cin >> threadsNumber;
    if (threadsNumber > maxThreadsNumber) {
        threadsNumber = maxThreadsNumber;
    }
    std::vector<int> array(1000);
    for (int i = 0; i < array.size(); ++i) {
        array[i] = rand() % 20 + 1;
        std::cout << array[i] << std::endl;
    }
    lastInd = array.size() / threadsNumber;
    for (int i = 0; i < threadsNumber; ++i) {
        if (i == threadsNumber - 1)
            lastInd = array.size();
        (new std::thread{func, std::ref(array), currentInd, lastInd, std::ref(finalI), std::ref(finalJ),
                         std::ref(maxLength), std::ref(prevLength), std::ref(currentI)})->join();
        currentInd += array.size() / threadsNumber;
        lastInd += array.size() / threadsNumber;
    }

    std::cout << "Start index: " << finalI << std::endl;
    std::cout << "End index: " << finalJ;
    return 0;
}
