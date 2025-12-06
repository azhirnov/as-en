
1.1. Nonuniform stress test v2<br/>
Разница в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Чтобы в варп попадали разные индексы используется хэш от `gl_FragCoord` с двумя режимами: квадрат 2х2 и попиксельно.<br/>
Вариант per object больше приближен к реальному использованию, тогда как per quad и per pixel это стресс-тест, но могут возникнуть: per quad для микротреугольников, per pixel в visibility buffer.<br/>
Тест сравнивает производительность разного доступа к ресурсам при низкой нагрузке на другие системы, но не показывает влияния bindless на производительность в целом.<br/>
Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/nonuniform/NonUniform-Stress.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniform-Stress-shared.as).

1.2. Nonuniform with depth pre-pass<br/>
Сделаны примитивные объекты в виде повернутых прямоугольников, вытянутые формы приводят к тому, что больше треугольников попадают в варп и сильнее проявляется неоднородность индексов.
Показывает разницу в производительности между использованием `nonuniform()` и выбором слоя из Texture2DArray.
Можно менять детализацию текстур, чтобы определить насколько bindless влияет на производительность при нормальной нагрузке на память и при пониженой, когда читаются нижние мип-уровни.<br/>
Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/nonuniform/NonUniform-DPP.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniform-shared.as).

1.3. Nonuniform with visibility buffer<br/>
Аналогично depth pre-pass, но вызывается меньше фрагментных шейдеров и больше уникальных индексов в варпе.
На слабом железе сильно нагружается ALU из-за чего нагрузка на текстуры оказалась минимальной.<br/>
Исходники: [скрипт](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/nonuniform/NonUniform-VB.as), [шейдер](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipeline_inc/NonUniform-VB-shared.as).

Все три теста оказались не достаточно информативными, хватило данных чтобы разбить ГП по группам, но сложно оценить как влияет увеличение общего количества текстур, увеличение количества чтений текстур для PBR, parallax mapping, ландшафта, где нагрузка увеличивается в 4 раза.

