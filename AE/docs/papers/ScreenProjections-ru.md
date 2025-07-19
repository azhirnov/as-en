**Виды проекций на экран.**


## Прямолинейная проекция (Rectilinear)

Она же перспективная проекция.

Плюсы:
* Прямые линии остаются прямыми
* Совместима с матрицами проекции

Минусы:
* Чем больше FOV тем больше искажения по краям, так как часть сферы проецируется на плоскость.
* Из-за искажений плотность пикселей в центре меньше, чем по краям. Это создает проблемы при изменении проекции пост-процессом.

![](img/Proj_Rectilinear-1.jpg)
![](img/Proj_Rectilinear-2.jpg)
![](img/Proj_Rectilinear-3.jpg)

## Стереографическая проекция (Stereographical)

Вектор в 3D конвертируется в сферические координаты и отображается на плоскости.

Плюсы:
* Угловое расстояние не искажается.
* Форма не искажается.
* Хорошо подходит для рисования сфер, звездного неба.

Минусы:
* Прямые линии искривляются, из-за чего тяжело смотреть на прямоугольные формы.
* Некомфортно смотреть в динамике.
* При fovY>120° начинаются искажения на полюсах, но для ультраширокого монитора fov={360°, 101°}.

![](img/Proj_Stereographical-1.jpg)
![](img/Proj_Stereographical-2.jpg)
![](img/Proj_Stereographical-3.jpg)

## Panini

Стереографическая проекция, где камера смещена назад. Смещение задается от 0 до 1.0, для больших углов можно зафиксировать 1.0, смещение 0 совпадает с перспективной проекцией.

Плюсы:
* Наиболее комфортно воспринимается в динамике.
* Искажения менее заметные.
* Вертикальные линии остаются прямыми.

Минусы:
* Угловое расстояние искажается.
* Горизонтальные линии немного искажаются.
* Максимальный угол 180°.

![](img/Proj_Panini-1.jpg)
![](img/Proj_Panini-2.jpg)
![](img/Proj_Panini-3.jpg)


## Особенности проекций на 180°

Проекция на 180° потребует изменений в рендеринге:
* Делается через рисование в 3 камеры по 45° и пост-процессом с коррекцией на стаках.
* Билборды на стыках будут искажаться, поэтому их лучше рисовать в мировом пространсве, а не в экранном.
* Каскадные тени (CSM) придется переделать под что-то похожее на GeoClipMap.
* SSR и прочие экранные техники нужно дорабатывать, чтобы не было артефактов на границе между камерами, либо заменить на другие техники, например трассировку.


## Примеры

* [Panini](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-posteffects/Panini.as) - сцена рисуется с перспективной проекцией, затем применяется пост-процесс с Panini проекцией.
* [RenderToCubemap](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts/samples-3d/RenderToCubemap.as) - сцена рисуется в кубическую карту, затем нужный тексель выбирается по 3D координатам, аналогично трассировке лучей.

## Ссылки

* [Comparing Graphical Projection Methods at High Degrees of Field of View](https://www.diva-portal.org/smash/get/diva2:1229190/FULLTEXT02.pdf) - сравнивают какая проекция наиболее комфортно воспринимается.
* [Panini Projection in UE](https://dev.epicgames.com/documentation/en-us/unreal-engine/panini-projection-in-unreal-engine) - Panini как пост-процесс, работает на углах до примерно 120, после плотность пикселей в центре слишком мала.
* [Pannini: A New Projection for Rendering Wide Angle Perspective Images](http://tksharpless.net/vedutismo/Pannini/panini.pdf) - оригинальная статься про Panini проекцию.
* [RayTracingGems2: Essential Ray Generation Shaders](https://www.researchgate.net/publication/354065227_Essential_Ray_Generation_Shaders) - сравнивают разные проекции, есть код для рейтрейса.
* [Reducing stretch in high-FOV games using barrel distortion](https://www.decarpentier.nl/lens-distortion) - другой способ компенсации искажений через пост-процесс.
* [Lens Matched Shading](https://developer.nvidia.com/lens-matched-shading-and-unreal-engine-4-integration-part-1) - компенсация искажения для VR через multiview - рисование в 4 текстуры.
