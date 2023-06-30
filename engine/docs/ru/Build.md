Как собрать проект.

## Кэширование зависимостей

Этот этап можно пропустить.

Для стабильности и безопасности лучше склонировать все сторонние зависимости на свой git-сервер и далее использовать его. Локальный git-сервер значительно ускорит загрузку зависимостей.

Скрипт `clone_3party.bat` загружает все зависимости через сеть и сохраняет в локальный git, запускать его необходимо на сервере.


## Сборка зависимостей

Зайти в папку `AE/engine/external` и выбрать скрипт с соответствующей конфигурацией ОС и компилятора.

Linux: для запуска скрипта необходимо добавить разрешение на запуск через chmod +x <имя_файла>.

Далее будет долгий процесс компиляции зависимостей, но это сэкономит время при следующих сборках.


## Сборка проекта

Зайти в папку `build_scripts` и выбрать скрипт с соответствующей конфигурацией ОС и компилятора.

Произойдет настройка cmake, далее в папке `_build` окажется проект под VS/XCode/makefile и тд.


## Компиляция ресурсов

Проект `PACK_RES` скомпилирует компиляторы шейдеров и конвертеры ресурсов, а затем выполнит конвертацию ресурсов, используемых в примерах к движку.

Далее можно компилировать и запускать тесты и примеры.


## Обновление путей в документации

Для портативности пути заданы в виде `file:///<path>`, этот путь надо заменить на корректный абсолютный путь в виде `C:/...`.


### Тесты

В тестах используется сравнение результата рендеринга с предыдущими запусками, результат может незначительно отличаться между различными устройствами.
В тестах с использованием Vulkan API происходит логирование вызова команд и сравнение с предыдущими запусками, результат на разных устройствах может отличаться из-за разного выравнивания и наличия разных расширений и фич.
Перечисленные выше тесты нужны для регресс тестов при разработке и могут дать ложное срабатывание при первом запуске на новом устройстве.
