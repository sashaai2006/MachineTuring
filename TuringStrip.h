#pragma once

#include "LazySeq.h"
#include "Gen.h"
#include "Mem.h"

#include <vector>
#include <unordered_map>

/**
 * Лента (полоса) машины Тьюринга на основе LazySequence
 * Ответственность: хранение, чтение и запись символов на бесконечной ленте
 */
template <typename Symbol>
class TuringStrip {
public:
    using StripSequence = LazySeq<Symbol, TapeGenerator<Symbol, std::vector<Symbol>>, ArraySeqMem<Symbol>>;
    
private:
    mutable StripSequence strip_;
    Symbol blank_symbol_;
    
    // Кеш для модифицированных ячеек (оригинал LazySeq не поддерживает модификацию)
    mutable std::unordered_map<int, Symbol> modifications_;
    
public:
    /**
     * Конструктор ленты
     * @param blank_symbol Пустой символ
     * @param initial_data Начальные данные на ленте
     */
    explicit TuringStrip(const Symbol& blank_symbol, const std::vector<Symbol>& initial_data = {})
        : strip_(TapeGenerator<Symbol, std::vector<Symbol>>(initial_data, blank_symbol), ArraySeqMem<Symbol>()),
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
        auto symbol_ref = strip_.Get(static_cast<size_t>(position));
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
        strip_ = StripSequence(TapeGenerator<Symbol, std::vector<Symbol>>(new_initial_data, blank_symbol_), 
                             ArraySeqMem<Symbol>());
    }
    
    /**
     * Получить пустой символ
     */
    const Symbol& GetBlankSymbol() const {
        return blank_symbol_;
    }
    
    /**
     * Установить новый пустой символ
     */
    void SetBlankSymbol(const Symbol& blank_symbol) {
        blank_symbol_ = blank_symbol;
    }
    
    /**
     * Получить статистику о ленте
     */
    size_t GetMaterializedCount() const {
        return strip_.MaterializedCount();
    }
    
    size_t GetModificationsCount() const {
        return modifications_.size();
    }
    
    /**
     * Проверить, есть ли модификации на ленте
     */
    bool HasModifications() const {
        return !modifications_.empty();
    }
    
    /**
     * Получить все модификации (позиция -> символ)
     */
    const std::unordered_map<int, Symbol>& GetModifications() const {
        return modifications_;
    }
    
    /**
     * Очистить только модификации (оставить основную ленту нетронутой)
     */
    void ClearModifications() {
        modifications_.clear();
    }
    
    /**
     * Применить все модификации к основной ленте (опционально)
     */
    void ApplyModifications() {
        // Для демонстрации - можно реализовать позже если понадобится
        // Пока просто очищаем модификации
        ClearModifications();
    }
};