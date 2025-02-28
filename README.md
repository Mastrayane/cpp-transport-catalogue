# Transport Catalogue

## (Транспортный справочник)
Проект представляет собой транспортный справочник, который позволяет:
- Хранить информацию об остановках и маршрутах общественного транспорта.
- Рассчитывать оптимальные маршруты между остановками.
- Визуализировать карту маршрутов в формате SVG.
- Обрабатывать запросы на получение информации о маршрутах и остановках.

### Функциональность:
- Добавление остановок и маршрутов через JSON-файл.
- Расчет статистики по маршрутам (количество остановок, уникальных остановок, длина маршрута, кривизна).
- Построение оптимальных маршрутов между остановками с учетом времени ожидания и скорости транспорта.
- Генерация карты маршрутов в формате SVG с настраиваемыми параметрами (цвета, размеры, шрифты).

### Цель проекта:
Проект был создан для отработки навыков работы с:
- **C++17** (стандартные контейнеры, алгоритмы, работа с файлами).
- **CMake** (сборка проекта).
- **JSON** (парсинг и генерация данных).
- **SVG** (визуализация данных).
- **Графовые алгоритмы** (расчет маршрутов).

### Стек технологий:
- Язык программирования: **C++17**
- Сборка: **CMake**
- Формат данных: **JSON**
- Визуализация: **SVG**
- Алгоритмы: **Графовые алгоритмы (Dijkstra)**
---
## Инструкция по развёртыванию и системные требования

### Системные требования:
- Операционная система: **Windows**, **Linux**, **macOS**
- Компилятор: **GCC** (версия 9 и выше) или **MSVC** (Visual Studio 2022)
- CMake: **3.16 и выше**
- Стандарт языка: **C++17**

## Сборка
### Сборка под Windows

```bash
mkdir build && cd build && cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
cmake --build . --config Release
```

### Сборка под Linux

```bash
mkdir -p build && cd build
# Для Debug:
cmake .. -DCMAKE_BUILD_TYPE=Debug && cmake --build .
# Для Release:
cmake .. -DCMAKE_BUILD_TYPE=Release && cmake --build .
```
Бинарные файлы будут скопированы в bin/Debug и bin/Release


Протестируйте создание svg файла:
```bash
cd ..
# Для Debug:
./bin/Debug/transport_catalogue < opentest/test.json
# Для Release
./bin/Debug/transport_catalogue < opentest/test.json
```

Запустите сформировавшийся в корневом каталоге проекта файл map.svg в любом из браузеров или графическом редакторе

# Схема написания входного JSON-файла для транспортного справочника
Этот документ описывает структуру входного JSON-файла, который используется для настройки и заполнения транспортного справочника, а также для запроса информации и визуализации карты маршрутов.
## 1. Основные разделы JSON-файла
Входной JSON-файл состоит из четырех основных разделов:

1. **```base_requests```** — данные для заполнения справочника (остановки и маршруты).
2. **```render_settings```** — настройки визуализации карты.
3. **```routing_settings```** — настройки маршрутизации.
4. **```stat_requests```** — запросы на получение информации.

##  2. Раздел ```base_requests```
Содержит данные об остановках и маршрутах. Это массив объектов, где каждый объект описывает либо остановку, либо маршрут.

### 2.1. Остановка (Stop)
```bash
{
    "type": "Stop",
    "name": "Название остановки",
    "latitude": широта,
    "longitude": долгота,
    "road_distances": {
        "Название соседней остановки 1": расстояние в метрах,
        "Название соседней остановки 2": расстояние в метрах
    }
}
```
* **```type```**: Всегда ```"Stop"```.

* **```name```**: Уникальное название остановки.

* **```latitude```**, ```longitude```: Координаты остановки (широта и долгота).

* **```road_distances```**: Словарь, где ключи — названия соседних остановок, а значения — расстояния до них в метрах.

 ### 2.2. Маршрут (Bus)
```bash
 {
    "type": "Bus",
    "name": "Номер маршрута",
    "stops": ["Остановка 1", "Остановка 2", "Остановка 3"],
    "is_roundtrip": true/false
}
```
* **```type```**: Всегда ```"Bus"```.
* **```name```**: Уникальный номер маршрута.
* ***```stops```**: Массив названий остановок, через которые проходит маршрут.
* **```is_roundtrip```**: ```true```, если маршрут кольцевой, ```false```, если линейный.

  ### 3. Раздел ```render_settings```
  Содержит настройки для визуализации карты маршрутов.
  *(Все кроме последней пока выставлены поумолчанию. **Пока вписывать только "color_palette"!** Доработать возможность использования и по умолчанию, и самостоятельной настроки.)*
