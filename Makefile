# Makefile для проекта Машины Тьюринга
# Использует новую модульную архитектуру с разделёнными файлами

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2 -g

# Директории
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Исходные файлы
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Заголовочные файлы (для отслеживания зависимостей)
HEADERS = \
	SmartPtrs.h \
	StatisticsManager.h \
	TransitionManager.h \
	StateManager.h \
	TuringStrip.h \
	HeadManager.h \
	MT.h \
	LazySeq.h \
	Gen.h \
	Mem.h

# Основная цель
TARGET = $(BIN_DIR)/turing_machine

# Цель для тестирования
TEST_TARGET = $(BIN_DIR)/test_turing

# Все цели
.PHONY: all clean test run help debug release install

# Основные правила
all: $(TARGET)

# Создание основного исполняемого файла
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	@echo "🔗 Сборка исполняемого файла..."
	$(CXX) $(OBJECTS) -o $@
	@echo "✅ Сборка завершена: $@"

# Правила компиляции объектных файлов
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) | $(OBJ_DIR)
	@echo "🔧 Компиляция $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Создание директорий
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Очистка
clean:
	@echo "🧹 Очистка файлов сборки..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "✅ Очистка завершена"

# Запуск программы
run: $(TARGET)
	@echo "🚀 Запуск программы..."
	@./$(TARGET)

# Тестирование (если есть тестовые файлы)
test: $(TEST_TARGET)
	@echo "🧪 Запуск тестов..."
	@./$(TEST_TARGET)

# Сборка тестов (если есть test.cpp)
$(TEST_TARGET): $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS)) $(OBJ_DIR)/test.o | $(BIN_DIR)
	@echo "🔗 Сборка тестов..."
	$(CXX) $^ -o $@
	@echo "✅ Тесты собраны: $@"

# Сборка в режиме отладки
debug: CXXFLAGS += -DDEBUG -O0 -g3
debug: clean $(TARGET)
	@echo "🐛 Сборка в режиме отладки завершена"

# Релизная сборка
release: CXXFLAGS += -DNDEBUG -O3 -s
release: clean $(TARGET)
	@echo "🚀 Релизная сборка завершена"

# Установка заголовочных файлов (опционально)
install: $(HEADERS)
	@echo "📦 Установка заголовочных файлов..."
	@mkdir -p /usr/local/include/turing_machine
	@cp $(HEADERS) /usr/local/include/turing_machine/
	@echo "✅ Заголовочные файлы установлены в /usr/local/include/turing_machine/"

# Помощь
help:
	@echo "🎯 Доступные цели:"
	@echo "  all      - Собрать основную программу (по умолчанию)"
	@echo "  clean    - Удалить все файлы сборки"
	@echo "  run      - Собрать и запустить программу"
	@echo "  test     - Собрать и запустить тесты"
	@echo "  debug    - Собрать в режиме отладки"
	@echo "  release  - Собрать релизную версию"
	@echo "  install  - Установить заголовочные файлы"
	@echo "  help     - Показать эту справку"
	@echo ""
	@echo "📁 Структура файлов:"
	@echo "  Заголовочные: $(HEADERS)"
	@echo "  Исполняемый:  $(TARGET)"
	@echo "  Тесты:        $(TEST_TARGET)"

# Проверка стиля кода (если установлен clang-format)
format:
	@echo "✨ Форматирование кода..."
	@find . -name '*.h' -o -name '*.cpp' | xargs clang-format -i
	@echo "✅ Форматирование завершено"

# Статический анализ (если установлен cppcheck)
analyze:
	@echo "🔍 Статический анализ кода..."
	@cppcheck --enable=all --std=c++17 $(SRC_DIR)
	@echo "✅ Анализ завершён"

# Информация о сборке
info:
	@echo "ℹ️  Информация о сборке:"
	@echo "  Компилятор: $(CXX)"
	@echo "  Флаги:      $(CXXFLAGS)"
	@echo "  Исходники:  $(SOURCES)"
	@echo "  Объекты:    $(OBJECTS)"
	@echo "  Цель:       $(TARGET)"

# Быстрая пересборка (только изменённые файлы)
rebuild: clean all

# Создание архива проекта
archive:
	@echo "📦 Создание архива проекта..."
	@tar -czf turing_machine_$(shell date +%Y%m%d).tar.gz *.h *.cpp Makefile README.md 2>/dev/null || true
	@echo "✅ Архив создан: turing_machine_$(shell date +%Y%m%d).tar.gz"

# Цели, которые не создают файлы
.PHONY: format analyze info rebuild archive