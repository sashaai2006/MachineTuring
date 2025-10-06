#pragma once

#include "LazySeq.h"
#include "Gen.h"
#include "Mem.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include <iostream>
#include <memory>

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
 * Описывает: (состояние, символ) -> (новое_состояние, новый_символ, направление)
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
    
    // Операторы для сравнения (для использования в hash-таблицах)
    bool operator==(const TransitionRule& other) const {
        return from_state == other.from_state && read_symbol == other.read_symbol;
    }
};

/**
 * Менеджер правил переходов
 * Ответственность: хранение и поиск правил переходов
 */
template <typename State, typename Symbol>
class TransitionManager {
public:
    using Rule = TransitionRule<State, Symbol>;
    using RuleKey = std::pair<State, Symbol>;
    
private:
    std::unordered_map<RuleKey, Rule, std::hash<RuleKey>> rules_map_;
    
    // Кастомный хеш-функционал для пары (State, Symbol)
    struct PairHasher {
        std::size_t operator()(const RuleKey& key) const {
            std::size_t h1 = std::hash<State>{}(key.first);
            std::size_t h2 = std::hash<Symbol>{}(key.second);
            return h1 ^ (h2 << 1);
        }
    };
    
public:
    TransitionManager() {
        // Переинициализируем с кастомным хешером
        rules_map_ = std::unordered_map<RuleKey, Rule, PairHasher>();
    }
    
    /**
     * Добавить правило перехода
     */
    void AddRule(const Rule& rule) {
        RuleKey key = std::make_pair(rule.from_state, rule.read_symbol);
        rules_map_[key] = rule;
    }
    
    void AddRule(const State& from_state, const Symbol& read_symbol,
                 const State& to_state, const Symbol& write_symbol,
                 Direction direction) {
        Rule rule(from_state, read_symbol, to_state, write_symbol, direction);
        AddRule(rule);
    }
    
