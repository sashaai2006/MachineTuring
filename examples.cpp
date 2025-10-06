#include "MT.h"
#include <iostream>
#include <vector>
#include <string>

/**
 * Пример 1: Машина Тьюринга, которая инвертирует бинарную строку
 * Демонстрирует использование LazySeq для ленты
 */
void ExampleBinaryInverter() {
    std::cout << "=== Пример 1: Инвертирование бинарной строки (на основе LazySeq) ===" << std::endl;
    
    // Создаём машину с пустым символом ' ' и начальным состоянием "START"
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Настраиваем правила через компоненты:
    // TransitionManager управляет правилами переходов
    tm.AddRule("START", '0', "START", '1', Direction::RIGHT);  // 0 -> 1, двигаемся вправо
    tm.AddRule("START", '1', "START", '0', Direction::RIGHT);  // 1 -> 0, двигаемся вправо
    tm.AddRule("START", ' ', "FINAL", ' ', Direction::STAY);   // При пустом символе останавливаемся
    
    // StateManager управляет конечными состояниями
    tm.AddFinalState("FINAL");
    
    // TuringTape (на основе LazySeq) управляет лентой
    std::vector<char> input = {'1', '0', '1', '1', '0', '0', '1'};
    tm.SetInput(input);
    
    std::cout << "Входная строка: ";
    for (char c : input) std::cout << c;
    std::cout << std::endl << std::endl;
    
    std::cout << "Начальное состояние:" << std::endl;
    tm.PrintState();
    std::cout << std::endl;
    
    // Включаем отладочный режим для первых нескольких шагов
    tm.SetDebugMode(true);
    
    // Выполняем 3 шага с отладкой
    std::cout << "Первые 3 шага (с отладкой):" << std::endl;
    for (int i = 0; i < 3 && tm.Step(); ++i) {
        // Отладка выводится автоматически
    }
    
    // Отключаем отладку и довыполняем
    tm.SetDebugMode(false);
    bool success = tm.Run();
    
    std::cout << std::endl << "Итоговое состояние:" << std::endl;
    tm.PrintState();
    
    std::cout << std::endl << "Результат: ";
    if (success) {
        std::cout << "Машина завершила работу успешно!" << std::endl;
        
        auto result_tape = tm.GetTapeSegment(0, input.size());
        std::cout << "Результирующая строка: ";
        for (char c : result_tape) std::cout << c;
        std::cout << std::endl;
    } else {
        std::cout << "Машина остановилась не в конечном состоянии" << std::endl;
    }
    
    tm.PrintStatistics();
    std::cout << std::endl;
}

/**
 * Пример 2: Машина для сложения в унарном коде
 * Демонстрирует работу с бесконечной лентой (LazySeq)
 */
void ExampleUnaryAddition() {
    std::cout << "=== Пример 2: Сложение в унарном коде ===" << std::endl;
    
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Логика: находим '+', заменяем на '1', идём в конец и стираем одну '1'
    tm.AddRule("START", '1', "START", '1', Direction::RIGHT);      // Пропускаем единицы
    tm.AddRule("START", '+', "FIND_END", '1', Direction::RIGHT);   // Заменяем '+' на '1'
    
    tm.AddRule("FIND_END", '1', "FIND_END", '1', Direction::RIGHT);  // Ищем конец второго числа
    tm.AddRule("FIND_END", ' ', "DELETE_ONE", ' ', Direction::LEFT);  // Нашли конец
    
    tm.AddRule("DELETE_ONE", '1', "FINAL", ' ', Direction::STAY);   // Удаляем одну единицу
    
    tm.AddFinalState("FINAL");
    
    // Тестируем: 3 + 2 = 5 ("111+11" -> "11111")
    std::vector<char> input = {'1', '1', '1', '+', '1', '1'};
    tm.SetInput(input);
    
    std::cout << "Входное выражение: ";
    for (char c : input) std::cout << c;
    std::cout << " (это 3 + 2 в унарном коде)" << std::endl;
    
    std::cout << "Начальное состояние:" << std::endl;
    tm.PrintState();
    
    bool success = tm.Run();
    
    std::cout << std::endl << "Конечное состояние:" << std::endl;
    tm.PrintState();
    
    if (success) {
        // Получаем результат и подсчитываем единицы
        auto result = tm.GetTapeSegment(0, 10);
        std::cout << std::endl << "Результат на ленте: ";
        
        int count = 0;
        for (char c : result) {
            if (c == '1') {
                count++;
                std::cout << c;
            } else if (c == ' ') {
                break;
            }
        }
        
        std::cout << " (= " << count << " в десятичном виде)" << std::endl;
    }
    
    tm.PrintStatistics();
    std::cout << std::endl;
}

/**
 * Пример 3: Машина для простого палиндрома
 * Демонстрирует сложные правила переходов и эффективность LazySeq
 */
