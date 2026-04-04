Содержание:
* [Типы](#Типы)
* [Шаблоны](#Шаблоны)
* [enable_if](#enable_if)
* [Концепты](#Концепты)
* [Constexpr](#Constexpr)
* [Аттрибуты](#Аттрибуты)
* [constinit](#constinit)
* [source_location](#source_location)
* [stacktrace](#stacktrace)
* [Variadic templates](#Variadic-templates)
* [using](#using)
* [Structured binding](#Structured-binding)
* [template export](#Template-export)
* [Макросы](#Макросы)
* [Контейнеры](#Контейнеры)
* [Время жизни объекта](#Время-жизни-объекта)
* [Модули](#Модули)
* [Корутины](#Корутины)


## Типы

### Указатели на поля и методы класса

```cpp
template <typename Class, typename Value>
void foo (Value Class:::* member);

template <typename Class, typename Result, typename ...Args>
void foo (Result (Class::*method) (Args...));

template <typename Class, typename Result, typename ...Args>
void foo (Result (Class::*method) (Args...) const);
```

### Ссылка на массив

```cpp
template <typename T, size_t Count>
void foo (T (&arr)[Count]);
```

### Подсказка для вывода типа

[Class template argument deduction (C++17)](https://en.cppreference.com/w/cpp/language/class_template_argument_deduction.html)

```cpp
template <typename T>
struct ArrayView;

template <typename T, size_t S>
ArrayView (const T (&)[S]) -> ArrayView<T>;

int arr[100];
ArrayView view{arr};
```

Без подсказки компилятор выведет тип `Array<int[100}>` и это будет один элемент в view, а с подсказкой тип изменится на `ArrayView<int>` и view будет содержать массив из 100 элементов.

### Перегрузка методов

```cpp
Builder&  add() &  { return *this; }
Builder&& add() && { return std::move(*this); }
```

Одна из распространненных ошибок - сохранять ссылку на временный объект:

```cpp
auto ref = Builder{}.add();
ref.add();  // use after free
```

Чтобы не допускать таких ошибок требуется делать перегрузку `add() &&`, тогда `auto` будет иметь тип `Builder`, а не `Builder &` и ошибки не возникнет.

Здесь `std::move(*this)` только меняет тип ссылки, перемещение не происходит.

Аналогично в `Synchronized` типе сделана защита от получения ссылки на внутренний объект, когда время жизни обертки не совпадает с временем жизни ссылки, что приводит к незащищенному доступу:

```cpp
template <typename SyncObj, typename T>
class Synchronized_Ptr
{
    Synchronized_Ptr (SyncObj &sync, T &ref) : _sync{sync}, _ref{ref} { _sync.lock(); }
    ~Synchronized_Ptr ()   { _sync.unlock(); }
    T&  operator * ()      { return _ref; }  // <<<
};

Synchronized< Mutex, Array<int> >  sync;
Array<int>& arr = *sync.Ptr(); // lock(), operator*, unlock()
arr.emplace_back(); // unprotected access!
```

### Неявная конвертация в bool

С этим связано множество проблем, начиная с `operator bool`:

```
struct ValueAndError
{
private:
  int value;
  bool isOK;

public:
  operator bool() const { return isOK; }
  int Unwrap() { return value; }
};

ValueAndError  getValue();

int value = getValue(); // cast bool to int
```

Но эта проблема легко отлавливается если настроить ошибки компиляции, а еще лучше **всегда** использовать только `explicit operator bool`.

Другие ошибки сложнее отловить, например `if (x)` позволяет использовать и `bool` тип и `int` и указатели, поэтому такой код компилируется и предумпреждение никогда не выведется:

```
assert("error");
static_assert("error");
```

Но после улучшения и с настроенными ошибками компиляции, такой код больше не скомпилируется:

```
#define Assert( _expr_ )  assert(bool{_expr_})
#define StaticAssertMsg( _expr_, _msg_ ) static_assert(bool{_expr_}, _msg_)

Assert("error");  // compilation error
StaticAssertMsg(false, "error"); // ok
```

## Шаблоны

### Специализация

Определяется базовый шаблон `Type<T>`, который принимает 1 параметр.
Затем добавляется специализация под сложные типы, например указатели на функции.
Далее можно использовать `Type<>` для любых типов без необходимости передавать все аргументы шаблона.

```cpp
template <typename T>
struct Type;

template <typename Ret, typename ...Args>
struct Type< Ret (*) (Args...) > {};

int Foo (float, double, bool);

using T = Type< decltype(&Foo) >;
```


## enable_if

Давно известный способ подсказать компилятору какой шаблон нужно использовать.

```cpp
template <typename T>
enable_if_t<is_floating_point_v<T>, bool>  IsNaN (T x);
```

Другая форма записи:

```cpp
template <typename T, enable_if_t<is_floating_point_v<T>, bool> = true>
bool  IsNaN (T x);
```


## Концепты

В C++20 добавили более мощный инструмент для замены `enable_if`.

Подробно разобрано в серии статей на [Modern C++](https://www.modernescpp.com/index.php/tag/concepts/)

### Включаем/выключаем методы

Простой пример

```cpp
template <typename T>
  requires( is_base_of< T, Class >)
explicit operator T ();

template <typename T>
explicit operator T () requires is_base_of< T, Class >;
```

`requires` работает только с шаблонами, но не обязательно явно задавать типы для шаблона.<br/>
Разрешаем неявную конвертацию к базовому типу и требуем явную конвертацию ко всем остальным типам.

```cpp
template <typename T>
struct ClassA
{
    template <typename B = T>
      requires( B::allowImplicitCast )
    operator T ();  // implicit cast

    template <typename B = T>
      requires( not B::allowImplicitCast )
    explicit operator T ();  // explicit cast
};
```

### Проверяем интерфейс

```cpp
template <typename T>
concept IsCoroutine = requires()
{
    (typename T::promise_type{}).initial_suspend();
    (typename T::promise_type{}).final_suspend();

    (typename T::promise_type{}).unhandled_exception();

    (typename T::promise_type{}).get_return_object();
    T::promise_type::get_return_object_on_allocation_failure();
};
```

Используется несколькими способами:

```cpp
static_assert( IsCoroutine< T >);

template <typename T>
  requires( IsCoroutine<T> )
void foo (T coro);

template <IsCoroutine T>
void foo (T coro);
```

Одно из преимуществ концептов в том, что они позволяют использовать несуществующие типы, поля и методы.
Тогда как проверка старым способов не скомпилируется из-за несуществующего типа `promise_type`.

```cpp
template <typename T>
static constexpr bool  IsDefer = is_same<
        decltype(&T::promise_type::initial_suspend),
        suspend_always (T::*)() >;
```


### Работает внутри `if constexpr` и `static_assert`

```cpp
template <typename T>
void foo ()
{
    if constexpr( requires{ typename T::type; })
    {}

    static_assert( requires{ typename T::type; });
}
```

Вариант с `static_assert` удобно использовать для юнит-тестов.

### Проверка возвращаемого типа

```cpp
template <typename T1, typename T2>
concept Add = requires(T1 a, T2 b)
{
    {a + b} -> same_as<T1>;
};
```

Здесь [same_as](https://en.cppreference.com/w/cpp/concepts/same_as) это новый концепт аналогичный `is_same<>`.<br/>
Конструкция `{A} -> B` интерпретируется как `decltype((A))` и получаем `is_same< decltype((A)), B >`.

Но проверка поля класса работает немного иначе:

```cpp
template <typename T>
concept HasIntM = requires(T obj)
{
    {obj.m} -> same_as< int >;
};

template <typename T>
concept HasIntRefM = requires(T obj)
{
    {obj.m} -> same_as< int& >;
};

struct Class {
   int m;
};
HasIntM< Class >;     // false
HasIntRefM< Class >;  // true
```

В C++23 для этого добавили конструкцию `{auto{ A }}` (decay-copy):

```cpp
template <typename T>
concept HasIntM = requires(T obj)
{
    {auto{ obj.m }} -> same_as< int >;
};
```

Но это в целом неправильный подход для проверки типа поля класса:

```cpp
struct ClassA { int       m; };
struct ClassB { const int m; };
struct ClassC { int&      m; };

HasIntM< ClassA >;  // true
HasIntM< ClassB >;  // true
HasIntM< ClassC >;  // true
```

Правильный способ:

```cpp
template <typename T>
concept HasIntM = requires
{
    { &T::m } -> same_as< int T::* >;
};
```

### Сочетаем `requires` с другими концептами

```cpp
template <typename T>
concept IsInt = is_integral<T> and
                requires(T a, T b) { {a += b} -> same_as<T&>; };
```

### Проверяем компиляцию

Также можно использовать для проверки кода который не должен скомпилироваться.
Здесь `Ptr()` возвращает объект у которого есть только `operator * () &`, а в данном выражении требуется `operator * () &&`, поэтому код не компилируется.

```cpp
template <typename T>
concept PtrDereference = requires (T obj)
{
    *obj.Ptr();  // must be compile-time error
};

Synchronized< Mutex, Object >  obj;
static_assert( not PtrDereference< decltype(obj) >);
```

Но настройки компилятора не влияют на проверку компиляции:

```cpp
#pragma warning(error: 834) // discarding return value of function with [[nodiscard]] attribute

template <typename T>
concept NoDiscard = requires(T a, T b)
{
    a + b;	// discarding return value
};

static_assert( NoDiscard<int2> );  // ok
```

А вот `static_assert` внутри функции внутри `requires{}` блока будет срабатывать всегда и ломать компиляцию.

```cpp
template <typename T>
void foo ()
{
    static_assert( is_integer<T> );
}

requires{ foo<float>(); };  // compilation error
```

Но компиляция выполняется только частично.
Здесь концепт вернет `true`, но код не скомпилируется.

```cpp
template <typename T>
void foo (T value)
{
    foo2( std::move(value) );
}

template <typename T>
concept FooCompatible = requires { foo( T{} ); };

static_assert( FooCompatible< Noncopyable >);
foo( T{} );  // error: move-ctor marked as delete
```

### Проверяем существует ли шаблон

```cpp
template <typename T, typename ...Args>
concept IsCallOperatorSpecialization = requires
{
    &T::template operator()< Args... >;
};
```

### Включаем битовые операции для enum типа

Вариант с определением функции

```cpp
enum class Enum
{
    Flag1 = 1 << 0,
    Flag2 = 1 << 2,
};
constexpr bool EnableBitOperators(Enum);

template <typename T>
concept AllowBitOperators = is_enum_v<T> and requires(T e){ EnableBitOperators(e); };

template <AllowBitOperators T>
constexpr T  operator | (T lhs, T rhs);
```

Добавляем флаг в enum по которому будем включать операторы.

```cpp
enum class Enum
{
    Flag1 = 1 << 0,
    Flag2 = 1 << 2,
    _BITOPS_
};

template <typename T>
concept AllowBitOperators = is_enum_v<T> and requires{ T::_BITOPS_ };
```

Может возникнуть проблема с пространствами имен.
Если операторы определены в одном пространсве, а используются в другом, в котором уже есть глобальные битовые операторы, то компилятор их не найдет, нужно добавить подсказку с `using EnumBitOperatorsNS::operator |;`.

## Constexpr

### is_constant_evaluated

Одна из возможных ошибок написать так:

```cpp
if constexpr( is_constant_evaluated() )
```

Такой код всегда вернет `true`. А правильно:

```cpp
if ( is_constant_evaluated() )
```

При выполнении на этапе компиляции `if` и так будет работать как `if constexpr`.

Чтобы не допускать подобных ошибок я вынес все в макросы:

```
#define if_consteval()       if ( is_constant_evaluated() )
#define if_not_consteval()   if ( not is_constant_evaluated() )
```

### is constexpr

[Пример с StackOverflow](https://stackoverflow.com/questions/55288555/c-check-if-statement-can-be-evaluated-constexpr).
Шаблон позволяет определить может ли функция выполниться на этапе компиляции.
Это удобно для использования в `if constexpr` и в `static_assert` для юнит-тестов.
Внутрь блока `[]{ ... }` можно помещать не одну функцию, а произвольный код.

```cpp
template<class Lambda, int=(Lambda{}(), 0)>
constexpr bool is_constexpr(Lambda) { return true; }
constexpr bool is_constexpr(...) { return false; }

if constexpr( is_constexpr([]{ base::get_data(); }))
{}

static_assert( is_constexpr([]{ base::get_data(); }));
```

Другой вариант через передачу результата функции в шаблон, стало меньше кода, но и меньше функционала.

```cpp
template<auto> struct require_constant;
template<class T>
concept has_constexpr_data = requires { typename require_constant<T::get_data()>; }
```


## Аттрибуты

### Force inline calls

[`[[msvc::forceinline_calls]]`](https://learn.microsoft.com/en-us/cpp/cpp/attributes?view=msvc-170#msvcforceinline_calls)<br/>
[`[[clang::always_inline]]`](https://clang.llvm.org/docs/AttributeReference.html#always-inline-force-inline)

Использование:

```cpp
[[msvc::forceinline_calls]]
{
    foo();  // inline
    bar();  // inline
}

[[msvc::forceinline_calls]]
foo();  // inline
```

### inline all

[`[[msvc::flatten]]`](https://learn.microsoft.com/en-us/cpp/cpp/attributes?view=msvc-170#msvcflatten)<br/>
[`[[clang::flatten]]`](https://clang.llvm.org/docs/AttributeReference.html#flatten)<br/>
`__attribute__((flatten))` - GCC, только для функции

Рекурсивно встраивает все функции внутри блока.

```cpp
[[msvc::flatten]]
{
    foo();
}

[[msvc::flatten]] void MyFn()
{
    foo();
}
```

Чтобы встроить и функцию и все функции, которая она вызывает пишут:

```cpp
[[msvc::forceinline]] [[msvc::flatten]] void MyFn()
{
    foo();
}

__inline__ __attribute__((__always_inline__)) [[clang::flatten]] void MyFn()
{
    foo();
}
```

### Likely/Unlikely

`[[unlikely]]` аттрибут подсказывает компилятору, что вероятность пойти по этому пути низкая, поэтому компилятор перемещает код в конец функции.
Когда выполнение идет по вероятному пути, то инструкции расположены последовательно.
### Без внешних зависимостей

`[[gnu::const]]` или `__attribute__((const))`<br/>
Аттрибутом помечается функция, которая не имеет внешних зависимостей:
* нет чтения/записи глобальной памяти
* операции идут только над аргументами
* нет обращений по ссылке или указателю.

Это позволяет компилятору лучше оптимизировать код.<br/>
Пример функции: abs().

`[[gnu::pure]]` или `__attribute__((pure))` или `__declspec(noalias)`<br/>
В функции происходят операции только с аргументами. На аргументы могут влиять значения глобальных переменных.
Разрешено чтение/запись по указателю, но только один уровень косвенного обращения.<br/>
Это позволяет компилятору выкинуть повторные вызовы функции с одинаковыми аргументами, предполагая, что глобальные переменные не изменились, значит и результат будет такой же.<br/>
Пример функции: strlen().


## constinit

Требует `constexpr` конструктор.
В C++20 добавили `__cpp_lib_constexpr_string` и `__cpp_lib_constexpr_vector`, но в MSVC с дебажными итераторами это не работает, что делает функционал не очень пригодным к использованию.


## source_location

В отличие от старых `__FILE__` и `__LINE__` позволяет получить информацию о месте вызова функции:

```cpp
void foo (const source_location &loc = source_location::current());

foo();  // __FILE__, __LINE__ берется здесь
```

## stacktrace

Появился в C++23 и позволяет получить полный стэк вызова, но работает медленно, поэтому пригоден только для сообщений об ошибках.

При использовании вместе с RenderDoc стэк вызова становится нечитаемым, возможно сторонние тулзы что-то перехватывают и сами получают стэк.


## Variadic templates

[Parameter pack](https://cppreference.com/w/cpp/language/parameter_pack.html)<br/>
[Fold expressions (C++17)](https://cppreference.com/w/cpp/language/fold.html)<br/>
[Pack indexing (C++26)](https://cppreference.com/w/cpp/language/pack_indexing.html)

Использование:

```cpp
template <typename T>
struct Class;

template <typename ...Types>
struct Class< tuple< Types... >>;

template <typename ...Args>
void foo (Args&& ...args)
{
    bar( forward<Args>(args) ... );
}

template <typename ...Types1, typename ...Types2>
void foo (tuple<Types1> a, tuple<Types2> b);
```

Количество аргументов

```cpp
template <typename ...Args>
void foo (Args&& ...args)
{
    static_assert( sizeof...(Args) > 1 );
}
```

Тип аргумента

```cpp
template <typename ...Args>
void foo (Args&& ...args)
{
    using T1 = typename tuple_element< 1, tuple< Args... >>::type;
}
```

Рекурсия

```cpp
template <typename Arg0, typename ...Args>
void foo (Arg0&& arg0, Args&& ...args)
{
    bar( arg0 );
    if constexor( sizeof...(Args) > 0 )
        foo( forward<Args>(args) ... );
}
```

Операции с аргументами (C++17)

```cpp
f( ++args ... );       // f( ++args[0], ++args[1] )
f( &args ..., argN );  // f( &args[0], &args[1], argN )
f( arg0 + args ... );  // f( arg0 + args[0], arg0 + args[1] )
f( ... and args );     // f<bool>( args[0] and args[1] )
f( args + ... + 1 );   // f( args[0] + args[1] + 1 )
```

Using-declarations (C++17)

```cpp
template<typename... bases>
struct X : bases...
{
    using bases::g...;
};
X<B, D> x;  // using B::g;  using D::g;
```

Получить первый и последний аргумент.
Оператор запятая пропустит все аргументы и вернет самый последний.

```cpp
template <typename Arg0, typename ...Args>
decltype(auto)  First (Arg0 &&arg0, Args&& ...args)  { return forward<Arg0>(arg0); }

template <typename ...Args>
decltype(auto)  Last (Args&& ...args)  { return (forward<Args>(args), ... ); }
```

Pack indexing (C++26)

```cpp
consteval auto first_plus_last (auto... args)
{
    return args...[0] + args...[sizeof...(args) - 1];
}
```

### Подсмотрено в [Nifty Fold Expression Tricks](https://www.foonathan.net/2020/05/fold-tricks/)

Вызвать функцию для каждого элемента в обратном порядке.

```cpp
int dummy;
(dummy = ... = (f(ts), 0));
// expands to: dummy = ((f(ts[0]), 0) = (f(ts[1]), 0)) = ...
```

Вызвать функцию пока не сработало условие

```cpp
((pred(ts) ? false : (f(ts), true)) && ...);
// expands to: (pred(ts[0]) ? false : (f(ts[0]), true))
//              && (pred(ts[1]) ? false : (f(ts[1]), true))
//              && ...
```

Найти первый элемент удовлетворяющий условию

```cpp
std::common_type_t<decltype(ts)...> result;
bool found = ((pred(ts) ? (result = ts, true) : false) || ...);
// expands to: (pred(ts[0]) ? (result = ts[0], true) : false)
//          || (pred(ts[1]) ? (result = ts[1], true) : false)
//          || ...
```

## using

### Изменение уровня доступа для класса

```cpp
struct AllPublic
{
    int x;
    int y;
    void Foo();
};

class Wrapper : private AllPublic
{
public:
    using AllPublic::x;
    using AllPublic::Foo();
};

Wrapper w;
w.x = 0;  // OK
w.y = 0;  // error
```


## Template export

Позволяет спрятать методы шаблона в cpp файл.

Важный нюанс - MSVC позволяет объевить `template class` до реализации всех методов, а clang - нет.

```cpp
// .h
template <typename T>
struct Templ
{
	void foo();
};

extern template class Templ< int >;

// .cpp
template <typename T>
void Templ<T>::foo()
{}

template class Templ< int >;
```
## Модули

### Архитектура

Файл '.cppm' это интерфейс модуля, в нем следует объявлять экспортную секцию `export module <name>;` и `export import module <name>;`.

Также в интерфейсе может быть неэкспортируемая часть, начинается она с `module : private;`.

Другой вариант - вынести реализацию в '.cpp' файл. Для одного интерфейса модуля может быть несколько '.cpp' файлов с реализацией.

Все вызовы `#include` должны быть до начала модуля и только в глобальном модуле (`module;`).
Вместо `#include` можно использовать `import "<name>";` тогда модуль импортируется вместе с макросами.

Но макросы до `import ""` не влияют на компиляцию того, что импортируется, в этом большое отличие от `#include`.


### Как подключить заголовки по-старинке

Интерфейс модуля (.cppm).

```cpp
module;

#define MY_MACRO
#include "OldHeader"

export module MyFirstModule;

import "NewModule";

export
{
    using OldNamespace::Foo;  // public
}

export namespace NewNamespace
{
    void Foo2 ();  // public

    using namespace OldNamespace;  // private
}

export namespace NewNamespace2 = OldNamespace;  // public
```

Реализация модуля (.cpp).

```cpp
import MyFirstModule;

// макрос не экспортируется
#ifdef MY_MACRO
# error has MY_MACRO
#endif

namespace NewNamespace
{
    // нужно добавлять в каждый модуль
    using namespace OldNamespace;

    // export
    void Foo2 ()
    {
        // impl
    }
}
```

Здесь `#include` объявлен в глобальной секции и все типы, функции, пространства имен будут видны всем.

А макрос, объявленный рядом, не будет виден за пределами модуля, аналогично и с макросами внутри `#include`.
Макросы могут быть использованы только чтобы задать поведение препроцессора внутри `#include` и текущего модуля.

Единственный способ экспортировать пространство имен это через пвсевдоним:
```
export namespace NewNamespace = OldNamespace;
```
Вложенные пространства имен также можно экспортировать:
```
export namespace NewNS {
    export namespace Base = OldNS::Base;  // public
}
```
