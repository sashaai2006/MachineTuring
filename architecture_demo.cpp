#include "MT.h"
#include "TuringEntities.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * Демонстрация архитектуры с разделением ответственности
 * Показывает, как каждый компонент имеет свою специфичную зону ответственности
 */
void DemonstrateArchitecture() {
    std::cout << "🏠 === Демонстрация архитектуры с разделением ответственности ===" << std::endl << std::endl;
    
    std::cout << "🔧 Создаём машину Тьюринга с компонентной архитектурой:" << std::endl;
    MachineTuring<std::string, char> tm(' ', "INIT");
    
    std::cout << std::endl << "🔍 1. TransitionManager - управление правилами переходов:" << std::endl;
    tm.AddRule("INIT", 'A', "PROCESS", 'X', Direction::RIGHT);
    tm.AddRule("PROCESS", 'B', "PROCESS", 'Y', Direction::RIGHT);
    tm.AddRule("PROCESS", ' ', "FINISH", 'Z', Direction::STAY);
    std::cout << "   - Добавлено 3 правила перехода" << std::endl;
    
    std::cout << std::endl << "📍 2. StateManager - управление состояниями:" << std::endl;
    tm.AddFinalState("FINISH");
    std::cout << "   - Начальное состояние: " << tm.GetCurrentState() << std::endl;
    std::cout << "   - Добавлено конечное состояние: FINISH" << std::endl;
    
    std::cout << std::endl << "📜 3. TuringTape (LazySeq) - бесконечная лента:" << std::endl;
    std::vector<char> input = {'A', 'B', 'B', 'A'};
    tm.SetInput(input);
    std::cout << "   - Начальные данные установлены на ленту через LazySeq" << std::endl;
    std::cout << "   - Пустой символ: '" << tm.GetBlankSymbol() << "'" << std::endl;
    
    std::cout << std::endl << "🔄 4. HeadManager - управление головкой:" << std::endl;
    std::cout << "   - Начальная позиция: " << tm.GetHeadPosition() << std::endl;
    std::cout << "   - Символ под головкой: '" << tm.ReadCurrentSymbol() << "'" << std::endl;
    
    std::cout << std::endl << "📊 Начальное состояние машины:" << std::endl;
    tm.PrintState(std::cout, 10);
    
    std::cout << std::endl << "⚙️ Запускаем выполнение с отладкой:" << std::endl;
    tm.SetDebugMode(true);
    
    bool success = tm.Run();
    
    std::cout << std::endl << "📈 Итоговая статистика:" << std::endl;
    tm.PrintStatistics();
    
    if (success) {
        std::cout << std::endl << "🎉 Машина завершила работу успешно!" << std::endl;
        std::cout << "Итоговое состояние ленты:" << std::endl;
        tm.PrintState(std::cout, 8);
    }
    
    std::cout << std::endl;
}

/**
 * Демонстрация эффективности LazySeq на отдалённых позициях
 */
void DemonstrateLazySeqEfficiency() {
    std::cout << "🚀 === Эффективность LazySeq: обращение к миллионным позициям ===" << std::endl << std::endl;
    
    MachineTuring<std::string, char> tm('.', "JUMP");
    
    // Простая машина, которая прыгает на отдалённые позиции
    tm.AddRule("JUMP", '.', "FINAL", 'V', Direction::STAY);
    tm.AddFinalState("FINAL");
    
    // Маленькие начальные данные
    std::vector<char> input = {'S', 'M', 'A', 'L', 'L'};
    tm.SetInput(input);
    
    std::cout << "📋 Начальные данные:" << std::endl;
    std::cout << "   Позиции 0-4: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << tm.GetSymbolAt(i) << " ";
    }
    std::cout << std::endl;
    
    std::cout << std::endl << "🌌 Проверяем доступ к отдалённым позициям через LazySeq:" << std::endl;
    
    // Проверяем отдалённые позиции - LazySeq генерирует их лениво
    std::vector<int> distant_positions = {100, 1000, 10000, 100000, 1000000};
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int pos : distant_positions) {
        char symbol = tm.GetSymbolAt(pos);
        std::cout << "   Позиция " << pos << ": '" << symbol << "' (LazySeq генерирует по запросу)" << std::endl;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto access_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    std::cout << "   ✅ Время доступа к отдалённым позициям: " << access_time.count() << " мкс" << std::endl;
    
    std::cout << std::endl << "🐈 Проверяем отрицательные позиции:" << std::endl;
    for (int pos = -1; pos >= -5; --pos) {
        char symbol = tm.GetSymbolAt(pos);
        std::cout << "   Позиция " << pos << ": '" << symbol << "' (левая часть ленты)" << std::endl;
    }
    
    std::cout << std::endl << "🏃 Запускаем машину:" << std::endl;
    bool success = tm.Run();
    
    if (success) {
        std::cout << "✅ Машина завершила работу!" << std::endl;
        tm.PrintStatistics();
    }
    
    std::cout << std::endl;
}

