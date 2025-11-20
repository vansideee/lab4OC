#include "common.h"
#include <vector>

int main() {
    setlocale(LC_ALL, "Russian");
    std::string filename;
    int recordsCount;
    int sendersCount;

    std::cout << "Введите имя бинарного файла: ";
    std::cin >> filename;
    std::cout << "Введите количество записей в файле: ";
    std::cin >> recordsCount;

    if (!InitFile(filename, recordsCount)) {
        std::cerr << "Ошибка создания файла!" << std::endl;
        return 1;
    }

    HANDLE hMutex = CreateMutexA(NULL, FALSE, MUTEX_NAME.c_str());
    HANDLE hSemEmpty = CreateSemaphoreA(NULL, recordsCount, recordsCount, SEM_EMPTY_NAME.c_str());
    HANDLE hSemFull = CreateSemaphoreA(NULL, 0, recordsCount, SEM_FULL_NAME.c_str());
    HANDLE hSemReady = CreateSemaphoreA(NULL, 0, 100, SEM_READY_NAME.c_str());

    if (!hMutex || !hSemEmpty || !hSemFull || !hSemReady) {
        std::cerr << "Ошибка создания объектов синхронизации." << std::endl;
        return 1;
    }

    std::cout << "Введите количество процессов Sender: ";
    std::cin >> sendersCount;

    std::string cmdLineStr = "Sender.exe " + filename;
    std::vector<STARTUPINFOA> si(sendersCount);
    std::vector<PROCESS_INFORMATION> pi(sendersCount);

    for (int i = 0; i < sendersCount; ++i) {
        ZeroMemory(&si[i], sizeof(si[i]));
        si[i].cb = sizeof(si[i]);
        ZeroMemory(&pi[i], sizeof(pi[i]));

        char cmdBuffer[256];
        strcpy_s(cmdBuffer, cmdLineStr.c_str());

        if (!CreateProcessA(NULL, cmdBuffer, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[i], &pi[i])) {
            std::cerr << "Не удалось запустить Sender #" << i << " Код ошибки: " << GetLastError() << std::endl;
        }
    }

    std::cout << "Ожидание готовности всех отправителей..." << std::endl;
    for (int i = 0; i < sendersCount; ++i) {
        WaitForSingleObject(hSemReady, INFINITE);
    }
    std::cout << "Все отправители готовы!" << std::endl;

    int choice;
    while (true) {
        std::cout << "\nМеню Receiver:\n1. Читать сообщение\n0. Выход\nВаш выбор: ";
        std::cin >> choice;

        if (choice == 0) break;
        if (choice == 1) {
            WaitForSingleObject(hSemFull, INFINITE);
            WaitForSingleObject(hMutex, INFINITE);

            Message msg = ReadMessageFromFile(filename);
            std::cout << "Прочитано сообщение: " << msg.text << std::endl;

            ReleaseMutex(hMutex);
            ReleaseSemaphore(hSemEmpty, 1, NULL);
        }
    }

    CloseHandle(hMutex);
    CloseHandle(hSemEmpty);
    CloseHandle(hSemFull);
    CloseHandle(hSemReady);

    for (int i = 0; i < sendersCount; ++i) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    return 0;
}