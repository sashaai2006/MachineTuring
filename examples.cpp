#include "MT.h"
#include <iostream>
#include <vector>
#include <string>

/**
 * Пример 1: Машина Тьюринга, которая инвертирует бинарную строку
 * (0 -> 1, 1 -> 0)
 */
void ExampleBinaryInverter() {
    std::cout << "=== Пример 1: Инвертирование бинарной строки ===" << std::endl;
    
    // Используем char для символов и string для состояний
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Добавляем правила:
    // Из START: при '0' -> заменяем на '1' и идём вправо
    tm.AddRule("START", '0', "START", '1', Direction::RIGHT);
    // Из START: при '1' -> заменяем на '0' и идём вправо
    tm.AddRule("START", '1', "START", '0', Direction::RIGHT);
    // При пустом символе (конец строки) -> переходим в конечное состояние
    tm.AddRule("START", ' ', "FINAL", ' ', Direction::STAY);
    
    // Добавляем конечное состояние
    tm.AddFinalState("FINAL");
    
    // Задаём входную строку
    std::vector<char> input = {'1', '0', '1', '1', '0', '0', '1'};
    tm.SetInput(input);
    
    std::cout << "Входная строка: ";
    for (char c : input) std::cout << c;
    std::cout << std::endl << std::endl;
    
    tm.PrintState();
    
    // Выполняем машину
    bool success = tm.Run();
    
    std::cout << std::endl;
    tm.PrintState();
    
    std::cout << std::endl << "Результат: ";
    if (success) {
        std::cout << "Машина завершила работу в конечном состоянии" << std::endl;
        
        auto result_tape = tm.GetTapeSegment(0, input.size());
        std::cout << "Результирующая строка: ";
        for (char c : result_tape) std::cout << c;
        std::cout << std::endl;
    } else {
        std::cout << "Машина остановилась не в конечном состоянии" << std::endl;
    }
    
    std::cout << "Шагов выполнено: " << tm.GetStepCount() << std::endl << std::endl;
}

/**
 * Пример 2: Машина Тьюринга, которая проверяет палиндром
 * (строка читается одинаково слева направо и справа налево)
 */
void ExamplePalindromeChecker() {
    std::cout << "=== Пример 2: Проверка палиндрома ===" << std::endl;
    
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Проверяем палиндром для строки из 'a' и 'b'
    
    // Основная логика: машина читает символ слева, запоминает его,
    // затем идёт в конец строки, сравнивает с последним символом
    
    // Начало: читаем первый символ
    tm.AddRule("START", 'a', "FIND_END_A", 'X', Direction::RIGHT);  // Запомнили 'a', отметили как 'X'
    tm.AddRule("START", 'b', "FIND_END_B", 'X', Direction::RIGHT);  // Запомнили 'b', отметили как 'X'
    tm.AddRule("START", ' ', "ACCEPT", ' ', Direction::STAY);       // Пустая строка - палиндром
    tm.AddRule("START", 'X', "START", 'X', Direction::RIGHT);      // Пропускаем уже обработанные
    
    // Ищем конец строки для 'a'
    tm.AddRule("FIND_END_A", 'a', "FIND_END_A", 'a', Direction::RIGHT);
    tm.AddRule("FIND_END_A", 'b', "FIND_END_A", 'b', Direction::RIGHT);
    tm.AddRule("FIND_END_A", 'X', "FIND_END_A", 'X', Direction::RIGHT);
    tm.AddRule("FIND_END_A", ' ', "BACK_A", ' ', Direction::LEFT);    // Нашли конец, возвращаемся
    
    // Возвращаемся к последнему символу для 'a'
    tm.AddRule("BACK_A", 'a', "RETURN_A", 'X', Direction::LEFT);     // Нашли соответствующий 'a'
    tm.AddRule("BACK_A", 'b', "REJECT", 'b', Direction::STAY);       // Не палиндром!
    tm.AddRule("BACK_A", 'X', "BACK_A", 'X', Direction::LEFT);      // Пропускаем обработанные
    
    // Аналогично для 'b'
    tm.AddRule("FIND_END_B", 'a', "FIND_END_B", 'a', Direction::RIGHT);
    tm.AddRule("FIND_END_B", 'b', "FIND_END_B", 'b', Direction::RIGHT);
    tm.AddRule("FIND_END_B", 'X', "FIND_END_B", 'X', Direction::RIGHT);
    tm.AddRule("FIND_END_B", ' ', "BACK_B", ' ', Direction::LEFT);
    
    tm.AddRule("BACK_B", 'b', "RETURN_B", 'X', Direction::LEFT);
    tm.AddRule("BACK_B", 'a', "REJECT", 'a', Direction::STAY);
    tm.AddRule("BACK_B", 'X', "BACK_B", 'X', Direction::LEFT);
    
    // Возвращаемся в начало
    tm.AddRule("RETURN_A", 'a', "RETURN_A", 'a', Direction::LEFT);
    tm.AddRule("RETURN_A", 'b', "RETURN_A", 'b', Direction::LEFT);
    tm.AddRule("RETURN_A", 'X', "RETURN_A", 'X', Direction::LEFT);
    tm.AddRule("RETURN_A", ' ', "START", ' ', Direction::RIGHT);
    
    tm.AddRule("RETURN_B", 'a', "RETURN_B", 'a', Direction::LEFT);
    tm.AddRule("RETURN_B", 'b', "RETURN_B", 'b', Direction::LEFT);
    tm.AddRule("RETURN_B", 'X', "RETURN_B", 'X', Direction::LEFT);
    tm.AddRule("RETURN_B", ' ', "START", ' ', Direction::RIGHT);
    
    // Конечные состояния
    tm.AddFinalState("ACCEPT");  // Палиндром!
    tm.AddFinalState("REJECT");  // Не палиндром
    
    // Тестируем на палиндроме
    std::vector<char> input1 = {'a', 'b', 'b', 'a'};
    tm.SetInput(input1);
    
    std::cout << "Входная строка: ";
    for (char c : input1) std::cout << c;
    std::cout << std::endl;
    
    bool result1 = tm.Run();
    std::cout << "Результат: " << (tm.GetCurrentState() == "ACCEPT" ? "Палиндром!" : "Не палиндром") << std::endl;
    std::cout << "Шагов: " << tm.GetStepCount() << std::endl << std::endl;
    
    // Тестируем на не-палиндроме
    tm.Reset();
    std::vector<char> input2 = {'a', 'b', 'a', 'b'};
    tm.SetInput(input2);
    
    std::cout << "Входная строка: ";
    for (char c : input2) std::cout << c;
    std::cout << std::endl;
    
    bool result2 = tm.Run();
    std::cout << "Результат: " << (tm.GetCurrentState() == "ACCEPT" ? "Палиндром!" : "Не палиндром") << std::endl;
    std::cout << "Шагов: " << tm.GetStepCount() << std::endl << std::endl;
}