    /**
     * Найти правило для заданного состояния и символа
     */
    std::optional<Rule> FindRule(const State& state, const Symbol& symbol) const {
        RuleKey key = std::make_pair(state, symbol);
        auto it = rules_map_.find(key);
        if (it != rules_map_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * Проверить, существует ли правило
     */
    bool HasRule(const State& state, const Symbol& symbol) const {
        RuleKey key = std::make_pair(state, symbol);
        return rules_map_.find(key) != rules_map_.end();
    }
    
    /**
     * Очистить все правила
     */
    void Clear() {
        rules_map_.clear();
    }
    
    /**
     * Получить количество правил
     */
    size_t GetRulesCount() const {
        return rules_map_.size();
    }
};

/**
 * Менеджер состояний
 * Ответственность: управление начальным, текущим и конечными состояниями
 */
template <typename State>
class StateManager {
private:
    State initial_state_;
    State current_state_;
    std::unordered_set<State> final_states_;
    
public:
    explicit StateManager(const State& initial_state)
        : initial_state_(initial_state), current_state_(initial_state) {}
    
    /**
     * Получить текущее состояние
     */
    const State& GetCurrentState() const {
        return current_state_;
    }
    
    /**
     * Установить новое текущее состояние
     */
    void SetCurrentState(const State& state) {
        current_state_ = state;
    }
    
    /**
     * Сбросить в начальное состояние
     */
    void Reset() {
        current_state_ = initial_state_;
    }
    
    /**
     * Добавить конечное состояние
     */
    void AddFinalState(const State& state) {
        final_states_.insert(state);
    }
    
    /**
     * Проверить, является ли текущее состояние конечным
     */
    bool IsInFinalState() const {
        return final_states_.find(current_state_) != final_states_.end();
    }
    
    /**
     * Проверить, является ли состояние конечным
     */
    bool IsFinalState(const State& state) const {
        return final_states_.find(state) != final_states_.end();
    }
    
    /**
     * Получить начальное состояние
     */
    const State& GetInitialState() const {
        return initial_state_;
    }
    
    /**
     * Получить количество конечных состояний
     */
    size_t GetFinalStatesCount() const {
        return final_states_.size();
    }
};

/**
 * Лента машины Тьюринга на основе LazySequence
 * Ответственность: хранение, чтение и запись символов на бесконечной ленте
 */
template <typename Symbol>
class TuringTape {
public:
    using TapeSequence = LazySeq<Symbol, TapeGenerator<Symbol, std::vector<Symbol>>, ArraySeqMem<Symbol>>;
    
private:
    mutable TapeSequence tape_;
    Symbol blank_symbol_;
    
    // Кеш для модифицированных ячеек (оригинал LazySeq не поддерживает модификацию)
    mutable std::unordered_map<int, Symbol> modifications_;
    
public:
    /**
     * Конструктор ленты
     * @param blank_symbol Пустой символ
     * @param initial_data Начальные данные на ленте
     */
    explicit TuringTape(const Symbol& blank_symbol, const std::vector<Symbol>& initial_data = {})
        : tape_(TapeGenerator<Symbol, std::vector<Symbol>>(initial_data, blank_symbol), ArraySeqMem<Symbol>()),
          blank_symbol_(blank_symbol) {}
    
    /**
     * Получить символ по позиции
     * @param position Позиция на ленте (может быть отрицательной)
     */
    Symbol GetSymbolAt(int position) const {
        // Сначала проверяем кеш модификаций
        auto it = modifications_.find(position);
        if (it != modifications_.end()) {
            return it->second;
        }
        
        // Если позиция отрицательная, возвращаем пустой символ
        if (position < 0) {
            return blank_symbol_;
        }
        
        // Получаем из ленивой последовательности
        auto symbol_ref = tape_.Get(static_cast<size_t>(position));
        if (symbol_ref) {
            return symbol_ref->get();
        }
        
        return blank_symbol_;
    }
    
    /**
     * Установить символ по позиции
     * @param position Позиция на ленте
     * @param symbol Новый символ
     */
    void SetSymbolAt(int position, const Symbol& symbol) {
        // Сохраняем модификацию в кеше
        modifications_[position] = symbol;
    }
    
    /**
     * Получить сегмент ленты
     * @param start_pos Начальная позиция
     * @param length Длина сегмента
     */
    std::vector<Symbol> GetSegment(int start_pos, size_t length) const {
        std::vector<Symbol> segment;
        segment.reserve(length);
        
        for (size_t i = 0; i < length; ++i) {
            segment.push_back(GetSymbolAt(start_pos + static_cast<int>(i)));
        }
        
        return segment;
    }
    
    /**
     * Очистить ленту (сбросить модификации)
     */
    void Reset(const std::vector<Symbol>& new_initial_data = {}) {
        modifications_.clear();
        tape_ = TapeSequence(TapeGenerator<Symbol, std::vector<Symbol>>(new_initial_data, blank_symbol_), 
                             ArraySeqMem<Symbol>());
    }
    
    /**
     * Получить пустой символ
     */
    const Symbol& GetBlankSymbol() const {
        return blank_symbol_;
    }
    
    /**
     * Получить статистику о ленте
     */
    size_t GetMaterializedCount() const {
        return tape_.MaterializedCount();
    }
    
    size_t GetModificationsCount() const {
        return modifications_.size();
    }
};

/**
 * Менеджер головки машины Тьюринга
 * Ответственность: управление позицией головки чтения/записи
 */
class HeadManager {
private:
    int position_;
    int initial_position_;
    
public:
    explicit HeadManager(int initial_position = 0)
        : position_(initial_position), initial_position_(initial_position) {}
    
    /**
     * Получить текущую позицию головки
     */
    int GetPosition() const {
        return position_;
    }
    
    /**
     * Переместить головку
     */
    void Move(Direction direction) {
        position_ += static_cast<int>(direction);
    }
    
    /**
     * Установить позицию головки
     */
    void SetPosition(int position) {
        position_ = position;
    }
    
    /**
     * Сбросить в начальную позицию
     */
    void Reset() {
        position_ = initial_position_;
    }
    
    /**
     * Получить начальную позицию
     */
    int GetInitialPosition() const {
        return initial_position_;
    }
};
