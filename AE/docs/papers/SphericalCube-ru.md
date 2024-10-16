**Сферический куб**

## Где используется

* Рисование планет.
* Вывод кубических текстур (skybox) и видео 360/VR.


## Виды проекций

В статье [Cube-to-sphere Projections for Procedural Texturing and Beyond](https://www.jcgt.org/published/0007/02/01/paper.pdf) исследованы различные виды проекций:

![](img/SC_Proj.png)

На картинке цветом обозначена площадь треугольника, более равномерный цвет характеризует равномерную площадь треугольников на сфере.<br/>
[Код](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/SphericalCube-1.as).

Проекции 5th Poly и COBE в шейдере дают наименьшую точность расчетов. В тесте берется развертка сферы и применяются обратная и прямая проекции, сравниваются вектора и выводится погрешность.<br/>
[Код](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/tests/CubeMapTest-1.as).


## Проекция текстуры

![](img/SC_TexProj.png)

В tangential проекции текстурные координаты такие же как в identity, но за счет трансформации вершин текстура накладывается более равномерно.<br/>
Если использовать координаты вершин как текстурные координаты, то искажения текстуры соответствуют identity проекции.

При применении проекции, кроме identity, текстурные координаты перестают соответствовать 3D вектору, тогда при записи в текстуру требуется применять проекцию.<br/>
Для всех проекций UV координаты грани куба перестают быть прямоугольными, то есть все формы искажаются.<br/>
UV куба дает распределение,  близкое к равномерному. При этом чем лучше проекция, тем равномернее результат.<br/>
[Код](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/SphericalCube-2.as).


## Топология

![](img/SC_Topology.png)

Обычная сетка после проецирования дает сильно искаженные треугольники, поэтому сетка должна быть симметричной относительно центра грани.


## Проекция из 3D

### Искажения

![](img/SC_Distortion.png)

Пример проекции сферы (круга) на сферический куб, ближе к грани куба начинаются небольшие искажения, для круга это не критично.<br/>
Проекция прямоугольника на сферический куб не имеет искажений только в центре грани куба, на краях начинаются искажения, но радиус вписанной окружности не меняется.<br/>
[Код](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/SphericalCube-3.as)


### Погрешность интерполяции

![](img/SC_ProjError.png)

Тест показывает погрешность, когда проекция применяется для вершин, а между ними происходит линейная интерполяция. За счет расхождения между проекцией и интерполяцией появляется ошибка. На картинке размер ошибки показан белым цветом, красный - когда ошибка более 1 после масштабирования.

Улучшить точность можно повторив линейную интерполяцию между контрольными точками. На картинке это вариант справа.

Код:<br/>
[коррекция в вычислительном шейдере](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/SphericalCube-4.as).<br/>
[коррекция во фрагментном шейдере](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/SphericalCube-5.as).


## Проекция из 2D

Существует 2 варианта записи данных в кубическую карту (cubemap):
1. Использовать UV координаты для каждой из граней и отдельно обрабатывать грани куба.
2. Сделать рендеринг в текстуру, где проецируется геометрия с текстурой или с UV для процедурной генерации в фрагментном шейдере.

### Рендеринг в текстуру

Проекция квадрата на границу между гранями кубической карты дает некорректные UV.

![](img/SC_RenderToTex_UVBug3D.png)

В текстуру квадрат рисуется с искажениями, чтобы после проекции в 3D сохранить пропорции, но из-за этого интерполяция UV работает некорректно.<br/>
Так выглядит грань кубической карты.

![](img/SC_RenderToTex_UVBug2D.png)

Тангенциальная проекция значительно улучшает интерполяцию UV, но граница все еще заметна.

![](img/SC_RenderToTex_UVBug_Tang.png)

Проблема решается, если на границе между гранями поставить дополнительные точки.

[Код](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/UVSphere-2.as)


## Сфера без геометрии

Когда стоит использовать геометрию:
* При высокой детализации и с картой смещений (displacement map).
* Рядом расположена другая геометрия. Например постройки на планете.
* При значительных деформациях геометрии. Например столкновение сфер.

В остальных случаях оптимальнее использовать процедурную сферу без геометрии.
Из геометрии только квадрат или шестиугольник, во фрагментном шейдере по UV расчитывается нормаль сферы в заданной точке. С дополнительными расчетами можно получить и глубину и записать в `gl_FragDepth`.
Далее идет попиксельная проекция (коррекция) текстурных координат.
Для перспективной проекции нормаль сферы нужно спроецировать, ведь в зависимости от расстояния между камерой и сферой видны разные части сферы.

Преимущества:
* Экономия памяти на геометрии.
* Геометрия при низкой детализации дает заметные углы по краям, а процедурная сфера всегда идеально круглая и со сглаживанием по краям.
* Геометрия при высокой детализации использует намного больше потоков фрагментного шейдера, так как по краям треугольников вызываются вспомогательные потоки (quad overdraw). А процедурная сфера использует с геометрией в виде шестиугольника расходует намного меньше потоков.
* Производительность в 2 раза выше даже на мобилках.

[Код](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/sphere/UVSphere-1.as)
