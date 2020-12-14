//Дадугин Егор Артемович
//БПИ191
//Вариант 12
//Задача о больнице.
//В больнице два врача принимают пациентов, выслушивают их жалобы и отправляют их или к стоматологу
// или к хирургу или к терапевту. Стоматолог, хирург и терапевт лечат пациента.
// Каждый врач может принять только одного пациента за раз.
// Пациенты стоят в очереди к врачам и никогда их не покидают.
// Создать многопоточное приложение, моделирующее рабочий день клиники.

#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <random>

//Потокобесопасная очередь
template<typename T>
class concurrent_queue {
private:
    std::queue<T> queue;
    std::mutex mutex;
public:
    void push(const T data) {
        std::unique_lock<std::mutex> locker(mutex);
        queue.push(data);
        locker.unlock();
    }

    bool empty() {
        std::unique_lock<std::mutex> locker(mutex);
        bool result = queue.empty();
        locker.unlock();
        return result;
    }

    T front() {
        std::unique_lock<std::mutex> locker(mutex);
        T result = queue.front();
        locker.unlock();
        return result;
    }

    void pop() {
        std::unique_lock<std::mutex> locker(mutex);
        queue.pop();
        locker.unlock();
    }
};

std::mutex mutex;
std::mutex mutexDentist;
std::mutex mutexTherapist;
std::mutex mutexSurgeon;
concurrent_queue<std::string> patients;
concurrent_queue<std::string> dentistPatients;
concurrent_queue<std::string> therapistPatients;
concurrent_queue<std::string> surgeonPatients;
std::condition_variable dentist;
std::condition_variable therapist;
std::condition_variable surgeon;
std::default_random_engine defEngine(time(NULL));

//Функция для генерации случайного числа
int getRand() {
    std::uniform_int_distribution<int> randomNum(0, 2);
    return randomNum(defEngine);
}

//Функция для докторов
void doctorFunc(std::string doctorName) {
    while (!patients.empty()) {
        std::unique_lock<std::mutex> locker(mutex);
        std::string patientName = patients.front();
        patients.pop();
        std::cout << "Доктор " << doctorName << " принимает пациента " << patientName << std::endl;
        locker.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int nextDoc = getRand();
        if (nextDoc == 0) {
            locker.lock();
            std::cout << "Доктор " << doctorName << " направил пациента " << patientName << " к стоматологу"
                      << std::endl;
            locker.unlock();
            //Добавляем пациента в очередь к стоматологу
            dentistPatients.push(patientName);
            //Сообщаем стоматологу о наличии пациента
            dentist.notify_all();
        } else if (nextDoc == 1) {
            locker.lock();
            std::cout << "Доктор " << doctorName << " направил пациента " << patientName << " к терапевту"
                      << std::endl;
            locker.unlock();
            //Добавляем пациента в очередь к терапевту
            therapistPatients.push(patientName);
            //Сообщаем терапевту о наличии пациента
            therapist.notify_all();
        } else {
            locker.lock();
            std::cout << "Доктор " << doctorName << " направил пациента " << patientName << " к хирургу"
                      << std::endl;
            locker.unlock();
            //Добавляем пациента в очередь к хирургу
            surgeonPatients.push(patientName);
            //Сообщаем хирургу о наличии пациента
            surgeon.notify_all();
        }
    }
}

//Функция для стоматолога
void dentistFunc(std::string doctorName) {
    while (true) {
        if (dentistPatients.empty()) {
            std::unique_lock<std::mutex> dentistLocker(mutexDentist);
            //Стоматолог ждет пациентов
            dentist.wait_for(dentistLocker, std::chrono::seconds(5));
            if (patients.empty() && dentistPatients.empty()) return;
        }

        if (!dentistPatients.empty()) {
            std::unique_lock<std::mutex> locker(mutex);
            std::cout << "Стоматолог " << doctorName << " принимает пациента " << dentistPatients.front() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Стоматолог " << doctorName << " закончил прием пациента " << dentistPatients.front()
                      << std::endl;
            locker.unlock();
            dentistPatients.pop();
        }
    }
}

//Функция для терапевта
void therapistFunc(std::string doctorName) {
    while (true) {
        if (therapistPatients.empty()) {
            std::unique_lock<std::mutex> therapistLocker(mutexTherapist);
            //Терапевт ждет пациентов
            therapist.wait_for(therapistLocker, std::chrono::seconds(5));
            if (patients.empty() && therapistPatients.empty()) return;
        }

        if (!therapistPatients.empty()) {
            std::unique_lock<std::mutex> locker(mutex);
            std::cout << "Терапевт " << doctorName << " принимает пациента " << therapistPatients.front() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Терапевт " << doctorName << " закончил прием пациента " << therapistPatients.front()
                      << std::endl;
            locker.unlock();
            therapistPatients.pop();
        }
    }
}

//Функция для хирурга
void surgeonFunc(std::string doctorName) {
    while (true) {
        if (surgeonPatients.empty()) {
            std::unique_lock<std::mutex> surgeonLocker(mutexSurgeon);
            //Хирург ждет пациентов
            surgeon.wait_for(surgeonLocker, std::chrono::seconds(5));
            if (patients.empty() && surgeonPatients.empty()) return;
        }


        if (!surgeonPatients.empty()) {
            std::unique_lock<std::mutex> locker(mutex);
            std::cout << "Хирург " << doctorName << " принимает пациента " << surgeonPatients.front() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Хирург " << doctorName << " закончил прием пациента " << surgeonPatients.front() << std::endl;
            locker.unlock();
            surgeonPatients.pop();
        }
    }
}

int main() {
    //Создание пациентов
    std::string names[9] = {"Валерий", "Петр", "Владимир", "Полина", "Мария", "Геннадий", "Людмила", "Валерия",
                            "Павел"};
    for (int i = 0; i < 9; ++i) {
        patients.push(names[i]);
    }
    //Создание потоков для докторов
    std::thread doctor1(doctorFunc, "Василий");
    std::thread doctor2(doctorFunc, "Борис");
    std::thread dentistDoc(dentistFunc, "Виталий");
    std::thread therapistDoc(therapistFunc, "Григорий");
    std::thread surgeonDoc(surgeonFunc, "Константин");
    doctor1.join();
    doctor2.join();
    dentistDoc.join();
    therapistDoc.join();
    surgeonDoc.join();
    std::cout << "Рабочий день закончен" << std::endl;
    return 0;
}
