#pragma once

#include <chrono>
#include <iostream>

/**
 * Менеджер статистики выполнения машины Тьюринга
 * Отвечает за сбор и предоставление статистической информации
 */
class StatisticsManager {
private:
    size_t step_count_;
    size_t max_steps_;
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    bool execution_started_;
    bool execution_finished_;
    
public:
    explicit StatisticsManager(size_t max_steps = 100000)
        : step_count_(0), 
          max_steps_(max_steps),
          execution_started_(false),
          execution_finished_(false) {}
    
    /**
     * Начать измерение времени выполнения
     */
    void StartExecution() {
        start_time_ = std::chrono::high_resolution_clock::now();
        execution_started_ = true;
        execution_finished_ = false;
        step_count_ = 0;
    }
    
    /**
     * Завершить измерение времени выполнения
     */
    void EndExecution() {
        end_time_ = std::chrono::high_resolution_clock::now();
        execution_finished_ = true;
    }
    
    /**
     * Увеличить счётчик шагов
     */
    void IncrementStepCount() {
        step_count_++;
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
     * Установить максимальное количество шагов
     */
    void SetMaxSteps(size_t max_steps) {
        max_steps_ = max_steps;
    }
    
    /**
     * Проверить, превышен ли лимит шагов
     */
    bool IsStepLimitExceeded() const {
        return step_count_ >= max_steps_;
    }
    
    /**
     * Получить время выполнения в миллисекундах
     */
    std::chrono::milliseconds GetExecutionTime() const {
        if (!execution_started_) {
            return std::chrono::milliseconds(0);
        }
        
        auto end = execution_finished_ ? end_time_ : std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_);
    }
    
    /**
     * Получить время выполнения в микросекундах
     */
    std::chrono::microseconds GetExecutionTimeMicros() const {
        if (!execution_started_) {
            return std::chrono::microseconds(0);
        }
        
        auto end = execution_finished_ ? end_time_ : std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    }
    
    /**
     * Получить среднее время на шаг в микросекундах
     */
    double GetAverageTimePerStep() const {
        if (step_count_ == 0) return 0.0;
        
        auto total_time = GetExecutionTimeMicros().count();
        return static_cast<double>(total_time) / static_cast<double>(step_count_);
    }
    
    /**
     * Сбросить всю статистику
     */
    void Reset() {
        step_count_ = 0;
        execution_started_ = false;
        execution_finished_ = false;
    }
    
    /**
     * Вывести статистику выполнения
     */
    void PrintStatistics(std::ostream& out = std::cout) const {
        out << "=== Статистика выполнения ===" << std::endl;
        out << "Шагов выполнено: " << step_count_ << std::endl;
        out << "Максимально шагов: " << max_steps_ << std::endl;
        out << "Время выполнения: " << GetExecutionTime().count() << " мс" << std::endl;
        
        if (step_count_ > 0) {
            out << "Среднее время на шаг: " << GetAverageTimePerStep() << " мкс" << std::endl;
        }
        
        out << "Лимит шагов превышен: " << (IsStepLimitExceeded() ? "Да" : "Нет") << std::endl;
    }
    
    /**
     * Проверить, началось ли выполнение
     */
    bool IsExecutionStarted() const {
        return execution_started_;
    }
    
    /**
     * Проверить, завершилось ли выполнение
     */
    bool IsExecutionFinished() const {
        return execution_finished_;
    }
};