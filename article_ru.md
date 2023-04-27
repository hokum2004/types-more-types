# Типы, больше типов!

## Введение

В процессе работы над кодом, при описании модели данных, приходится создавать новые типы, в первую очередь используя такие ключевые слова как `class`/`struct`/`record`. Это позволяет создавать новые типы данных, которые агрегируют в себе другие типы как простые, так и составные. Все это знают и применяют. Я же предлагаю взглянуть на случаи, когда моделируемая сущность, описывается существующим типом, таким как целое число или строка.

В статье я хочу поделиться мыслями, которые привели меня к использованию специальных типов там, где часто используются встроенные типы. На написание статьи побудил релиз и относительно массовый переход на третью версию языка Scala. В частности, я говорю о новой конструкции [`opaque type`][1], которая упростила создание новых типов. Но так же приведу примеры и других на языках с которыми довелось поработать, а именно Scala, C++, Go.

## Почти type alias, но лучше

Одним из плюсов использования новых типов является повышение читабельности кода, когда мы видно, что используется не просто какое-то целое число, а это временная метка как количество микросекунд прошедших с 1 января 1970 года. Или поля `user` и `goods` не просто строковые поля, а ID пользователя и ID товара. С этой задачей вполне справляются псевдонимы для типов. Правда используются они чаще для того, чтобы дать какое-то короткое имя в ограниченное области видимости для типа с длинным именем, или для фиксации определенных параметров шаблона/дженерика.

Примеры:

<spoiler title="Scala">

```scala
// определили типы
type Timestamp = Long
type UserId = String
type SKU = String

// используем
case class Sale(
    customer: UserId,
    item: SKU,
    date: Timestamp,
)
```
</spoiler>

<spoiler title="Go">

```go
// определили типы
type Timestamp = int64
type UserId = string
type SKU = string

// используем
type Sale struct {
    Customer UserId
    Item     SKU
    Date     Timestamp
}
```

</spoiler>

<spoiler title="C++">

```cpp
// определили типы
using Timestamp = int64_t;
using UserId = string;
using SKU = string;

// используем
struct Sale {
    UserId customer;
    SKU item;
    Timestamp date;
};
```

</spoiler>

Удобство чтения - это хорошо, но компилятор, увы, никак не подскажет в случае, если вместо `UserId` передали `SKU` и наоборот, или просто строку. Похоже и с `Timestamp` - можно `Timestamp`, можно любое другое целое. Применять можно, применяют, но хочется большего. Хочется, чтобы при передаче значения типа `SKU` или строкового типа туда, где ожидается `UserId` компилятор об этом сказал, выдал бы ошибку.

Собственно тут и пригодятся упомянутые `opaque types` (непрозрачные типы). Если с псевдонимамми типов примеры были плюс-минус одинаковые, то с созданием отдельных, самостоятельных типов различий будет куда больше. Под спойлерами будет как код так и описание.

Кажется, что такой "оверинжениринг" не нужен, но на практике я сталкивался с тем, что различные идентификаторы, которы ебыли представлены строками в разных функциях передавались в разном порядке и когда функции используются рядом, то легко можно опечататься и передать не в том порядке. Тесты выловят такое, а может и нет, а компилятор сможет не допустить такое. Или добавляешь в начало или середину аргументов функции новый, ну вот по смыслу его логичнее в сердину добавить. Например, аргументы являются частями составного ключа и было бы не очень удобно, если часть ключа шла бы после значений. В таком случае специализированные типы опять же помогут и в коде вставить его в нужное место.

<spoiler title="Scala">

Как я сказал выше, тут и пригодятся `opaque type`. К объявлению псевдонима типа можно вначале дописать новое ключевое слово `opaque`. Это создаст новый тип, существовать он будет только во время компиляции, а в рантайме будет из себя представлять просто базовый тип. Таким образом никаких дополнительных затрат на него не будет, как при наследовании от `AnyVal`, где в определенных случаях обертка создавалась в рантайме.

Из нюансов, создать такой тип можно только из той области видимости где он был объявлен. Если прямо в пакете, то только из него, если внутри объекта, то только в теле или методах этого объекта. В связи с этим удобно создавать такой тип внутри вспомогательного объекта и добавлять метод `apply`. Чтобы уменьшить повторения, можно сделать вспомогательный трейт:

```scala
trait NewType[T]:
    opaque type Type = T
    def apply(v: T): Type = v
```

Тогда код из предыдущего примера будет выглядеть следующим образом:

```scala
// определили типы
object Timestamp extends NewType[Long]
type Timestamp = Timestamp.Type

object UserId extends NewType[String]
type UserId = UserId.Type

object SKU extends NewType[String]
type SKU = SKU.Type

// используем
case class Sale(
    customer: UserId,
    item: SKU,
    date: Timestamp,
)
```

И можно убедиться, что передать что-то другое туда где ожидаем новый тип нельзя.

```scala
trait NewType[T]:
    opaque type Type = T
    def apply(v: T): Type = v

object Timestamp extends NewType[Long]
type Timestamp = Timestamp.Type

type TimestampAlias = Long

def ExpectAlias(ts: TimestampAlias): String = "OK"

def ExpectNewType(ts: Timestamp): String = "OK"

val t1: TimestampAlias = 1000
val t2: Timestamp = Timestamp(1000)

ExpectAlias(t1) // <- OK
ExpectAlias(1000) // <- OK
ExpectNewType(t2) // <- OK
ExpectNewType(1000) // <- Ошибка компиляции
```

Использование вспомогательного трейта так же позволит добавить реализации необходимых given instances (реализации имплиситов в терминах новой скалы), которые будут выведены на базе given instances базовых типов.

</spoiler>

<spoiler title="Go">


</spoiler>

<spoiler title="C++">

В C++ подобных языковых конструкций нет. Но можно создать шаблонную структуру с одним полем и в качестве аргумента шаблона использовать специальную структуру тег. Тег будет представлять собой пустую структуру с типом внутри, который будет использоваться в качестве базового типа.

```cpp
template <typename Tag>
struct NewType {
    explicit NewType(typename Tag::Type value): value(value) {}
    typename Tag::Type value;
};
```

И использование будет выглядеть так:

```cpp
struct TimestampTag {
    using Type = int64_t;
};
using Timestamp = NewType<TimestampTag>;

struct UserIdTag {
    using Type = std::string;
};
using UserId = NewType<UserIdTag>;

struct SKUTag {
    using Type = std::string;
};
using SKU = NewType<SKUTag>;

struct Sale {
    UserId customer;
    SKU item;
    Timestamp date;
};
```

```cpp
#include <string>

template <typename Tag>
struct NewType {
    explicit NewType(typename Tag::Type value): value(value) {}
    typename Tag::Type value;
};

struct TimestampTag {
    using Type = int64_t;
};
using Timestamp = NewType<TimestampTag>;

using TimestampAlias = int64_t;

void ExpectAlias(TimestampAlias) {}

void ExpectNewType(Timestamp) {}

int main() {

    TimestampAlias ts1 = 1000;
    Timestamp ts2 = Timestamp(1000);

    ExpectAlias(ts1); // <- OK
    ExpectAlias(1000); // <- OK
    ExpectNewType(ts2); // <- OK
    ExpectNewType(1000); // <- Ошибка компиляции

    return 0;
}
```

</spoiler>


[1]: <https://docs.scala-lang.org/scala3/book/types-opaque-types.html> 'Opaque Types'
