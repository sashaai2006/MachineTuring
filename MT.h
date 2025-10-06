#pragma once

#include "SmartPtrs.h"
#include "StatisticsManager.h"
#include "TransitionManager.h"
#include "StateManager.h"
#include "TuringStrip.h"
#include "HeadManager.h"

#include <stdexcept>
#include <sstream>

/**
 * Результат выполнения машины Тьюринга
 */
enum class ExecutionResult {
    ACCEPTED,     // Принято (достигнуто конечное состояние)
    REJECTED,     // Отклонено (нет правила для перехода)
    TIMEOUT,      // Превышен лимит шагов
    ERROR         // Ошибка выполнения
};

/**
 * Машина Тьюринга
 * Основной класс, инкапсулирующий все компоненты машины Тьюринга
 * Использует композицию для управления состояниями, лентой, правилами переходов и статистикой
 */
template <typename State, typename Symbol>
class TuringMachine {
private:
    // Используем твои самописные умные указатели
    UniquePtr<StateManager<State>> state_manager_;
    UniquePtr<TuringStrip<Symbol>> strip_;  // Переименовано с tape_ на strip_
    UniquePtr<TransitionManager<State, Symbol>> transition_manager_;
    UniquePtr<HeadManager> head_manager_;
    UniquePtr<StatisticsManager> statistics_manager_;
    
public:
    /**
     * Конструктор машины Тьюринга
     * @param initial_state Начальное состояние
     * @param blank_symbol Пустой символ
     * @param initial_data Начальные данные на ленте
     * @param initial_head_position Начальная позиция головки
     */
    explicit TuringMachine(const State& initial_state, 
                          const Symbol& blank_symbol,
                          const std::vector<Symbol>& initial_data = {},
                          int initial_head_position = 0)
        : state_manager_(UniquePtr<StateManager<State>>::MakeUnique(initial_state)),
          strip_(UniquePtr<TuringStrip<Symbol>>::MakeUnique(blank_symbol, initial_data)),
          transition_manager_(UniquePtr<TransitionManager<State, Symbol>>::MakeUnique()),
          head_manager_(UniquePtr<HeadManager>::MakeUnique(initial_head_position)),
          statistics_manager_(UniquePtr<StatisticsManager>::MakeUnique()) {}
    
    /**
     * Добавить правило перехода
     */
    void AddTransition(const State& from_state, const Symbol& read_symbol,
                      const State& to_state, const Symbol& write_symbol,
                      Direction direction) {
        transition_manager_->AddRule(from_state, read_symbol, to_state, write_symbol, direction);
    }
    
    /**
     * Добавить конечное состояние
     */
    void AddFinalState(const State& state) {
        state_manager_->AddFinalState(state);
    }
    
    /**
     * Выполнить один шаг машины Тьюринга
     * @return true если шаг выполнен успешно, false если нет правила для перехода
     */
    bool Step() {
        // Проверяем, не превышен ли лимит шагов
        if (statistics_manager_->IsStepLimitExceeded()) {
            return false;
        }
        
        // Получаем текущее состояние и символ под головкой
        const State& current_state = state_manager_->GetCurrentState();
        Symbol current_symbol = strip_->GetSymbolAt(head_manager_->GetPosition());
        
        // Ищем правило для текущей конфигурации
        auto rule = transition_manager_->FindRule(current_state, current_symbol);
        
        if (!rule) {
            return false;  // Нет правила - останавливаемся
        }
        
        // Применяем правило
        state_manager_->SetCurrentState(rule->to_state);
        strip_->SetSymbolAt(head_manager_->GetPosition(), rule->write_symbol);
        head_manager_->Move(rule->direction);
        
        // Обновляем статистику
        statistics_manager_->IncrementStepCount();
        
        return true;
    }
    
    /**
     * Запустить машину до остановки
     * @param max_steps Максимальное количество шагов (0 = использовать настройки StatisticsManager)
     * @return Результат выполнения
     */
    ExecutionResult Run(size_t max_steps = 0) {
        if (max_steps > 0) {
            statistics_manager_->SetMaxSteps(max_steps);
        }
        
        statistics_manager_->StartExecution();
        
        try {
            while (true) {
                // Проверяем конечное состояние
                if (state_manager_->IsInFinalState()) {
                    statistics_manager_->EndExecution();
                    return ExecutionResult::ACCEPTED;
                }
                
                // Проверяем превышение лимита шагов
                if (statistics_manager_->IsStepLimitExceeded()) {
                    statistics_manager_->EndExecution();
                    return ExecutionResult::TIMEOUT;
                }
                
                // Выполняем шаг
                if (!Step()) {
                    statistics_manager_->EndExecution();
                    return ExecutionResult::REJECTED;
                }
            }
        } catch (const std::exception&) {
            statistics_manager_->EndExecution();
            return ExecutionResult::ERROR;
        }
    }
    
    /**
     * Сбросить машину в начальное состояние
     */
    void Reset(const std::vector<Symbol>& new_data = {}) {
        state_manager_->Reset();
        head_manager_->Reset();
        strip_->Reset(new_data);
        statistics_manager_->Reset();
    }
    
    // ===================
    // Геттеры для доступа к компонентам
    // ===================
    
    /**
     * Получить текущее состояние
     */
    const State& GetCurrentState() const {
        return state_manager_->GetCurrentState();
    }
    
    /**
     * Получить позицию головки
     */
    int GetHeadPosition() const {
        return head_manager_->GetPosition();
    }
    
