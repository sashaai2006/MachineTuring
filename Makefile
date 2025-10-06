# Makefile для проекта машины Тьюринга
# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g

# Директории
SRC_DIR = .
BUILD_DIR = build

# Исходные файлы
SOURCES = 
HEADERS = MT.h LazySeq.h Mem.h Gen.h

# Исполняемые файлы
EXAMPLES_TARGET = $(BUILD_DIR)/examples
TESTS_TARGET = $(BUILD_DIR)/tests

# Основная цель - собрать всё
all: $(BUILD_DIR) $(EXAMPLES_TARGET) $(TESTS_TARGET)

# Создание директории для сборки
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Сборка примеров
$(EXAMPLES_TARGET): examples.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) examples.cpp -o $(EXAMPLES_TARGET)

# Сборка тестов
$(TESTS_TARGET): tests.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) tests.cpp -o $(TESTS_TARGET)

# Запуск примеров
run-examples: $(EXAMPLES_TARGET)
	@echo "Запуск примеров машины Тьюринга..."
	@./$(EXAMPLES_TARGET)

# Запуск тестов
run-tests: $(TESTS_TARGET)
	@echo "Запуск модульных тестов..."
	@./$(TESTS_TARGET)

# Запуск всего (тесты + примеры)
run: run-tests run-examples

# Очистка собранных файлов
clean:
	rm -rf $(BUILD_DIR)

# Форматирование кода (если установлен clang-format)
format:
	@if command -v clang-format >/dev/null 2>&1; then \
		echo "Форматирование кода..."; \
		clang-format -i *.cpp *.h; \
	else \
		echo "clang-format не найден, пропускаем форматирование"; \
	fi

# Проверка статического анализа (если установлен cppcheck)
check:
	@if command -v cppcheck >/dev/null 2>&1; then \
		echo "Статический анализ кода..."; \
		cppcheck --enable=all --std=c++17 --quiet *.cpp *.h; \
	else \
		echo "cppcheck не найден, пропускаем статический анализ"; \
	fi

# Отладочная сборка
debug: CXXFLAGS += -DDEBUG -g3 -O0
debug: all

# Релизная сборка
release: CXXFLAGS += -DNDEBUG -O3
release: all

# Помощь
help:
	@echo "Доступные команды:"
	@echo "  all          - Собрать всё (по умолчанию)"
	@echo "  examples     - Собрать только примеры"
	@echo "  tests        - Собрать только тесты"
	@echo "  run-examples - Запустить примеры"
	@echo "  run-tests    - Запустить тесты"
	@echo "  run          - Запустить тесты и примеры"
	@echo "  clean        - Очистить собранные файлы"
	@echo "  format       - Форматировать код (требует clang-format)"
	@echo "  check        - Статический анализ (требует cppcheck)"
	@echo "  debug        - Отладочная сборка"
	@echo "  release      - Релизная сборка"
	@echo "  help         - Показать эту справку"

# Указываем, что это не файлы, а команды
.PHONY: all run-examples run-tests run clean format check debug release help
