﻿Виртульная файловая система


## Хранилище (IVirtualFileStorage)

Привязывает директорию на диске, архив и тд к виртуальной системе.

Хранилище:
 * Может быть статическим или динамическим.
 * Потокобезопасно для виртуальных методов (интерфейса).
 * Может быть непотокобезопасным при инициализации и в деструкторе.
 * Поддерживает директорию на диске, архив со сжатыми файлами, сетевой интерфейс.


## Файлы

Поддерживаются:
 * Поток для последовательного чтения (RStream)
 * Файл со случайным доступом на чтение (RDataSource)
 * Файл с асинхронным чтением со случайным доступом (AsyncRDataSource)

Потокобезопасность зависит от реализации интерфейса и проверяется через `IDataSource::IsThreadSafe()`.


## Имя файла (FileName)

Используется хэш от строки, чтобы не было динамических выделений памяти.

Уникальность хэша гарантированна для архива.<br/>
Остальные типы хранилищ проверяют уникальность хэша только в режиме отладки.


## Файловая система (VirtualFileSystem)

 * Хранит список файловых хранилищ в порядке их добавления.
 * Позволяет искать и открывать файл по имени.
 * Содержит оптимизации под статичные хранилища для быстрого поиска.
