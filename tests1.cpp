#include <gtest/gtest.h>
#include "common.h"
#include <cstdio>


class RingBufferTest : public ::testing::Test {
protected:
    std::string testFile = "test_data.bin";

    void SetUp() override {
        // Перед каждым тестом создаем чистый файл на 3 записи
        InitFile(testFile, 3);
    }

    void TearDown() override {
        // После каждого теста удаляем файл
        std::remove(testFile.c_str());
    }
};

// Тест 1: Проверка инициализации
TEST_F(RingBufferTest, InitializationCorrect) {
    FileHeader h = GetHeader(testFile);
    EXPECT_EQ(h.head, 0);
    EXPECT_EQ(h.tail, 0);
    EXPECT_EQ(h.capacity, 3);
}

// Тест 2: Проверка записи и движения хвоста (tail)
TEST_F(RingBufferTest, WriteMovesTail) {
    Message msg;
    strcpy_s(msg.text, "Test1");

    WriteMessageToFile(testFile, msg);

    FileHeader h = GetHeader(testFile);
    EXPECT_EQ(h.tail, 1); // Хвост сдвинулся
    EXPECT_EQ(h.head, 0); // Голова на месте
}

// Тест 3: Проверка чтения и движения головы (head)
TEST_F(RingBufferTest, ReadMovesHead) {
    // Сначала пишем
    Message msgOut;
    strcpy_s(msgOut.text, "Hello");
    WriteMessageToFile(testFile, msgOut);

    // Потом читаем
    Message msgIn = ReadMessageFromFile(testFile);

    EXPECT_STREQ(msgIn.text, "Hello"); // Текст совпадает

    FileHeader h = GetHeader(testFile);
    EXPECT_EQ(h.head, 1); // Голова сдвинулась
}

// Тест 4: Проверка кольцевого переполнения (FIFO)
TEST_F(RingBufferTest, CircularBufferLogic) {
    // Емкость 3. Пишем 3 раза.
    Message m;
    strcpy_s(m.text, "1"); WriteMessageToFile(testFile, m);
    strcpy_s(m.text, "2"); WriteMessageToFile(testFile, m);
    strcpy_s(m.text, "3"); WriteMessageToFile(testFile, m);

    FileHeader h = GetHeader(testFile);
    EXPECT_EQ(h.tail, 0); // Хвост должен вернуться в начало (3 % 3 = 0)

    // Читаем первое сообщение
    Message mRead = ReadMessageFromFile(testFile);
    EXPECT_STREQ(mRead.text, "1"); // Должны прочитать самое старое
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}