```bash
{
    "width": ширина холста,
    "height": высота холста,
    "padding": отступ от краев,
    "stop_radius": радиус круга остановки,
    "line_width": ширина линии маршрута,
    "bus_label_font_size": размер шрифта номера маршрута,
    "bus_label_offset": [смещение по X, смещение по Y],
    "stop_label_font_size": размер шрифта названия остановки,
    "stop_label_offset": [смещение по X, смещение по Y],
    "underlayer_color": цвет подложки,
    "underlayer_width": толщина подложки,
    "color_palette": ["цвет 1", "цвет 2", "цвет 3"]
}
```
* Цвета могут быть заданы в форматах:
  * Строка: ```"green"```, ```"red"```, ```"#FF0000"```.
  * RGB: ```[255, 0, 0]```.
  * RGBA: ```[255, 0, 0, 0.5]```.

### 4. Раздел ```routing_settings```
Содержит настройки для расчета маршрутов.
```bash
{
    "bus_wait_time": время ожидания на остановке (в минутах),
    "bus_velocity": скорость автобуса (в км/ч)
}
```
### 5. Раздел ```stat_requests```
Содержит запросы на получение информации. Это массив объектов, где каждый объект описывает запрос.
#### 5.1. Запрос информации о маршруте (Bus)
```bash
{
    "type": "Bus",
    "name": "Номер маршрута",
    "id": уникальный идентификатор запроса
}
```
#### 5.2. Запрос информации об остановке (Stop)
```bash
{
    "type": "Stop",
    "name": "Название остановки",
    "id": уникальный идентификатор запроса
}
```
#### 5.3. Запрос визуализации карты (Map)
```bash
{
    "type": "Map",
    "id": уникальный идентификатор запроса
}
```
#### 5.4. Запрос маршрута между остановками (Route)
```bash
{
    "type": "Route",
    "from": "Название начальной остановки",
    "to": "Название конечной остановки",
    "id": уникальный идентификатор запроса
}
```
### 6. Пример полного JSON-файла
```bash
{
    "base_requests": [
        {
            "type": "Stop",
            "name": "Stop 1",
            "latitude": 55.611087,
            "longitude": 37.208290,
            "road_distances": {"Stop 2": 500}
        },
        {
            "type": "Bus",
            "name": "Bus 1",
            "stops": ["Stop 1", "Stop 2"],
            "is_roundtrip": false
        }
    ],
    "render_settings": {
        "width": 1200,
        "height": 800,
        "padding": 50,
        "stop_radius": 5,
        "line_width": 14,
        "bus_label_font_size": 20,
        "bus_label_offset": [7, 15],
        "stop_label_font_size": 14,
        "stop_label_offset": [7, -3],
        "underlayer_color": "white",
        "underlayer_width": 3,
        "color_palette": ["green", "red", "blue"]
    },
    "routing_settings": {
        "bus_wait_time": 2,
        "bus_velocity": 40
    },
    "stat_requests": [
        {"type": "Bus", "name": "Bus 1", "id": 1},
        {"type": "Map", "id": 2}
    ]
}
```
### 7. Рекомендации по заполнению
### Остановки:

* *Убедитесь, что все остановки, упомянутые в маршрутах, описаны в разделе base_requests.*
* *Координаты должны быть в диапазоне: широта от -90 до 90, долгота от -180 до 180.*

### Маршруты:

* *Для кольцевых маршрутов (is_roundtrip: true) последняя остановка должна совпадать с первой.*
* *Для линейных маршрутов (is_roundtrip: false) маршрут будет "туда и обратно".*

### Цвета:

* *Используйте яркие цвета для лучшей визуализации.*
* *Убедитесь, что количество цветов в палитре не меньше количества маршрутов.*

### Запросы:

* *Уникальные id для каждого запроса.*
* *Запросы выполняются в порядке их указания.*
-- --
Эта схема поможет вам корректно заполнить входной JSON-файл для работы с транспортным справочником.
## Планы по доработке
### Текущие задачи:
- Рефакторинг кода:

- Улучшить структуру проекта, разделив код на более мелкие модули.

- Добавить unit-тесты для проверки корректности работы алгоритмов.

### Расширение функциональности:

- Добавить поддержку нескольких типов транспорта (автобусы, трамваи, троллейбусы).

- Реализовать возможность редактирования данных через консольный интерфейс.

### Оптимизация:

- Ускорить расчет маршрутов за счет кэширования результатов.

- Уменьшить использование памяти за счет оптимизации структур данных.

### Документация:

- Добавить подробное описание API для работы с библиотекой.

- Создать примеры использования для разработчиков.
