#pragma once

#include "TuringEntities.h"
#include "LazySeq.h"
#include "Gen.h"
#include "Mem.h"

#include <iostream>
#include <stdexcept>
#include <chrono>

/**
 * Машина Тьюринга - комплексная абстрактная вычислительная модель
 * 
 * Основана на LazySequence для эффективного моделирования бесконечной ленты.
 * Использует четкое разделение ответственности между компонентами.
 * 
 * Компоненты:
 * - TransitionManager: Управление правилами переходов
 * - StateManager: Управление состояниями
 * - TuringTape: Лента на основе LazySeq
 * - HeadManager: Управление головкой чтения/записи
 */
template <typename State, typename Symbol>
class MachineTuring {
public:
    using Rule = TransitionRule<State, Symbol>;
    
private:
    // Компоненты машины (четкое разделение ответственности)
    TransitionManager<State, Symbol> transitions_;    // Правила переходов
    StateManager<State> states_;                     // Управление состояниями
    TuringTape<Symbol> tape_;                        // Лента на основе LazySeq
    HeadManager head_;                               // Управление головкой
    
    // Конфигурация выполнения
    size_t step_count_;           // Количество выполненных шагов
    size_t max_steps_;            // Максимальное количество шагов
    bool debug_mode_;             // Режим отладки
    
    // Статистика выполнения
    std::chrono::milliseconds execution_time_;  // Время выполнения
    
public:
    /**
     * Конструктор машины Тьюринга
     * @param blank_symbol Пустой символ для заполнения ленты
     * @param initial_state Начальное состояние
     * @param max_steps Максимальное количество шагов (защита от зацикливания)
     * @param initial_head_pos Начальная позиция головки
     */
    explicit MachineTuring(const Symbol& blank_symbol, 
                          const State& initial_state,
                          size_t max_steps = 100000,
                          int initial_head_pos = 0)
        : transitions_(),
          states_(initial_state),
          tape_(blank_symbol),
          head_(initial_head_pos),
          step_count_(0),
          max_steps_(max_steps),
          debug_mode_(false),
          execution_time_(0) {}
    
    /**
     * Копирующий конструктор
     */
    MachineTuring(const MachineTuring& other)
        : transitions_(other.transitions_),
          states_(other.states_),
          tape_(other.tape_),
          head_(other.head_),
          step_count_(other.step_count_),
          max_steps_(other.max_steps_),
          debug_mode_(other.debug_mode_),
          execution_time_(other.execution_time_) {}
    
    /**
     * Оператор присваивания
     */
    MachineTuring& operator=(const MachineTuring& other) {
        if (this != &other) {
            transitions_ = other.transitions_;
            states_ = other.states_;
            tape_ = other.tape_;
            head_ = other.head_;
            step_count_ = other.step_count_;
            max_steps_ = other.max_steps_;
            debug_mode_ = other.debug_mode_;
            execution_time_ = other.execution_time_;
        }
        return *this;
    }
    
    // =================
    // Настройка машины
    // =================
    
    /**
     * Добавить правило перехода
     */
    void AddRule(const Rule& rule) {
        transitions_.AddRule(rule);
    }
    
    void AddRule(const State& from_state, const Symbol& read_symbol,
                 const State& to_state, const Symbol& write_symbol,
                 Direction direction) {
        transitions_.AddRule(from_state, read_symbol, to_state, write_symbol, direction);
    }
    
    /**
     * Добавить конечное состояние
     */
    void AddFinalState(const State& state) {
        states_.AddFinalState(state);
    }
    
    /**
     * Установить начальные данные на ленте
     * @param input Входные данные
     * @param start_position Позиция для размещения данных
     */
    void SetInput(const std::vector<Symbol>& input, int start_position = 0) {
        // Очищаем ленту и устанавливаем новые данные
        tape_.Reset(input);
        
        // Устанавливаем позицию головки
        head_.SetPosition(start_position);
        
        // Сбрасываем состояние и счётчики
        states_.Reset();
        step_count_ = 0;
        execution_time_ = std::chrono::milliseconds(0);
    }
    
    /**
     * Включить/выключить режим отладки
     */
    void SetDebugMode(bool enable) {
        debug_mode_ = enable;
    }
    
    // =================
    // Выполнение
    // =================
    
    /**
     * Выполнить один шаг вычисления
     * @return true, если шаг выполнен; false, если машина остановилась
     */
    bool Step() {
        // Проверяем условия остановки
        if (states_.IsInFinalState() || step_count_ >= max_steps_) {
            return false;
        }
        
        // Читаем текущий символ
        Symbol current_symbol = tape_.GetSymbolAt(head_.GetPosition());
        
        // Ищем применимое правило
        auto rule_opt = transitions_.FindRule(states_.GetCurrentState(), current_symbol);
        
        if (!rule_opt) {
            // Нет применимого правила - остановка
            return false;
        }
        
        const Rule& rule = *rule_opt;
        
        // Отладочный вывод перед применением правила
        if (debug_mode_) {
            PrintDebugStep(rule);
        }
        
        // Применяем правило:
        // 1. Записываем новый символ
        tape_.SetSymbolAt(head_.GetPosition(), rule.write_symbol);
        
        // 2. Переходим в новое состояние
        states_.SetCurrentState(rule.to_state);
        
        // 3. Двигаем головку
        head_.Move(rule.direction);
        
        // Увеличиваем счётчик шагов
        step_count_++;
        
        return true;
    }
    
