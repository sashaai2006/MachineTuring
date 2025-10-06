#include "MT.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>

/**
 * Простая система модульных тестов
 */
class TestFramework {
private:
    static int total_tests;
    static int passed_tests;
    
public:
    static void RunTest(const std::string& test_name, std::function<bool()> test_func) {
        total_tests++;
        std::cout << "[Тест] " << test_name << "... ";
        
        try {
            if (test_func()) {
                passed_tests++;
                std::cout << "PASS" << std::endl;
            } else {
                std::cout << "FAIL" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
    
    static void PrintSummary() {
        std::cout << std::endl << "=== Результаты тестирования ===" << std::endl;
        std::cout << "Прошло: " << passed_tests << "/" << total_tests << " тестов" << std::endl;
        if (passed_tests == total_tests) {
            std::cout << "Все тесты прошли успешно! \u2713" << std::endl;
        } else {
            std::cout << "Некоторые тесты не прошли \u2717" << std::endl;
        }
    }
};

int TestFramework::total_tests = 0;
int TestFramework::passed_tests = 0;

/**
 * Тест базового функционала машины Тьюринга
 */
bool TestBasicFunctionality() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    // Проверяем создание и начальное состояние
    if (tm.GetCurrentState() != "q0") return false;
    if (tm.GetHeadPosition() != 0) return false;
    if (tm.GetStepCount() != 0) return false;
    if (tm.ReadCurrentSymbol() != ' ') return false;
    
    return true;
}

/**
 * Тест установки входных данных
 */
bool TestInputSetting() {
    MachineTuring<std::string, char> tm('_', "start");
    
    std::vector<char> input = {'a', 'b', 'c'};
    tm.SetInput(input, 0);
    
    // Проверяем что вход установлен корректно
    if (tm.GetSymbolAt(0) != 'a') return false;
    if (tm.GetSymbolAt(1) != 'b') return false;
    if (tm.GetSymbolAt(2) != 'c') return false;
    if (tm.GetSymbolAt(3) != '_') return false;  // Пустой символ
    
    return true;
}

/**
 * Тест простого перехода
 */
bool TestSimpleTransition() {
    MachineTuring<std::string, char> tm('_', "q0");
    
    // Правило: из q0 при 'a' -> в q1, записать 'b', сдвинуться вправо
    tm.AddRule("q0", 'a', "q1", 'b', Direction::RIGHT);
    tm.AddFinalState("q1");
    
    std::vector<char> input = {'a'};
    tm.SetInput(input);
    
    // Выполняем один шаг
    bool step_result = tm.Step();
    
    if (!step_result) return false;
    if (tm.GetCurrentState() != "q1") return false;
    if (tm.GetHeadPosition() != 1) return false;
    if (tm.GetSymbolAt(0) != 'b') return false;  // Символ должен быть заменён
    if (tm.GetStepCount() != 1) return false;
    
    return true;
}

/**
 * Тест бинарного инвертера
 */
bool TestBinaryInverter() {
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Настраиваем правила для инверсии
    tm.AddRule("START", '0', "START", '1', Direction::RIGHT);
    tm.AddRule("START", '1', "START", '0', Direction::RIGHT);
    tm.AddRule("START", ' ', "FINAL", ' ', Direction::STAY);
    tm.AddFinalState("FINAL");
    
    // Тестируем на короткой строке
    std::vector<char> input = {'1', '0', '1'};
    tm.SetInput(input);
    
    bool success = tm.Run();
    if (!success) return false;
    
    // Проверяем результат
    if (tm.GetSymbolAt(0) != '0') return false;  // 1 -> 0
    if (tm.GetSymbolAt(1) != '1') return false;  // 0 -> 1
    if (tm.GetSymbolAt(2) != '0') return false;  // 1 -> 0
    
    return tm.IsInFinalState();
}

/**
 * Тест остановки при отсутствии правил
 */
bool TestHaltOnNoRule() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    // Не добавляем никаких правил
    std::vector<char> input = {'a'};
    tm.SetInput(input);
    
    // Машина должна остановиться на первом шаге
    bool step_result = tm.Step();
    
    if (step_result) return false;  // Не должно быть успешным
    if (tm.GetStepCount() != 0) return false;
    
    return true;
}

/**
 * Тест ограничения по количеству шагов
 */
bool TestMaxStepsLimit() {
    // Создаём машину с лимитом в 5 шагов
    MachineTuring<std::string, char> tm(' ', "q0", 5);
    
    // Создаём бесконечный цикл
    tm.AddRule("q0", ' ', "q0", ' ', Direction::RIGHT);
    
    std::vector<char> input;
    tm.SetInput(input);
    
    bool success = tm.Run();
    
    // Машина должна остановиться через 5 шагов
    if (tm.GetStepCount() != 5) return false;
    if (success) return false;  // Не должна завершиться успешно
    
    return true;
}

/**
 * Тест сброса машины
 */
bool TestReset() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    tm.AddRule("q0", ' ', "q1", 'x', Direction::RIGHT);
    tm.AddFinalState("q1");
    
