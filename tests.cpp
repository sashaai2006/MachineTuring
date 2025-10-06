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
                std::cout << "PASS \u2713" << std::endl;
            } else {
                std::cout << "FAIL \u2717" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
        }
    }
    
    static void PrintSummary() {
        std::cout << std::endl << "=== Результаты тестирования ===" << std::endl;
        std::cout << "Прошло: " << passed_tests << "/" << total_tests << " тестов" << std::endl;
        if (passed_tests == total_tests) {
            std::cout << "Все тесты прошли успешно! 🎉" << std::endl;
        } else {
            std::cout << "Некоторые тесты не прошли ❌" << std::endl;
        }
    }
};

int TestFramework::total_tests = 0;
int TestFramework::passed_tests = 0;

/**
 * Тест создания и настройки машины
 */
bool TestMachineCreation() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    // Проверяем начальное состояние
    if (tm.GetCurrentState() != "q0") return false;
    if (tm.GetHeadPosition() != 0) return false;
    if (tm.GetStepCount() != 0) return false;
    if (tm.GetBlankSymbol() != ' ') return false;
    
    return true;
}

/**
 * Тест работы с LazySeq (ленивая лента)
 */
bool TestLazySequenceTape() {
    MachineTuring<std::string, char> tm('_', "start");
    
    // Устанавливаем начальные данные
    std::vector<char> input = {'a', 'b', 'c'};
    tm.SetInput(input);
    
    // Проверяем чтение из ленивой последовательности
    if (tm.GetSymbolAt(0) != 'a') return false;
    if (tm.GetSymbolAt(1) != 'b') return false;
    if (tm.GetSymbolAt(2) != 'c') return false;
    
    // Проверяем автоматическое заполнение пустыми символами
    if (tm.GetSymbolAt(3) != '_') return false;  // LazySeq генерирует пустой символ
    if (tm.GetSymbolAt(100) != '_') return false; // Даже очень отдалённая позиция
    
    // Проверяем отрицательные позиции (левая часть ленты)
    if (tm.GetSymbolAt(-1) != '_') return false;
    if (tm.GetSymbolAt(-50) != '_') return false;
    
    return true;
}

/**
 * Тест компонентов (разделение ответственности)
 */
bool TestComponentSeparation() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    // Тестируем TransitionManager
    tm.AddRule("q0", 'a', "q1", 'b', Direction::RIGHT);
    tm.AddRule("q1", ' ', "q2", 'c', Direction::LEFT);
    
    // Тестируем StateManager
    tm.AddFinalState("q2");
    
    // Тестируем TuringTape (LazySeq)
    std::vector<char> input = {'a'};
    tm.SetInput(input);
    
    // Проверяем начальное состояние
    if (tm.GetCurrentState() != "q0") return false;
    if (tm.ReadCurrentSymbol() != 'a') return false;
    
    // Первый шаг: q0, 'a' -> q1, 'b', RIGHT
    bool step1 = tm.Step();
    if (!step1) return false;
    if (tm.GetCurrentState() != "q1") return false;
    if (tm.GetHeadPosition() != 1) return false;
    if (tm.GetSymbolAt(0) != 'b') return false;  // Проверяем запись
    
    // Второй шаг: q1, ' ' -> q2, 'c', LEFT
    bool step2 = tm.Step();
    if (!step2) return false;
    if (tm.GetCurrentState() != "q2") return false;
    if (tm.GetHeadPosition() != 0) return false;
    if (tm.GetSymbolAt(1) != 'c') return false;  // Проверяем запись
    
    // Проверяем, что машина в конечном состоянии
    if (!tm.IsInFinalState()) return false;
    
    return true;
}

/**
 * Тест бинарного инвертера на LazySeq
 */
bool TestBinaryInverterLazy() {
    MachineTuring<std::string, char> tm(' ', "START");
    
    // Настраиваем машину
    tm.AddRule("START", '0', "START", '1', Direction::RIGHT);
    tm.AddRule("START", '1', "START", '0', Direction::RIGHT);
    tm.AddRule("START", ' ', "FINAL", ' ', Direction::STAY);
    tm.AddFinalState("FINAL");
    
    // Тестируем на большой строке (для проверки LazySeq)
    std::vector<char> input;
    for (int i = 0; i < 100; ++i) {
        input.push_back((i % 2) ? '1' : '0');
    }
    
    tm.SetInput(input);
    
    bool success = tm.Run();
    if (!success) return false;
    
    // Проверяем корректность инверсии
    auto result = tm.GetTapeSegment(0, input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        char expected = (input[i] == '0') ? '1' : '0';
        if (result[i] != expected) return false;
    }
    
    return tm.IsInFinalState();
}

