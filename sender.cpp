#include "common.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    if (argc < 2) {
        std::cerr << "Ошибка: Не передано имя файла!" << std::endl;
        system("pause");
        return 1;
    }

    std::string filename = argv[1];

    HANDLE hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, MUTEX_NAME.c_str());
    HANDLE hSemEmpty = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SEM_EMPTY_NAME.c_str());
    HANDLE hSemFull = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, SEM_FULL_NAME.c_str());
    HANDLE hSemReady = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, SEM_READY_NAME.c_str());

    if (!hMutex || !hSemEmpty || !hSemFull || !hSemReady) {
        std::cerr << "Ошибка открытия объектов синхронизации. Код: " << GetLastError() << std::endl;
        system("pause");
        return 1;
    }

    ReleaseSemaphore(hSemReady, 1, NULL);

    int choice;
    while (true) {
        std::cout << "\nМеню Sender:\n1. Отправить сообщение\n0. Выход\nВаш выбор: ";
        std::cin >> choice;

        if (choice == 0) break;
        if (choice == 1) {
            Message msg;
            std::cout << "Введите сообщение: ";
            std::cin >> msg.text;

            if (WaitForSingleObject(hSemEmpty, 0) == WAIT_TIMEOUT) {
                std::cout << "Буфер полон. Ожидание..." << std::endl;
                WaitForSingleObject(hSemEmpty, INFINITE);
            }

            WaitForSingleObject(hMutex, INFINITE);

            WriteMessageToFile(filename, msg);
            std::cout << "Сообщение отправлено." << std::endl;

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hSemFull, 1, NULL);
        }
    }

    return 0;
}