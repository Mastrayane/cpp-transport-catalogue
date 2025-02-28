# cpp-transport-catalogue
Финальный проект: транспортный справочник

### Сборка под Windows

```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug
cmake --build . --config Release
```

Бинарные файлы будут скопированы в bin/Debug и bin/Release

### Сборка под Linux

```bash
mkdir -p build && cd build
# Для Debug:
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
# Для Release:
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

Протестируйте создание svg файла
```bash
# Для Debug:
cd .. && ./bin/Debug/transport_catalogue < opentest/test.json
# Для Release
cd .. && ./bin/Debug/transport_catalogue < opentest/test.json
```

Запустите сформировавшийся в корневом каталоге проекта файл map.svg