/**
 * Демонстрация модификаций на LazySeq
 */
void DemonstrateLazySeqModifications() {
    std::cout << "🔍 === Модификации ленты через LazySeq ===" << std::endl << std::endl;
    
    MachineTuring<std::string, char> tm('-', "MODIFY");
    
    // Машина, которая модифицирует символы на ленте
    tm.AddRule("MODIFY", 'A', "MODIFY", '1', Direction::RIGHT);
    tm.AddRule("MODIFY", 'B', "MODIFY", '2', Direction::RIGHT);
    tm.AddRule("MODIFY", 'C', "MODIFY", '3', Direction::RIGHT);
    tm.AddRule("MODIFY", '-', "JUMP_BACK", '*', Direction::LEFT);  // Остановимся и пойдём назад
    
    // Возвращаемся в начало
    tm.AddRule("JUMP_BACK", '1', "JUMP_BACK", '1', Direction::LEFT);
    tm.AddRule("JUMP_BACK", '2', "JUMP_BACK", '2', Direction::LEFT);
    tm.AddRule("JUMP_BACK", '3', "JUMP_BACK", '3', Direction::LEFT);
    tm.AddRule("JUMP_BACK", '-', "FINAL", '!', Direction::STAY);
    
    tm.AddFinalState("FINAL");
    
    std::vector<char> input = {'A', 'B', 'C', 'A', 'B'};
    tm.SetInput(input);
    
    std::cout << "📋 Начальные данные на ленте: ";
    for (char c : input) std::cout << c << " ";
    std::cout << std::endl;
    
    std::cout << std::endl << "🔄 Показываем, как LazySeq обрабатывает модификации:" << std::endl;
    
    bool success = tm.Run();
    
    if (success) {
        std::cout << std::endl << "🎉 Модификации завершены!" << std::endl;
        std::cout << "Итоговое состояние ленты:" << std::endl;
        tm.PrintState();
        
        std::cout << std::endl << "🔍 Модифицированные данные на ленте: ";
        auto result = tm.GetTapeSegment(0, input.size() + 1);
        for (char c : result) std::cout << c << " ";
        std::cout << std::endl;
        
        tm.PrintStatistics();
    }
    
    std::cout << std::endl;
}

/**
 * Главная функция
 */
int main() {
    std::cout << "🏠 Демонстрация архитектуры машины Тьюринга" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "📅 Основывается на LazySequence с разделением ответственности" << std::endl;
    std::cout << std::endl;
    
    try {
        DemonstrateArchitecture();
        DemonstrateLazySeqEfficiency();
        DemonstrateLazySeqModifications();
        
        std::cout << "📚 === Ключевые принципы архитектуры ===" << std::endl;
        std::cout << std::endl;
        std::cout << "1. 🤝 Разделение ответственности (Separation of Concerns):" << std::endl;
        std::cout << "   • TransitionManager: только правила переходов" << std::endl;
        std::cout << "   • StateManager: только управление состояниями" << std::endl;
        std::cout << "   • TuringTape: только работа с лентой (на основе LazySeq)" << std::endl;
        std::cout << "   • HeadManager: только управление головкой" << std::endl;
        std::cout << std::endl;
        
        std::cout << "2. 🚀 Оптимизация через LazySeq:" << std::endl;
        std::cout << "   • Ленивая генерация символов на ленте" << std::endl;
        std::cout << "   • Мемоизация ранее вычисленных элементов" << std::endl;
        std::cout << "   • Оптимальное использование памяти" << std::endl;
        std::cout << std::endl;
        
        std::cout << "3. ⚙️ Модульность:" << std::endl;
        std::cout << "   • Каждый компонент можно тестировать отдельно" << std::endl;
        std::cout << "   • Легкое расширение функциональности" << std::endl;
        std::cout << "   • Повторное использование компонентов" << std::endl;
        
        std::cout << std::endl << "✨ Все демонстрации завершены успешно!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