/**
 * Пример 3: Машина Тьюринга для сложения двух чисел в унарном коде
 * Унарный код: число n представлено n символами '1'
 * Например: 3 + 2 = "111+11" -> "11111"
 */
void ExampleUnaryAddition() {
    std::cout << "=== Пример 3: Сложение в унарном коде ===" << std::endl;
    
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Логика: находим '+', заменяем на '1', идём в конец и стираем одну '1'
    
    // Ищем знак '+'
    tm.AddRule("START", '1', "START", '1', Direction::RIGHT);
    tm.AddRule("START", '+', "FIND_END", '1', Direction::RIGHT);  // Заменяем '+' на '1'
    
    // Ищем конец второго числа
    tm.AddRule("FIND_END", '1', "FIND_END", '1', Direction::RIGHT);
    tm.AddRule("FIND_END", ' ', "DELETE_ONE", ' ', Direction::LEFT);
    
    // Удаляем одну '1' в конце
    tm.AddRule("DELETE_ONE", '1', "FINAL", ' ', Direction::STAY);
    
    tm.AddFinalState("FINAL");
    
    // Тестируем: 3 + 2 = 5
    std::vector<char> input = {'1', '1', '1', '+', '1', '1'};
    tm.SetInput(input);
    
    std::cout << "Входное выражение: ";
    for (char c : input) std::cout << c;
    std::cout << " (это 3 + 2)" << std::endl;
    
    bool success = tm.Run();
    
    if (success) {
        auto result = tm.GetTapeSegment(0, 10);
        std::cout << "Результат на ленте: ";
        for (char c : result) {
            if (c == ' ') break;
            std::cout << c;
        }
        
        // Подсчитываем количество единиц
        int count = 0;
        for (char c : result) {
            if (c == '1') count++;
            else if (c == ' ') break;
        }
        
        std::cout << " (= " << count << " в десятичном виде)" << std::endl;
    }
    
    std::cout << "Шагов выполнено: " << tm.GetStepCount() << std::endl << std::endl;
}

/**
 * Главная функция для запуска примеров
 */
int main() {
    std::cout << "Примеры работы машины Тьюринга" << std::endl;
    std::cout << "==========================================" << std::endl << std::endl;
    
    try {
        ExampleBinaryInverter();
        ExamplePalindromeChecker();
        ExampleUnaryAddition();
        
        std::cout << "Все примеры выполнены успешно!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
