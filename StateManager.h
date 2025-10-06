#pragma once

#include <unordered_set>

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
     * Удалить конечное состояние
     */
    void RemoveFinalState(const State& state) {
        final_states_.erase(state);
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
     * Установить новое начальное состояние (с сбросом текущего)
     */
    void SetInitialState(const State& state) {
        initial_state_ = state;
        current_state_ = state;
    }
    
    /**
     * Получить количество конечных состояний
     */
    size_t GetFinalStatesCount() const {
        return final_states_.size();
    }
    
    /**
     * Очистить все конечные состояния
     */
    void ClearFinalStates() {
        final_states_.clear();
    }
    
    /**
     * Получить все конечные состояния
     */
    const std::unordered_set<State>& GetFinalStates() const {
        return final_states_;
    }
    
    /**
     * Проверить, есть ли конечные состояния
     */
    bool HasFinalStates() const {
        return !final_states_.empty();
    }
};