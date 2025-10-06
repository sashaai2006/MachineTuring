#pragma once

#include "TransitionManager.h"  // Для Direction

/**
 * Менеджер головки машины Тьюринга
 * Ответственность: управление позицией головки чтения/записи
 */
class HeadManager {
private:
    int position_;
    int initial_position_;
    
    // Статистика перемещений
    size_t total_moves_;
    size_t left_moves_;
    size_t right_moves_;
    size_t stay_moves_;
    
    // Минимальная и максимальная позиция
    int min_position_;
    int max_position_;
    
public:
    explicit HeadManager(int initial_position = 0)
        : position_(initial_position), 
          initial_position_(initial_position),
          total_moves_(0),
          left_moves_(0),
          right_moves_(0), 
          stay_moves_(0),
          min_position_(initial_position),
          max_position_(initial_position) {}
    
    /**
     * Получить текущую позицию головки
     */
    int GetPosition() const {
        return position_;
    }
    
    /**
     * Переместить головку в соответствии с направлением
     */
    void Move(Direction direction) {
        total_moves_++;
        
        switch (direction) {
            case Direction::LEFT:
                position_--;
                left_moves_++;
                if (position_ < min_position_) {
                    min_position_ = position_;
                }
                break;
                
            case Direction::RIGHT:
                position_++;
                right_moves_++;
                if (position_ > max_position_) {
                    max_position_ = position_;
                }
                break;
                
            case Direction::STAY:
                stay_moves_++;
                break;
        }
    }
    
    /**
     * Переместить головку на определённое количество позиций
     */
    void MoveBy(int offset) {
        if (offset > 0) {
            for (int i = 0; i < offset; ++i) {
                Move(Direction::RIGHT);
            }
        } else if (offset < 0) {
            for (int i = 0; i < -offset; ++i) {
                Move(Direction::LEFT);
            }
        }
        // Если offset == 0, ничего не делаем
    }
    
    /**
     * Установить позицию головки напрямую
     */
    void SetPosition(int position) {
        position_ = position;
        
        // Обновляем мин/макс позиции
        if (position_ < min_position_) {
            min_position_ = position_;
        }
        if (position_ > max_position_) {
            max_position_ = position_;
        }
    }
    
    /**
     * Сбросить в начальную позицию
     */
    void Reset() {
        position_ = initial_position_;
        total_moves_ = 0;
        left_moves_ = 0;
        right_moves_ = 0;
        stay_moves_ = 0;
        min_position_ = initial_position_;
        max_position_ = initial_position_;
    }
    
    /**
     * Получить начальную позицию
     */
    int GetInitialPosition() const {
        return initial_position_;
    }
    
    /**
     * Установить новую начальную позицию
     */
    void SetInitialPosition(int position) {
        initial_position_ = position;
        Reset();  // Сбрасываем к новой начальной позиции
    }
    
    // =================
    // Статистика перемещений
    // =================
    
    /**
     * Получить общее количество перемещений
     */
    size_t GetTotalMoves() const {
        return total_moves_;
    }
    
    /**
     * Получить количество перемещений влево
     */
    size_t GetLeftMoves() const {
        return left_moves_;
    }
    
    /**
     * Получить количество перемещений вправо
     */
    size_t GetRightMoves() const {
        return right_moves_;
    }
    
    /**
     * Получить количество стояний на месте
     */
    size_t GetStayMoves() const {
        return stay_moves_;
    }
    
    /**
     * Получить минимальную посещённую позицию
     */
    int GetMinPosition() const {
        return min_position_;
    }
    
    /**
     * Получить максимальную посещённую позицию
     */
    int GetMaxPosition() const {
        return max_position_;
    }
    
    /**
     * Получить ширину использованной ленты
     */
    int GetTapeUsage() const {
        return max_position_ - min_position_ + 1;
    }
    
    /**
     * Получить смещение от начальной позиции
     */
    int GetOffsetFromInitial() const {
        return position_ - initial_position_;
    }
    
    /**
     * Получить среднюю позицию
     */
    double GetAveragePosition() const {
        if (total_moves_ == 0) return static_cast<double>(initial_position_);
        
        // Простое приближение - среднее между мин и макс
        return static_cast<double>(min_position_ + max_position_) / 2.0;
    }
    
    /**
     * Вывести статистику перемещений
     */
    void PrintMoveStatistics(std::ostream& out = std::cout) const {
        out << "=== Статистика головки ===" << std::endl;
        out << "Общее количество перемещений: " << total_moves_ << std::endl;
        out << "Перемещений влево: " << left_moves_ << std::endl;
        out << "Перемещений вправо: " << right_moves_ << std::endl;
        out << "Остановок на месте: " << stay_moves_ << std::endl;
        out << "Диапазон позиций: [" << min_position_ << ", " << max_position_ << "]" << std::endl;
        out << "Ширина использованной ленты: " << GetTapeUsage() << std::endl;
        out << "Смещение от начала: " << GetOffsetFromInitial() << std::endl;
    }
};