    /**
     * Получить символ в текущей позиции головки
     */
    Symbol GetCurrentSymbol() const {
        return strip_->GetSymbolAt(head_manager_->GetPosition());
    }
    
    /**
     * Получить сегмент ленты
     */
    std::vector<Symbol> GetTapeSegment(int start_pos, size_t length) const {
        return strip_->GetSegment(start_pos, length);
    }
    
    /**
     * Проверить, находится ли машина в конечном состоянии
     */
    bool IsInFinalState() const {
        return state_manager_->IsInFinalState();
    }
    
    /**
     * Получить количество выполненных шагов
     */
    size_t GetStepCount() const {
        return statistics_manager_->GetStepCount();
    }
    
    /**
     * Получить время выполнения
     */
    std::chrono::milliseconds GetExecutionTime() const {
        return statistics_manager_->GetExecutionTime();
    }
    
    /**
     * Получить статистику выполнения
     */
    void PrintStatistics(std::ostream& out = std::cout) const {
        statistics_manager_->PrintStatistics(out);
        head_manager_->PrintMoveStatistics(out);
    }
    
    /**
     * Получить количество правил
     */
    size_t GetRulesCount() const {
        return transition_manager_->GetRulesCount();
    }
    
    /**
     * Получить пустой символ
     */
    const Symbol& GetBlankSymbol() const {
        return strip_->GetBlankSymbol();
    }
    
    /**
     * Получить начальное состояние
     */
    const State& GetInitialState() const {
        return state_manager_->GetInitialState();
    }
    
    // ===================
    // Дополнительные методы управления
    // ===================
    
    /**
     * Установить максимальное количество шагов
     */
    void SetMaxSteps(size_t max_steps) {
        statistics_manager_->SetMaxSteps(max_steps);
    }
    
    /**
     * Установить символ в определенную позицию
     */
    void SetSymbolAt(int position, const Symbol& symbol) {
        strip_->SetSymbolAt(position, symbol);
    }
    
    /**
     * Получить символ в определенной позиции
     */
    Symbol GetSymbolAt(int position) const {
        return strip_->GetSymbolAt(position);
    }
    
    /**
     * Переместить головку в определенную позицию
     */
    void SetHeadPosition(int position) {
        head_manager_->SetPosition(position);
    }
    
    /**
     * Установить текущее состояние
     */
    void SetCurrentState(const State& state) {
        state_manager_->SetCurrentState(state);
    }
    
    /**
     * Проверить, существует ли правило для данной конфигурации
     */
    bool HasTransition(const State& state, const Symbol& symbol) const {
        return transition_manager_->HasRule(state, symbol);
    }
    
    /**
     * Получить строковое представление конфигурации машины
     */
    std::string GetConfigurationString(int tape_window = 20) const {
        std::ostringstream oss;
        
        // Информация о состоянии
        oss << "Состояние: " << state_manager_->GetCurrentState() 
            << ", Позиция: " << head_manager_->GetPosition() << std::endl;
        
        // Отображение сегмента ленты
        int start_pos = head_manager_->GetPosition() - tape_window / 2;
        auto segment = strip_->GetSegment(start_pos, tape_window);
        
        oss << "Лента: ";
        for (size_t i = 0; i < segment.size(); ++i) {
            int pos = start_pos + static_cast<int>(i);
            if (pos == head_manager_->GetPosition()) {
                oss << "[" << segment[i] << "]";
            } else {
                oss << " " << segment[i] << " ";
            }
        }
        oss << std::endl;
        
        return oss.str();
    }
    
    // ===================
    // Методы для работы с менеджерами напрямую (если нужно)
    // ===================
    
    /**
     * Получить доступ к менеджеру состояний
     */
    StateManager<State>& GetStateManager() { 
        return *state_manager_; 
    }
    
    const StateManager<State>& GetStateManager() const { 
        return *state_manager_; 
    }
    
    /**
     * Получить доступ к ленте
     */
    TuringStrip<Symbol>& GetStrip() { 
        return *strip_; 
    }
    
    const TuringStrip<Symbol>& GetStrip() const { 
        return *strip_; 
    }
    
    /**
     * Получить доступ к менеджеру переходов
     */
    TransitionManager<State, Symbol>& GetTransitionManager() { 
        return *transition_manager_; 
    }
    
    const TransitionManager<State, Symbol>& GetTransitionManager() const { 
        return *transition_manager_; 
    }
    
    /**
     * Получить доступ к менеджеру головки
     */
    HeadManager& GetHeadManager() { 
        return *head_manager_; 
    }
    
    const HeadManager& GetHeadManager() const { 
        return *head_manager_; 
    }
    
    /**
     * Получить доступ к менеджеру статистики
     */
    StatisticsManager& GetStatisticsManager() { 
        return *statistics_manager_; 
    }
    
    const StatisticsManager& GetStatisticsManager() const { 
        return *statistics_manager_; 
    }
};

/**
 * Вспомогательная функция для создания машины Тьюринга
 */
template <typename State, typename Symbol>
UniquePtr<TuringMachine<State, Symbol>> MakeTuringMachine(
    const State& initial_state,
    const Symbol& blank_symbol,
    const std::vector<Symbol>& initial_data = {},
    int initial_head_position = 0) {
    
    return UniquePtr<TuringMachine<State, Symbol>>::MakeUnique(
        initial_state, blank_symbol, initial_data, initial_head_position);
}