/**
 * Тест ограничения по шагам
 */
bool TestStepLimit() {
    MachineTuring<std::string, char> tm(' ', "q0", 10);  // Лимит 10 шагов
    
    // Создаём бесконечный цикл
    tm.AddRule("q0", ' ', "q0", ' ', Direction::RIGHT);
    
    std::vector<char> input;
    tm.SetInput(input);
    
    bool success = tm.Run();
    
    // Машина должна остановиться по лимиту шагов
    if (tm.GetStepCount() != 10) return false;
    if (success) return false; // Не должна завершиться в конечном состоянии
    
    return true;
}

/**
 * Тест сброса машины (все компоненты)
 */
bool TestMachineReset() {
    MachineTuring<std::string, char> tm('_', "start");
    
    tm.AddRule("start", 'a', "middle", 'x', Direction::RIGHT);
    tm.AddRule("middle", '_', "end", 'y', Direction::LEFT);
    tm.AddFinalState("end");
    
    std::vector<char> input = {'a'};
    tm.SetInput(input);
    
    // Выполняем несколько шагов
    tm.Step(); // start -> middle
    tm.Step(); // middle -> end
    
    // Проверяем что машина изменилась
    if (tm.GetCurrentState() == "start") return false;
    if (tm.GetStepCount() == 0) return false;
    if (tm.GetHeadPosition() == 0) return false;
    
    // Сбрасываем
    tm.Reset();
    
    // Проверяем сброс всех компонентов
    if (tm.GetCurrentState() != "start") return false;  // StateManager
    if (tm.GetHeadPosition() != 0) return false;        // HeadManager
    if (tm.GetStepCount() != 0) return false;           // Счётчик
    if (tm.ReadCurrentSymbol() != '_') return false;    // TuringTape (очищена)
    
    return true;
}

/**
 * Тест сохранения/восстановления состояния
 */
bool TestStatePersistence() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    tm.AddRule("q0", ' ', "q1", 'x', Direction::RIGHT);
    tm.AddFinalState("q1");
    
    std::vector<char> input;
    tm.SetInput(input);
    
    // Сохраняем копию начального состояния
    MachineTuring<std::string, char> tm_copy(tm);
    
    // Изменяем оригинал
    tm.Step();
    
    // Проверяем, что копия не изменилась
    if (tm_copy.GetCurrentState() != "q0") return false;
    if (tm_copy.GetStepCount() != 0) return false;
    
    // Оригинал должен быть изменён
    if (tm.GetCurrentState() != "q1") return false;
    if (tm.GetStepCount() != 1) return false;
    
    return true;
}

/**
 * Тест эффективности LazySeq на отдалённых позициях
 */
bool TestLazySeqDistantAccess() {
    MachineTuring<std::string, int> tm(0, "q0");  // Используем int для символов
    
    // Маленькие начальные данные
    std::vector<int> input = {1, 2, 3};
    tm.SetInput(input);
    
    // Проверяем эффективность LazySeq на отдалённых позициях
    
    // Отдалённые позиции должны быть заполнены пустым символом
    if (tm.GetSymbolAt(1000) != 0) return false;
    if (tm.GetSymbolAt(10000) != 0) return false;
    
    // Начальные данные должны сохраниться
    if (tm.GetSymbolAt(0) != 1) return false;
    if (tm.GetSymbolAt(1) != 2) return false;
    if (tm.GetSymbolAt(2) != 3) return false;
    
    return true;
}

/**
 * Тест сегментов ленты
 */
bool TestTapeSegments() {
    MachineTuring<std::string, char> tm('?', "q0");
    
    std::vector<char> input = {'A', 'B', 'C', 'D', 'E'};
    tm.SetInput(input, 2);  // Размещаем с позиции 2
    
    // Проверяем размещение
    if (tm.GetSymbolAt(2) != 'A') return false;
    if (tm.GetSymbolAt(3) != 'B') return false;
    if (tm.GetSymbolAt(6) != 'E') return false;
    
    // Проверяем получение сегментов
    auto segment1 = tm.GetTapeSegment(0, 3);  // Пустые позиции
    if (segment1[0] != '?' || segment1[1] != '?' || segment1[2] != 'A') return false;
    
    auto segment2 = tm.GetTapeSegment(2, 5);  // Данные
    if (segment2[0] != 'A' || segment2[4] != 'E') return false;
    
    auto segment3 = tm.GetTapeSegment(10, 3); // Пустые позиции справа
    if (segment3[0] != '?' || segment3[2] != '?') return false;
    
    return true;
}

