#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// Константы
const std::string MUTEX_NAME = "Lab4_FileMutex";
const std::string SEM_EMPTY_NAME = "Lab4_SemEmpty";
const std::string SEM_FULL_NAME = "Lab4_SemFull";
const std::string SEM_READY_NAME = "Lab4_SemReady";

struct Message {
    char text[20];
};

struct FileHeader {
    int head;
    int tail;
    int capacity;
};



// Инициализация файла
inline bool InitFile(const std::string& filename, int recordsCount) {
    std::fstream file(filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    FileHeader header = { 0, 0, recordsCount };
    file.write((char*)&header, sizeof(FileHeader));

    Message emptyMsg = { "" };
    for (int i = 0; i < recordsCount; ++i) {
        file.write((char*)&emptyMsg, sizeof(Message));
    }
    file.close();
    return true;
}

// Запись сообщения (логика Sender)
inline void WriteMessageToFile(const std::string& filename, const Message& msg) {
    std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (fs.is_open()) {
        FileHeader h;
        fs.read((char*)&h, sizeof(FileHeader));

        int offset = sizeof(FileHeader) + h.tail * sizeof(Message);
        fs.seekp(offset, std::ios::beg);
        fs.write((char*)&msg, sizeof(Message));

        h.tail = (h.tail + 1) % h.capacity;

        fs.seekp(0, std::ios::beg);
        fs.write((char*)&h, sizeof(FileHeader));
        fs.close();
    }
}

// Чтение сообщения (логика Receiver)
inline Message ReadMessageFromFile(const std::string& filename) {
    Message msg = { "" };
    std::fstream fs(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (fs.is_open()) {
        FileHeader h;
        fs.read((char*)&h, sizeof(FileHeader));

        int offset = sizeof(FileHeader) + h.head * sizeof(Message);
        fs.seekg(offset, std::ios::beg);
        fs.read((char*)&msg, sizeof(Message));

        h.head = (h.head + 1) % h.capacity;

        fs.seekp(0, std::ios::beg);
        fs.write((char*)&h, sizeof(FileHeader));
        fs.close();
    }
    return msg;
}

// Функция для чтения заголовка (для тестов)
inline FileHeader GetHeader(const std::string& filename) {
    FileHeader h = { 0,0,0 };
    std::fstream fs(filename, std::ios::in | std::ios::binary);
    if (fs.is_open()) {
        fs.read((char*)&h, sizeof(FileHeader));
        fs.close();
    }
    return h;
}