    std::vector<char> input;
    tm.SetInput(input);
    
    // Выполняем один шаг
    tm.Step();
    
    // Проверяем, что состояние изменилось
    if (tm.GetCurrentState() == "q0") return false;
    if (tm.GetStepCount() == 0) return false;
    
    // Сбрасываем
    tm.Reset();
    
    // Проверяем сброс
    if (tm.GetCurrentState() != "q0") return false;
    if (tm.GetHeadPosition() != 0) return false;
    if (tm.GetStepCount() != 0) return false;
    
    return true;
}

/**
 * Тест копирования
 */
bool TestCopyConstructor() {
    MachineTuring<std::string, char> tm1('_', "start");
    tm1.AddRule("start", 'a', "end", 'b', Direction::RIGHT);
    tm1.AddFinalState("end");
    
    std::vector<char> input = {'a'};
    tm1.SetInput(input);
    
    // Создаём копию
    MachineTuring<std::string, char> tm2(tm1);
    
    // Проверяем, что копия работает
    bool success = tm2.Run();
    if (!success) return false;
    if (tm2.GetCurrentState() != "end") return false;
    
    return true;
}

/**
 * Тест движения влево
 */
bool TestLeftMovement() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    tm.AddRule("q0", 'a', "q1", 'a', Direction::RIGHT);
    tm.AddRule("q1", 'b', "q2", 'b', Direction::LEFT);
    tm.AddRule("q2", 'a', "final", 'x', Direction::STAY);
    tm.AddFinalState("final");
    
    std::vector<char> input = {'a', 'b'};
    tm.SetInput(input);
    
    bool success = tm.Run();
    if (!success) return false;
    
    // Проверяем, что головка вернулась к первому символу
    if (tm.GetHeadPosition() != 0) return false;
    if (tm.GetSymbolAt(0) != 'x') return false;
    
    return true;
}

/**
 * Тест на пустом входе
 */
bool TestEmptyInput() {
    MachineTuring<std::string, char> tm(' ', "start");
    
    tm.AddRule("start", ' ', "final", ' ', Direction::STAY);
    tm.AddFinalState("final");
    
    std::vector<char> empty_input;
    tm.SetInput(empty_input);
    
    bool success = tm.Run();
    if (!success) return false;
    if (!tm.IsInFinalState()) return false;
    
    return true;
}

/**
 * Тест получения сегмента ленты
 */
bool TestTapeSegment() {
    MachineTuring<std::string, char> tm('_', "q0");
    
    std::vector<char> input = {'a', 'b', 'c', 'd', 'e'};
    tm.SetInput(input);
    
    auto segment = tm.GetTapeSegment(1, 3);  // Получаем 3 символа начиная с позиции 1
    
    if (segment.size() != 3) return false;
    if (segment[0] != 'b') return false;
    if (segment[1] != 'c') return false;
    if (segment[2] != 'd') return false;
    
    return true;
}

/**
 * Основная функция для запуска всех тестов
 */
int main() {
    std::cout << "Модульные тесты машины Тьюринга" << std::endl;
    std::cout << "===================================" << std::endl;
    
    // Запускаем все тесты
    TestFramework::RunTest("Базовый функционал", TestBasicFunctionality);
    TestFramework::RunTest("Установка входа", TestInputSetting);
    TestFramework::RunTest("Простой переход", TestSimpleTransition);
    TestFramework::RunTest("Бинарный инвертер", TestBinaryInverter);
    TestFramework::RunTest("Остановка без правил", TestHaltOnNoRule);
    TestFramework::RunTest("Лимит шагов", TestMaxStepsLimit);
    TestFramework::RunTest("Сброс машины", TestReset);
    TestFramework::RunTest("Копирование", TestCopyConstructor);
    TestFramework::RunTest("Движение влево", TestLeftMovement);
    TestFramework::RunTest("Пустой вход", TestEmptyInput);
    TestFramework::RunTest("Сегмент ленты", TestTapeSegment);
    
    TestFramework::PrintSummary();
    
    return 0;
}
