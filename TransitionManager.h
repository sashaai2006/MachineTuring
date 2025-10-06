#pragma once

#include <unordered_map>
#include <optional>
#include <functional>

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