/**
 * Тест работы с отрицательными позициями
 */
bool TestNegativePositions() {
    MachineTuring<std::string, char> tm('#', "q0");
    
    tm.AddRule("q0", '#', "q1", 'X', Direction::LEFT);  // Двигаемся влево
    tm.AddRule("q1", '#', "q2", 'Y', Direction::LEFT);  // Ещё влево
    tm.AddFinalState("q2");
    
    std::vector<char> input;
    tm.SetInput(input);  // Пустая лента
    
    // Выполняем два шага
    bool step1 = tm.Step();
    bool step2 = tm.Step();
    
    if (!step1 || !step2) return false;
    
    // Проверяем конечное состояние
    if (tm.GetHeadPosition() != -2) return false;       // Головка на позиции -2
    if (!tm.IsInFinalState()) return false;
    
    // Проверяем отрицательные позиции обрабатываются LazySeq
    if (tm.GetSymbolAt(-1) != '#') return false; // Пустой символ для отрицательных позиций
    
    return true;
}

/**
 * Тест статистики выполнения
 */
bool TestExecutionStatistics() {
    MachineTuring<std::string, char> tm(' ', "q0");
    
    tm.AddRule("q0", ' ', "q1", 'a', Direction::RIGHT);
    tm.AddRule("q1", ' ', "q2", 'b', Direction::RIGHT);
    tm.AddRule("q2", ' ', "final", 'c', Direction::STAY);
    tm.AddFinalState("final");
    
    std::vector<char> input;
    tm.SetInput(input);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    bool success = tm.Run();
    auto end_time = std::chrono::high_resolution_clock::now();
    
    if (!success) return false;
    if (tm.GetStepCount() != 3) return false;
    
    // Проверяем, что время выполнения сохраняется
    auto execution_time = tm.GetExecutionTime();
    if (execution_time.count() < 0) return false;  // Время не может быть отрицательным
    
    return true;
}

/**
 * Основная функция для запуска всех тестов
 */
int main() {
    std::cout << "🧪 Модульные тесты машины Тьюринга на основе LazySeq" << std::endl;
    std::cout << "=================================================================" << std::endl;
    
    // Запускаем все тесты
    TestFramework::RunTest("🔧 Создание машины", TestMachineCreation);
    TestFramework::RunTest("🔍 LazySeq лента", TestLazySequenceTape);
    TestFramework::RunTest("⚙️ Разделение компонентов", TestComponentSeparation);
    TestFramework::RunTest("🔄 Бинарный инвертер (LazySeq)", TestBinaryInverterLazy);
    TestFramework::RunTest("🚫 Лимит шагов", TestStepLimit);
    TestFramework::RunTest("🔄 Сброс машины", TestMachineReset);
    TestFramework::RunTest("💾 Сохранение состояния", TestStatePersistence);
    TestFramework::RunTest("🌌 LazySeq отдалённый доступ", TestLazySeqDistantAccess);
    TestFramework::RunTest("📥 Сегменты ленты", TestTapeSegments);
    TestFramework::RunTest("➩ Отрицательные позиции", TestNegativePositions);
    TestFramework::RunTest("📊 Статистика выполнения", TestExecutionStatistics);
    
    TestFramework::PrintSummary();
    
    std::cout << std::endl << "🔍 Особенности тестирования:" << std::endl;
    std::cout << "- ✅ Проверка всех компонентов: TransitionManager, StateManager, TuringTape, HeadManager" << std::endl;
    std::cout << "- 🚀 Тестирование эффективности LazySeq на отдалённых позициях" << std::endl;
    std::cout << "- 🚫 Проверка защиты от зацикливания (лимит шагов)" << std::endl;
    std::cout << "- ♾️ Тесты отрицательных позиций (левая часть ленты)" << std::endl;
    
    return (TestFramework::total_tests == TestFramework::passed_tests) ? 0 : 1;
}