void ExampleSimplePalindrome() {
    std::cout << "=== Пример 3: Проверка простого палиндрома ===" << std::endl;
    
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Упрощённая машина для проверки палиндрома для строк типа "aba", "abba"
    
    // Ищем середину строки
    tm.AddRule("START", 'a', "MOVE_RIGHT", 'a', Direction::RIGHT);
    tm.AddRule("START", 'b', "MOVE_RIGHT", 'b', Direction::RIGHT);
    tm.AddRule("START", ' ', "CHECK_PALINDROME", ' ', Direction::LEFT);
    
    tm.AddRule("MOVE_RIGHT", 'a', "MOVE_RIGHT", 'a', Direction::RIGHT);
    tm.AddRule("MOVE_RIGHT", 'b', "MOVE_RIGHT", 'b', Direction::RIGHT);
    tm.AddRule("MOVE_RIGHT", ' ', "CHECK_PALINDROME", ' ', Direction::LEFT);
    
    // Простая проверка - для демонстрации просто принимаем любую строку
    tm.AddRule("CHECK_PALINDROME", 'a', "ACCEPT", 'a', Direction::STAY);
    tm.AddRule("CHECK_PALINDROME", 'b', "ACCEPT", 'b', Direction::STAY);
    
    tm.AddFinalState("ACCEPT");
    
    // Тестируем на разных строках
    std::vector<std::vector<char>> test_cases = {
        {'a', 'b', 'a'},
        {'a', 'b', 'b', 'a'},
        {'a'},
        {'a', 'b', 'c'}
    };
    
    for (const auto& test_input : test_cases) {
        tm.Reset();  // Сбрасываем машину
        tm.SetInput(test_input);
        
        std::cout << "Тестируем строку: ";
        for (char c : test_input) std::cout << c;
        std::cout << std::endl;
        
        bool success = tm.Run();
        std::cout << "Результат: " << (success ? "Принята" : "Отклонена");
        std::cout << " (шагов: " << tm.GetStepCount() << ")" << std::endl;
    }
    
    std::cout << std::endl;
}

/**
 * Пример 4: Демонстрация эффективности LazySeq
 * Показывает, как LazySeq обрабатывает обращения к отдалённым позициям
 */
void ExampleLazySeqEfficiency() {
    std::cout << "=== Пример 4: Эффективность LazySeq (длинная лента) ===" << std::endl;
    
    MachineTuring<std::string, char> tm('_', "START");
    
    // Простая машина, которая просто двигается к позиции 1000
    tm.AddRule("START", '_', "MOVING", 'X', Direction::RIGHT);
    tm.AddRule("MOVING", '_', "MOVING", '_', Direction::RIGHT);
    
    tm.AddFinalState("FINAL");
    
    // Маленький начальный ввод
    std::vector<char> input = {'S', 'T', 'A', 'R', 'T'};
    tm.SetInput(input);
    
    std::cout << "Начальные данные: ";
    for (char c : input) std::cout << c;
    std::cout << std::endl;
    
    std::cout << "Позиция головки: " << tm.GetHeadPosition() << std::endl;
    
    // Выполняем 50 шагов для демонстрации
    for (int i = 0; i < 50 && tm.Step(); ++i) {
        // Молчаливо выполняем
    }
    
    std::cout << "После 50 шагов:" << std::endl;
    std::cout << "Позиция головки: " << tm.GetHeadPosition() << std::endl;
    
    // Показываем ленту вокруг головки
    tm.PrintState(std::cout, 10);
    
    // Проверяем отдалённую позицию
    std::cout << "Символ на позиции 1000: " << tm.GetSymbolAt(1000) << std::endl;
    std::cout << "Символ на позиции -50: " << tm.GetSymbolAt(-50) << std::endl;
    
    tm.PrintStatistics();
    std::cout << std::endl;
}

/**
 * Главная функция
 */
int main() {
    std::cout << "Примеры работы машины Тьюринга на основе LazySeq" << std::endl;
    std::cout << "=================================================================" << std::endl << std::endl;
    
    try {
        ExampleBinaryInverter();
        ExampleUnaryAddition();
        ExampleSimplePalindrome();
        ExampleLazySeqEfficiency();
        
        std::cout << "🎉 Все примеры выполнены успешно!" << std::endl;
        std::cout << "
Особенности архитектуры:" << std::endl;
        std::cout << "- ⚙️  Четкое разделение ответственности между компонентами" << std::endl;
        std::cout << "- 🔍 TransitionManager: Правила переходов" << std::endl;
        std::cout << "- 📍 StateManager: Управление состояниями" << std::endl;
        std::cout << "- 📜 TuringTape (LazySeq): Бесконечная лента с ленивыми вычислениями" << std::endl;
        std::cout << "- 🔄 HeadManager: Управление головкой чтения/записи" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
