#pragma once

#include <stdexcept>
#include <optional>
#include <functional>

/**
 * Базовый интерфейс для генератора элементов
 * Обеспечивает единообразный API для различных типов генераторов
 */
template <typename T>
class IGenerator {
public:
    virtual ~IGenerator() = default;
    
    /**
     * Получить следующий элемент
     * @throws std::out_of_range если больше нет элементов
     */
    virtual T GetNext() = 0;
    
    /**
     * Проверить, есть ли следующий элемент
     */
    virtual bool HasNext() const = 0;
    
    /**
     * Попытаться получить следующий элемент без исключений
     */
    virtual std::optional<T> TryGetNext() = 0;
};

/**
 * Генератор на основе функции
 * Используется для создания бесконечных последовательностей
 */
template <typename T>
class FunctionGenerator : public IGenerator<T> {
private:
    std::function<T()> generator_func_;
    mutable bool has_next_;
    
public:
    explicit FunctionGenerator(std::function<T()> func) 
        : generator_func_(std::move(func)), has_next_(true) {}
    
    T GetNext() override {
        if (!has_next_) {
            throw std::out_of_range("Нет больше элементов в генераторе");
        }
        
        try {
            return generator_func_();
        } catch (...) {
            has_next_ = false;
            throw;
        }
    }
    
    bool HasNext() const override {
        return has_next_;
    }
    
    std::optional<T> TryGetNext() override {
        if (!has_next_) {
            return std::nullopt;
        }
        
        try {
            T result = generator_func_();
            return result;
        } catch (...) {
            has_next_ = false;
            return std::nullopt;
        }
    }
};

/**
 * Генератор константного символа для ленты машины Тьюринга
 * Заполняет пустые ячейки ленты одним и тем же символом (blank symbol)
 */
template <typename T>
class ConstantGenerator : public IGenerator<T> {
private:
    T constant_value_;
    
public:
    explicit ConstantGenerator(const T& value) : constant_value_(value) {}
    
    T GetNext() override {
        return constant_value_;
    }
    
    bool HasNext() const override {
        return true; // Константный генератор всегда имеет следующий элемент
    }
    
    std::optional<T> TryGetNext() override {
        return constant_value_;
    }
};

/**
 * Генератор последовательности из контейнера
 * Используется для инициализации ленты начальными данными
 */
template <typename T, typename Container>
class SequenceGenerator : public IGenerator<T> {
private:
    Container data_;
    mutable size_t current_index_;
    T default_value_;
    
public:
    explicit SequenceGenerator(const Container& data, const T& default_val = T{})
        : data_(data), current_index_(0), default_value_(default_val) {}
    
    T GetNext() override {
        if (current_index_ < data_.size()) {
            return data_[current_index_++];
        }
        // После исчерпания контейнера возвращаем значение по умолчанию
        return default_value_;
    }
    
    bool HasNext() const override {
        return true; // Всегда имеет следующий элемент (default_value_)
    }
    
    std::optional<T> TryGetNext() override {
        if (current_index_ < data_.size()) {
            return data_[current_index_++];
        }
        return default_value_;
    }
    
    // Дополнительные методы для работы с последовательностью
    size_t GetCurrentIndex() const { return current_index_; }
    size_t GetDataSize() const { return data_.size(); }
    bool IsInDataRange() const { return current_index_ < data_.size(); }
};

/**
 * Комбинированный генератор для ленты машины Тьюринга
 * Объединяет последовательность начальных данных с константным заполнением
 */
template <typename T, typename Container>
class TapeGenerator : public IGenerator<T> {
private:
    SequenceGenerator<T, Container> sequence_gen_;
    ConstantGenerator<T> constant_gen_;
    bool using_sequence_;
    
public:
    TapeGenerator(const Container& initial_data, const T& blank_symbol)
        : sequence_gen_(initial_data, blank_symbol),
          constant_gen_(blank_symbol),
          using_sequence_(true) {}
    
    T GetNext() override {
        if (using_sequence_ && sequence_gen_.IsInDataRange()) {
            return sequence_gen_.GetNext();
        } else {
            using_sequence_ = false;
            return constant_gen_.GetNext();
        }
    }
    
    bool HasNext() const override {
        return true; // Лента бесконечна
    }
    
    std::optional<T> TryGetNext() override {
        if (using_sequence_ && sequence_gen_.IsInDataRange()) {
            return sequence_gen_.TryGetNext();
        } else {
            using_sequence_ = false;
            return constant_gen_.TryGetNext();
        }
    }
    
    // Специфичные методы для ленты
    bool IsInInitialDataRange() const {
        return using_sequence_ && sequence_gen_.IsInDataRange();
    }
    
    size_t GetInitialDataSize() const {
        return sequence_gen_.GetDataSize();
    }
};

/**
 * Фабрика для создания различных типов генераторов
 */
template <typename T>
class GeneratorFactory {
public:
    // Создать константный генератор
    static std::unique_ptr<IGenerator<T>> CreateConstant(const T& value) {
        return std::make_unique<ConstantGenerator<T>>(value);
    }
    
    // Создать функциональный генератор
    static std::unique_ptr<IGenerator<T>> CreateFunction(std::function<T()> func) {
        return std::make_unique<FunctionGenerator<T>>(std::move(func));
    }
    
    // Создать генератор последовательности
    template <typename Container>
    static std::unique_ptr<IGenerator<T>> CreateSequence(const Container& data, const T& default_val = T{}) {
        return std::make_unique<SequenceGenerator<T, Container>>(data, default_val);
    }
    
    // Создать генератор для ленты машины Тьюринга
    template <typename Container>
    static std::unique_ptr<IGenerator<T>> CreateTape(const Container& initial_data, const T& blank_symbol) {
        return std::make_unique<TapeGenerator<T, Container>>(initial_data, blank_symbol);
    }
};