    /**
     * Выполнить машину до остановки
     * @return true, если машина завершилась в конечном состоянии
     */
    bool Run() {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Выполняем шаги до остановки
        while (Step()) {
            // Продолжаем выполнение
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        execution_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return states_.IsInFinalState();
    }
    
    // =================
    // Инспекция состояния
    // =================
    
    /**
     * Получить текущее состояние
     */
    const State& GetCurrentState() const {
        return states_.GetCurrentState();
    }
    
    /**
     * Получить текущую позицию головки
     */
    int GetHeadPosition() const {
        return head_.GetPosition();
    }
    
    /**
     * Проверить, находится ли машина в конечном состоянии
     */
    bool IsInFinalState() const {
        return states_.IsInFinalState();
    }
    
    /**
     * Получить количество выполненных шагов
     */
    size_t GetStepCount() const {
        return step_count_;
    }
    
    /**
     * Получить максимальное количество шагов
     */
    size_t GetMaxSteps() const {
        return max_steps_;
    }
    
    /**
     * Получить время выполнения
     */
    std::chrono::milliseconds GetExecutionTime() const {
        return execution_time_;
    }
    
    // =================
    // Работа с лентой
    // =================
    
    /**
     * Получить символ по позиции
     */
    Symbol GetSymbolAt(int position) const {
        return tape_.GetSymbolAt(position);
    }
    
    /**
     * Получить символ в текущей позиции головки
     */
    Symbol ReadCurrentSymbol() const {
        return tape_.GetSymbolAt(head_.GetPosition());
    }
    
    /**
     * Получить сегмент ленты
     */
    std::vector<Symbol> GetTapeSegment(int start_position, size_t length) const {
        return tape_.GetSegment(start_position, length);
    }
    
    /**
     * Получить пустой символ
     */
    const Symbol& GetBlankSymbol() const {
        return tape_.GetBlankSymbol();
    }
    
    // =================
    // Управление
    // =================
    
    /**
     * Сбросить машину в начальное состояние
     */
    void Reset() {
        states_.Reset();
        head_.Reset();
        tape_.Reset();
        step_count_ = 0;
        execution_time_ = std::chrono::milliseconds(0);
    }
    
    // =================
    // Отладка и визуализация
    // =================
    
    /**
     * Вывести текущее состояние машины
     */
    void PrintState(std::ostream& out = std::cout, int tape_window = 20) const {
        out << "Шаг: " << step_count_ << ", ";
        out << "Состояние: " << states_.GetCurrentState() << ", ";
        out << "Позиция: " << head_.GetPosition() << std::endl;
        
        // Выводим участок ленты вокруг головки
        int start = head_.GetPosition() - tape_window / 2;
        auto segment = tape_.GetSegment(start, tape_window);
        
        out << "Лента: ";
        for (size_t i = 0; i < segment.size(); ++i) {
            int pos = start + static_cast<int>(i);
            if (pos == head_.GetPosition()) {
                out << "[" << segment[i] << "]";
            } else {
                out << " " << segment[i] << " ";
            }
        }
        out << std::endl;
    }
    
    /**
     * Вывести статистику выполнения
     */
    void PrintStatistics(std::ostream& out = std::cout) const {
        out << "=== Статистика выполнения ===" << std::endl;
        out << "Шагов выполнено: " << step_count_ << std::endl;
        out << "Максимально шагов: " << max_steps_ << std::endl;
        out << "Время выполнения: " << execution_time_.count() << " мс" << std::endl;
        out << "Конечное состояние: " << (IsInFinalState() ? "Да" : "Нет") << std::endl;
        out << "Материализовано ячеек ленты: " << tape_.GetMaterializedCount() << std::endl;
        out << "Модификаций ленты: " << tape_.GetModificationsCount() << std::endl;
    }
    
private:
    /**
     * Отладочный вывод шага
     */
    void PrintDebugStep(const Rule& rule) const {
        std::cout << "[Отладка] Применяем правило: ("
                  << rule.from_state << ", " << rule.read_symbol << ") -> ("
                  << rule.to_state << ", " << rule.write_symbol << ", ";
        
        switch (rule.direction) {
            case Direction::LEFT: std::cout << "LEFT"; break;
            case Direction::STAY: std::cout << "STAY"; break;
            case Direction::RIGHT: std::cout << "RIGHT"; break;
        }
        
        std::cout << ")" << std::endl;
        PrintState(std::cout, 10);
        std::cout << std::endl;
    }
};
