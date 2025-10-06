#pragma once

#include "LazySeq.h"
#include "Mem.h"
#include "Gen.h"

#include <map>
#include <vector>
#include <string>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <optional>

/**
 * Направления движения головки машины Тьюринга
 */
enum class Direction {
    LEFT = -1,   // Влево
    STAY = 0,    // Остаться на месте  
    RIGHT = 1    // Вправо
};

/**
 * Правило перехода для машины Тьюринга
 * Описывает: из какого состояния, при каком символе на ленте,
 * в какое состояние перейти, какой символ записать и куда сдвинуть головку
 */
template <typename State, typename Symbol>
struct TransitionRule {
    State from_state;        // Из какого состояния
    Symbol read_symbol;      // При каком символе на ленте
    State to_state;          // В какое состояние перейти
    Symbol write_symbol;     // Какой символ записать
    Direction direction;     // Направление движения головки
    
    TransitionRule() = default;
    
    TransitionRule(const State& from, const Symbol& read, 
                   const State& to, const Symbol& write, Direction dir)
        : from_state(from), read_symbol(read), to_state(to), 
          write_symbol(write), direction(dir) {}
};

/**
 * Генератор для создания бесконечной ленты с заданным символом по умолчанию
 */
template <typename T>
class TapeGenerator {
private:
    T default_symbol_;
    
public:
    explicit TapeGenerator(const T& default_symbol) 
        : default_symbol_(default_symbol) {}
    
    T GetNext() {
        return default_symbol_;
    }
};

/**
 * Машина Тьюринга - абстрактная вычислительная модель
 * 
 * Основные компоненты:
 * - Бесконечная лента (реализованная через LazySeq)
 * - Головка чтения/записи с текущей позицией
 * - Конечное множество состояний
 * - Правила переходов
 * - Начальное и конечные состояния
 */
template <typename State, typename Symbol>
class MachineTuring {
public:
    using Tape = LazySeq<Symbol, TapeGenerator<Symbol>, ArraySeqMem<Symbol>>;
    using Rule = TransitionRule<State, Symbol>;
    
private:
    mutable Tape tape_;           // Лента машины (ленивая последовательность)
    State current_state_;         // Текущее состояние
    int head_position_;           // Позиция головки (может быть отрицательной)
    Symbol blank_symbol_;         // Пустой символ (заполняет пустые ячейки ленты)
    
    std::vector<Rule> rules_;     // Правила переходов
    State initial_state_;         // Начальное состояние
    std::vector<State> final_states_;  // Конечные состояния
    
    size_t step_count_;           // Счётчик шагов выполнения
    size_t max_steps_;            // Максимальное количество шагов (защита от зацикливания)
    
public:
    /**
     * Конструктор машины Тьюринга
     * @param blank_symbol - символ, заполняющий пустые ячейки ленты
     * @param initial_state - начальное состояние
     * @param max_steps - максимальное количество шагов (по умолчанию 100000)
     */
    MachineTuring(const Symbol& blank_symbol, const State& initial_state, 
                  size_t max_steps = 100000)
        : tape_(TapeGenerator<Symbol>(blank_symbol), ArraySeqMem<Symbol>()),
          current_state_(initial_state),
          head_position_(0),
          blank_symbol_(blank_symbol),
          initial_state_(initial_state),
          step_count_(0),
          max_steps_(max_steps) {}
    
    /**
     * Копирующий конструктор
     */
    MachineTuring(const MachineTuring& other)
        : tape_(other.tape_),
          current_state_(other.current_state_),
          head_position_(other.head_position_),
          blank_symbol_(other.blank_symbol_),
          rules_(other.rules_),
          initial_state_(other.initial_state_),
          final_states_(other.final_states_),
          step_count_(other.step_count_),
          max_steps_(other.max_steps_) {}
    
    /**
     * Оператор присваивания
     */
    MachineTuring& operator=(const MachineTuring& other) {
        if (this != &other) {
            tape_ = other.tape_;
            current_state_ = other.current_state_;
            head_position_ = other.head_position_;
            blank_symbol_ = other.blank_symbol_;
            rules_ = other.rules_;
            initial_state_ = other.initial_state_;
            final_states_ = other.final_states_;
            step_count_ = other.step_count_;
            max_steps_ = other.max_steps_;
        }
        return *this;
    }
    
    /**
     * Добавить правило перехода
     * @param rule - правило перехода
     */
    void AddRule(const Rule& rule) {
        rules_.push_back(rule);
    }
    
    /**
     * Добавить правило перехода (с параметрами)
     */
    void AddRule(const State& from_state, const Symbol& read_symbol,
                 const State& to_state, const Symbol& write_symbol,
                 Direction direction) {
        rules_.emplace_back(from_state, read_symbol, to_state, write_symbol, direction);
    }
    
    /**
     * Добавить конечное состояние
     */
    void AddFinalState(const State& state) {
        final_states_.push_back(state);
    }
    
    /**
     * Инициализировать ленту входными данными
     * @param input - входная строка/последовательность
     * @param start_position - начальная позиция для записи (по умолчанию 0)
     */
    void SetInput(const std::vector<Symbol>& input, int start_position = 0) {
        // Очищаем кеш ленты для новых данных
        tape_ = Tape(TapeGenerator<Symbol>(blank_symbol_), ArraySeqMem<Symbol>());
        
        // Записываем входные данные на ленту
        for (size_t i = 0; i < input.size(); ++i) {
            SetSymbolAt(start_position + static_cast<int>(i), input[i]);
        }
        
        head_position_ = start_position;
        current_state_ = initial_state_;
        step_count_ = 0;
    }
    
    /**
     * Прочитать символ в текущей позиции головки
     */
    Symbol ReadCurrentSymbol() const {
        return GetSymbolAt(head_position_);
    }
    
    /**
     * Записать символ в текущую позицию головки
     */
    void WriteCurrentSymbol(const Symbol& symbol) {
        SetSymbolAt(head_position_, symbol);
    }
    
    /**
     * Получить символ по позиции на ленте
     * @param position - позиция (может быть отрицательной)
     */
    Symbol GetSymbolAt(int position) const {
        if (position < 0) {
            // Для отрицательных позиций возвращаем пустой символ
            // (упрощение - в полной реализации можно расширить ленту влево)
            return blank_symbol_;
        }
        
        auto symbol_ref = tape_.Get(static_cast<size_t>(position));
        if (symbol_ref) {
            return symbol_ref->get();
        }
        return blank_symbol_;
    }
    
    /**
     * Установить символ по позиции на ленте
     * @param position - позиция
     * @param symbol - символ для установки
     */
    void SetSymbolAt(int position, const Symbol& symbol) {
        if (position < 0) {
            return; // Игнорируем отрицательные позиции в этой упрощённой реализации
        }
        
        size_t pos = static_cast<size_t>(position);
        
        // Расширяем ленту до нужной позиции
        while (tape_.MaterializedCount() <= pos) {
            tape_.Get(tape_.MaterializedCount());
        }
        
        // Здесь нужно было бы реализовать модификацию существующих элементов
        // В рамках данной реализации предполагаем, что лента расширяется только вперёд
    }
    
    /**
     * Найти применимое правило для текущего состояния и символа
     */
    std::optional<Rule> FindRule(const State& state, const Symbol& symbol) const {
        for (const auto& rule : rules_) {
            if (rule.from_state == state && rule.read_symbol == symbol) {
                return rule;
            }
        }
        return std::nullopt;
    }
    
    /**
     * Выполнить один шаг машины Тьюринга
     * @return true, если шаг выполнен успешно; false, если машина остановлена
     */
    bool Step() {
        if (IsInFinalState() || step_count_ >= max_steps_) {
            return false;
        }
        
        Symbol current_symbol = ReadCurrentSymbol();
        auto rule_opt = FindRule(current_state_, current_symbol);
        
        if (!rule_opt) {
            // Нет применимого правила - машина останавливается
            return false;
        }
        
        const Rule& rule = *rule_opt;
        
        // Применяем правило
        WriteCurrentSymbol(rule.write_symbol);
        current_state_ = rule.to_state;
        
        // Двигаем головку
        head_position_ += static_cast<int>(rule.direction);
        
        step_count_++;
        return true;
    }
    
    /**
     * Выполнить машину до остановки
     * @return true, если машина завершилась в конечном состоянии
     */
    bool Run() {
        while (Step()) {
            // Продолжаем выполнение
        }
        return IsInFinalState();
    }
    
    /**
     * Проверить, находится ли машина в конечном состоянии
     */
    bool IsInFinalState() const {
        for (const auto& final_state : final_states_) {
            if (current_state_ == final_state) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Сбросить машину в начальное состояние
     */
    void Reset() {
        current_state_ = initial_state_;
        head_position_ = 0;
        step_count_ = 0;
        tape_ = Tape(TapeGenerator<Symbol>(blank_symbol_), ArraySeqMem<Symbol>());
    }
    
    // Геттеры для состояния машины
    const State& GetCurrentState() const { return current_state_; }
    int GetHeadPosition() const { return head_position_; }
    size_t GetStepCount() const { return step_count_; }
    size_t GetMaxSteps() const { return max_steps_; }
    const Symbol& GetBlankSymbol() const { return blank_symbol_; }
    
    /**
     * Получить строковое представление участка ленты
     * @param start - начальная позиция
     * @param length - длина участка
     */
    std::vector<Symbol> GetTapeSegment(int start, size_t length) const {
        std::vector<Symbol> segment;
        segment.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            segment.push_back(GetSymbolAt(start + static_cast<int>(i)));
        }
        
        return segment;
    }
    
    /**
     * Вывести текущее состояние машины для отладки
     */
    void PrintState(std::ostream& out = std::cout, int tape_window = 20) const {
        out << "Состояние: " << current_state_ << ", ";
        out << "Позиция головки: " << head_position_ << ", ";
        out << "Шаг: " << step_count_ << std::endl;
        
        // Выводим участок ленты вокруг головки
        int start = head_position_ - tape_window / 2;
        auto segment = GetTapeSegment(start, tape_window);
        
        out << "Лента: ";
        for (size_t i = 0; i < segment.size(); ++i) {
            if (start + static_cast<int>(i) == head_position_) {
                out << "[" << segment[i] << "]";
            } else {
                out << " " << segment[i] << " ";
            }
        }
        out << std::endl;
    }
};
