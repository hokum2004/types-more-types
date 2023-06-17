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

Кажется, что такой "оверинжениринг" не нужен, но на практике я сталкивался с тем, что различные идентификаторы, которые были представлены строками в разных функциях передавались в разном порядке и когда функции используются рядом, то легко можно опечататься и передать не в том порядке. Тесты выловят такое, а может и нет, а компилятор сможет не допустить такое. Или добавляешь в начало или середину аргументов функции новый, ну вот по смыслу его логичнее в сердину добавить. Например, аргументы являются частями составного ключа и было бы не очень удобно, если часть ключа шла бы после значений. В таком случае специализированные типы опять же помогут и в коде вставить его в нужное место.

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

С Go проще всего. Новый тип создается при помощи [type definition][2].

```go
// определили типы
type Timestamp int64
type UserId string
type SKU string

// используем
type Sale struct {
    Customer UserId
    Item     SKU
    Date     Timestamp
}
```

Отличие от варианта с псевдонимами в том, что при определении типа между именем нового типа и базовым типом нет знака равно. В целом, в Go создание новых типов можно встретить довольно часто, так как это позволяет "добавить" нужные методы к уже существующему типу, чтобы он начал соответствовать требуемому интерфейсу или просто обеспечить более удобную работу с ним.

```go
package main

type Timestamp int64

type TimestampAlias = int64

func ExpectAlias(v TimestampAlias) {}

func ExpectNewType(v Timestamp) {}

func main() {
    v := int64(1000)
    t1 := TimestampAlias(v)
    t2 := Timestamp(v)
    
    ExpectAlias(t1)   // <- OK
    ExpectAlias(v)    // <- OK
    
    ExpectNewType(t2) // <- OK
    ExpectNewType(v)  // <- Ошибка
}
```

Можно заметить, что во втором случае в `ExpectNewType` передается не просто число, а переменная с типом `int64`. Дело в том, что целочисленный литерал неявно приводится к любому типу который может быть сконструирован из него. Это целые числа, числа с плавающей точкой и типы для которых перечисленные являются базовыми (underlying). Поэтому вызов `ExpectNewType(1000)` не приведет к ошибке.

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
    using Raw = typename Tag::Type;
    explicit NewType(Raw const& value): value(value) {}
    explicit NewType(Raw && value): value(value) {}
    Raw value;
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

Уже использование таких типов, помогает улучшить читаемость кода и уменьшить ошибки вызванные передачей неправильных параметров. В момент написания нового кода вероятность подобных ошибок не очень большая, но она возрастет при расширении и рефакторинге кода. Хорошо, когда код покрыт тестами и подобные ошибки выловит разработчик до того изменения передадут в тестирование. Но ещё лучше, когда такие ошибки не пропустит компилятор.

Так же, введение нового типа позволит переопределить, например, способ конвертации в строку для вывода отладочной печати. Например, если информация о времени события хранится в Unix Time (число секунд прошедших с 1 января 1970), то смотреть в логи на числа вида `1682941151` не очень удобно. Гораздо проще понять в какой момент произошло событие, если в логе это значение будет представлено как '2023-05-01T11:39:11Z'. Введение отдельного типа повзолит это сделать. Другой пример - добавлять в лог к числу единицы измерения, чтобы было понятно о чем идет речь - градусы, радианы, метры, секунды, граммы или штуки.

Степень проработанности типов зависит от потребностей проекта. Где-то будет удобно переопределить и какие-то математические операторы, ввести функции для манипуляции такими типами, чтобы случайно не перемножить друг на дргу килограммы, например. В большинстве же случаев, достаточо просто будет определить новый тип и в случе каких-то манипуляций извлекать из него базовое значение. А бывает, что и вовсе нет нужды что-то делать кроме как писать/читать в/из хранилища данных.

## Можно ли лучше?

Всё выше перечисленное уже полезно само по себе. Но можно ли получить от типов еще больше пользы. Да, можно. Когда работаем с моделью данных, может потребоваться ввести ограничение на возможные значения. Например, если описывать треугольник, то ограничением будет, что сумма длин двух прилегающих сторон будет больше третьей оставшейся. В случае с одним значением подобные ограничения тоже могут быть. Например, географическая широта может быть ограничена значениями от -85° до 85°, а долгота от -180° до 180. Или от пользователя (или внешней системы) ожидается строка только в определенном формате.

Когда данные приходят в систему, то они проходят проверку и упомянутые ограничения, обычно, делаются на этапе валидации входных данных. Но дальше, смотря на описание структуры данных или сигнатуру функции эти ограничения теряются. И может возникнуть вопрос, а что делать если в какую-то функцию, которая работает с данными уже прошедшими валидацию, пришли невалидные данные. С одной стороны - такая ситуация должна быть исключена, с другой при невалидных данных что-нибудь может сломаться или пойти не так и заметить это может быть не просто.

Какие подходы есть к решению такой проблемы? Самое простое - ничего не делать, считаем, что данные которыми оперируют функции внутри сервиса валидны и ничего не сломается. Можно, вставить проверки, которые в случае ошибки кинут исключение, в языках такие проверки могут называться `assert` или `require`. Можно (нужно) к таким проверка добавить запись в лог, чтобы понимать что конкретно и где пошло не так и с какими данными. В случае C++ можно встретить, что такие проверки присутствуют в отладочной версии, но исключаются в релизной. Таким образом приложение тестируется с включенными проверками и если что-то пошло не так на тестовом стенде, то приложение упадет, а после тестирования считаем, что подобные проверки не нужны и они убираются. Самый трудоемкий путь - во всех функция добавить проверки и определить поведение в случае, если на вход пришли не валидные данные. Это кропотливый вариант, который сложно поддерживать и в итоге, всё может свестись к однообразным и не информативным ошибкам. А ещё, можно задать эти ограничения в самих типах. Как и в случае с треугольником, где проверка длин сторон может производиться в конструкторе типа или функции-фабрике. С типами поверх существующих можно сделать такие же проверки. И тогда, там где эти типы используются можно не опасаться за валидность данных.

Как этого добиться - зависит от языка и в предыдущем разделе была заложена основа для добавления проверок, приступим.

<spoiler title="Scala">

Из упомянутых здесь языков решение для Scala мне нравится больше всего. Его удобно расширять и использовать, и для работы с ошибками в Scala есть удобные абстракции. Решение с валидацией мало будет отличаться от создания нового типа без валидации. Так же будет вспомогательный трейт, только он будет содержать еще и шаг валидации, а при создании будет возвращаться не сам тип, а `Either`. В качестве ошибки, мне нравится использовать `NonEmptyList` из библиотеки Cats, содержащий строки с описанием ошибок. Это довольно универсальный вариант, но можно выбрать то, что подходит вам лучше.

Базовый трейт может выглядеть следующим образом:

```scala
trait ValidatedNewType[Raw]:
  /** Validation checks whether type can be constructed or not. It returns None
    * if it can be otherwise returns text description of error.
    */
  type Validation = Raw => Option[String]

  opaque type Type = Raw

  private[util] def make(v: Raw): Type = v

  private type ErrorOr[A] = ValidatedNel[String, A]

  def apply(v: Raw): Either[NonEmptyList[String], Type] =
    validations.traverse(f => f(v)).map(_ => make(v)).toEither

  def maybe(v: Raw): Option[Type] = apply(v).toOption

  protected def addValidations(vs: Validation*): Unit =
    validations ++= vs.map { f => (v: Raw) =>
      f(v) match
        case None      => ().validNel
        case Some(err) => err.invalidNel
    }

  private var validations: Vector[Raw => ErrorOr[Unit]] =
    Vector.empty

  extension (t: Type)
    protected def toRaw(): Raw = t

end ValidatedNewType
```

Трейт добавит публичные методы `apply` и `maybe` для создания инстанса типа с проверкой. Проверки добавляются в конструкторе объекта методом `addValidations`. Метод расширения toRaw, отмеченный как `protected`, позволяет в наследниках трейта получить доступ к базовому типу, что удобно для добавления различных методов расширения к новому типу.

Использование трейта выглядит так:

```scala
trait Degree:
  self: ValidatedNewType[Double] =>
  extension (t: Type) def toRad(): Double = t.toRaw() * Math.PI / 180

object Latitude extends ValidatedNewType[Double] with Degree {
  addValidations(
    v => if v <= -85 then Some("latitude must be greater than or equal to -85") else None,
    v => if v >= 85 then Some("latitude must be less than or equal to 85") else None
  )
}
type Latitude = Latitude.Type

object Longitude extends ValidatedNewType[Double] with Degree {
  addValidations(
    v => if v <= -180 then Some("longitude must be greater than or equal to -180") else None,
    v => if v >= 180 then Some("longitude must be less than or equal to 180") else None
  )
}
type Longitude = Longitude.Type
```

Здесь еще добавлен трейт `Degree`, который при подмешивании к объектам `Latitude` и `Longitude` добавляет к типам метод расширения `toRad`, возвращающий значение в радианах. Здесь это просто для демонстрации, а так, можно было сделать публичным метод `toRaw`.

Добавим класс `Point`, который будем содержать два поля долготу и широту и функцию для вычисления расстояния между двумя точками:

```scala
case class Point(lat: Latitude, lon: Longitude)

def NewPoint(lat: Double, lon: Double): Either[NonEmptyList[String], Point] =
  (Latitude(lat).toValidated, Longitude(lon).toValidated)
    .mapN(Point.apply)
    .toEither

val R = 6371e3

def haversin(x: Double): Double =
  (1 - Math.cos(x)) / 2

def ahaversin(x: Double): Double =
  Math.asin(Math.sqrt(x)) * 2

def distance(p1: Point, p2: Point): Double =
  val lat1 = p1.lat.toRad()
  val lat2 = p2.lat.toRad()

  val lon1 = p1.lon.toRad()
  val lon2 = p2.lon.toRad()

  val d = haversin(lat2 - lat1) + Math.cos(lat1) * Math.cos(lat2) * haversin(
    lon2 - lon1
  )

  val c = ahaversin(d)

  return c * R
```

И в итоге, расчет расстояния между двумя точками будет выглядеть так:

```scala
val d1 = for {
  p1 <- NewPoint(40.123, -73.456)
  p2 <- NewPoint(-30.456, 60.123)
} yield distance(p1, p2) / 1000.0

println(d1) // Right(15718.027575967817)

val d2 = for {
  p1 <- NewPoint(140.123, -273.456)
  p2 <- NewPoint(-130.456, 260.123)
} yield distance(p1, p2) / 1000.0

println(d2) // Left(NonEmptyList(latitude must be less than or equal to 85, longitude must be greater than or equal to -180))
```

</spoiler>

<spoiler title="Go">

Тут компилятор Go ничего предложить не может. Если хочется, чтобы при создании типа была обязательная валидация, то остается только написать в документации, чтобы тип создавался с использованием определенной функции и надеяться, что этому будут следовать.

Как вариант, можно сделать приватные типы и в функциях принимать их, но тогда использование таких типов будет ограничено одним пакетом. И всё равно остается некоторая возможность подставить неправильное значение, например передав константу из которой может быть сконструирован базовый тип. Но в целом, это будет работать, так как в основном валидации требуют данные приходящие из вне, которые будут представлены как переменные и автоматического приведения типов не будет.

В качестве примера, определим пакет `geo`:

```go
type Metres float64
type Kilometres float64

type latitude float64
type longitude float64

type point struct {
    Lat latitude
    Lon longitude
}

type Point = *point

func (p *point) String() string {
    return fmt.Sprintf("%v, %v", p.Lat, p.Lon)
}

func (m Metres) ToKilometres() Kilometres {
    return Kilometres(m / 1000)
}

func NewLat(v float64) (latitude, error) {
    if v <= -85 || v >= 85 {
        return 0, fmt.Errorf("latitude must be between -85 and 85, but got %v", v)
    }
    return latitude(v), nil
}

func NewLon(v float64) (longitude, error) {
    if v <= -180 || v >= 180 {
        return 0, fmt.Errorf("longitude must be between -180 and 180, but got %v", v)
    }
    return longitude(v), nil
}

func NewPoint(lat latitude, lon longitude) Point {
    return &point{
        Lat: lat,
        Lon: lon,
    }
}

// Distance returns the shortes distance, in metres, between two geo points.
func Distance(p1, p2 point) Metres {
    // compute distance ...
}
```

И для примера, будем получать координаты от пользователя и считать дистанцию между точками.

```go
func readPoint() (geo.Point, error) {
    var (
        rawLat float64
        rawLon float64
    )

    if _, err := fmt.Scanf("%f, %f", &rawLat, &rawLon); err != nil {
        return nil, err
    }

    errs := make([]error, 0, 2)

    lat, err := geo.NewLat(rawLat)
    if err != nil {
        errs = append(errs, err)
    }

    lon, err := geo.NewLon(rawLon)
    if err != nil {
        errs = append(errs, err)
    }

    if len(errs) != 0 {
        return nil, errors.Join(errs...)
    }

    return geo.NewPoint(lat, lon), nil
}

func main() {
    fmt.Print("Input start point (lat, lon): ")
    p1, err := readPoint()
    if err != nil {
        log.Fatalf("[E] reading the start point failed: %v", err)
        return
    }

    fmt.Print("Input end point (lat, lon): ")
    p2, err := readPoint()
    if err != nil {
        log.Fatalf("[E] reading the end point failed: %v", err)
        return
    }

    d := geo.Distance(*p1, *p2)

    fmt.Printf("Distance between points %v and %v is %.2f km\n", p1, p2, d.ToKilometres())
}
```

При таком подходе использовать типы `geo.latitude`, `geo.longitude` и `geo.point` вне пакета `geo` нельзя. В качестве обходного пути, можно задать экспортируемый псевдоним на указатель не экспортируемого типа, как сделано для `geo.point`. Псевдоним на указатель позволяет ссылаться на тип из внешнего кода, а так же использовать методы добавленные к типу `*geo.point`. Если бы тип `geo.Point` был бы задан как отдельный тип, а не псевдоним, то метод `String` был бы не доступен.

Почему нельзя экспортировать непосредственно `geo.point`? В данном случае, можно, так как значение по умолчанию для типа `float64` (ноль) для `geo.latitude` и `geo.longitude` являются валидными и созданный объект вызовом `geo.point{}` будет так же валидным. Но если бы это было не так, то можно было бы создать невалидный объект, чего хочется избежать.

Если хочется из вне использовать типы `geo.latitude` и `geo.longitude`, то можно сделать аналогичные псевдонимы на указатели на них.

Примеры исполнения программы:

```text
Input start point (lat, lon): 40.123, -73.456
Input end point (lat, lon): -30.456, 60.123
Distance between points 40.123, -73.456 and -30.456, 60.123 is 15718.03 km
```

```text
Input start point (lat, lon): 140.123, 270.456
[E] reading the start point failed: latitude must be between -85 and 85, but got 140.123
longitude must be between -180 and 180, but got 270.456
```

Если оперировать указателями на такие объекты, то в качестве значения можно передать `nil`, но в целом проверка на `nil` может быть дешевле, чем проверка, что переданные объект отвечает нужным требованиям, к тому же, если не делать никаких проверок - переданный указатель на `nil` проще отловить это с большей вероятностью приведет к падению не же ли к испорченным данным, что заметить и исправить может оказаться или очень сложно, а то и просто не возможно.

</spoiler>

<spoiler title="C++">

Будем развивать идею, заложенную в первой части. Так же заведем шаблонную структуру которая будет представлять основу для новых типов. Первое с чем надо определиться, так это как создавать её. Использование конструктора мне не нравится, так как в случае передачи неправильных значений в конструктор, всё, что можно сделать — это бросить исключение. Поэтому предлагаю пойти иным путем и добавить статический метод `New`, который будет возвращать или созданное значение, или ошибку.

Для того чтобы объединить возвращаемое значение и ошибку воспользуюсь типом `std::expected` из C++23. Компилятор g++ (у меня 12.2.0) уже содержит этот тип в составе стандартной библиотеке (с флагом `-std=C++2b`). Так как проверок может быть много и хочется вернуть результат всех проверок, то в качестве возвращаемой ошибки буду использовать такую структуру:

```cpp
struct MultipleErrors {
  std::string first;
  std::vector<std::string> rest;
};
```

В итоге определение типа `ValidatedNewType`, которое будет служить базой для создания новых типов будет выглядеть так:

```cpp
template <typename T>
using Result = std::expected<T, MultipleErrors>;

template <typename T>
using Validation = std::function<std::optional<std::string>(const T &)>;

template <typename Tag>
struct ValidatedNewType {
  using Type = ValidatedNewType<Tag>;
  using Raw = typename Tag::Type;
  using Result = ::Result<Type>;

  static Result New(const Raw &);

  friend std::ostream &operator<<(std::ostream &os, const Type &v) {
    return os << v.value_;
  }

private:
  explicit ValidatedNewType(const Raw &value) : value_(value) {}
  Raw value_;
};

template <typename Tag>
typename ValidatedNewType<Tag>::Result
ValidatedNewType<Tag>::New(typename ValidatedNewType<Tag>::Raw const &value) {
  std::vector<std::string> errs;
  for (Validation validation : Tag::validations) {
    if (auto err = validation(value); err) {
      errs.emplace_back(*err);
    }
  }

  if (errs.empty()) {
    return Result(Type(value));
  }

  return std::unexpected(
      MultipleErrors{errs.front(), std::vector(++errs.begin(), errs.end())});
}
```

Статический метод `New` ожидает, что тип `Tag` содержит статический набор проверок. Проверки — это функции с сигнатурой:

```cpp
std::optional<std::string> (const Tag::Type&);
```

Если проверка не удалась, то возвращается `std::optional` со строкой описывающий ошибку, в противном случае возвращается пустой `std::optional`.

Для удобства, так же добавлен `operator <<` для вывода значений в консоль.

Определим типы для долготы и широты, используя ValidatedNewType:

```cpp
struct LatitudeTag {
  using Type = double;
  static const std::vector<Validation<Type>> validations;
};

const std::vector<Validation<LatitudeTag::Type>> LatitudeTag::validations = {
    [](double v) -> std::optional<std::string> {
      return v <= -85 ? std::optional<std::string>(
                            "longitude must be greater than or equal to -85")
                      : std::optional<std::string>();
    },
    [](double v) -> std::optional<std::string> {
      return v >= 85 ? std::optional<std::string>(
                           "longitude must be less than or equal to 85")
                     : std::optional<std::string>();
    }};

using Latitude = ValidatedNewType<LatitudeTag>;

struct LongitudeTag {
  using Type = double;
  static const std::vector<Validation<Type>> validations;
};

const std::vector<Validation<LongitudeTag::Type>> LongitudeTag::validations = {
    [](double v) -> std::optional<std::string> {
      return v <= -180 ? std::optional<std::string>(
                             "longitude must be greater than or equal to -180")
                       : std::optional<std::string>();
    },
    [](double v) -> std::optional<std::string> {
      return v >= 180 ? std::optional<std::string>(
                            "longitude must be less than or equal to 180")
                      : std::optional<std::string>();
    }};

using Longitude = ValidatedNewType<LongitudeTag>;
```

Испльзовать их можно так:

```cpp
struct Point {
  Latitude lat;
  Longitude lon;
};

std::ostream &operator<<(std::ostream &os, const Point &v) {
  return os << "Point(" << v.lat << ", " << v.lon << ")";
}

Point NewPoint(Latitude lat, Longitude lon) {
  return Point{lat, lon};
}

int main() {
  auto lat1 = Latitude::New(60);
  auto lon1 = Longitude::New(40);
  auto p1 = mapn(NewPoint, lat1, lon1);

  std::cout << "lat1: " << lat1 << "\n";
  std::cout << "lon1: " << lon1 << "\n";
  std::cout << p1 << "\n";

  std::cout << "\n";

  auto lat2 = Latitude::New(-89);
  auto lon2 = Longitude::New(-181);
  auto p2 = mapn(NewPoint, lat2, lon2);

  std::cout << "lat2: " << lat2 << "\n";
  std::cout << "lon2: " << lon2 << "\n";
  std::cout << p2 << "\n";

  return 0;
}
```

Вывод программы будет таким:

```text
lat1: expected(60)
lon1: expected(40)
expected(Point(60, 40))

lat2: unexpected(longitude must be greater than or equal to -85)
lon2: unexpected(longitude must be greater than or equal to -180)
unexpected(longitude must be greater than or equal to -85; longitude must be greater than or equal to -180)
```

Дополнительно был объявлен `operator <<` для типа `std::expected`, а также, для комбинации нескольких `std::expected`, добавлена функция `mapn`, упрощенно, её сигнатура:

```cpp
template <typename R, typename Error, typename T1, typename T2>
std::expected<R, E> mapn(std::function<R(T1, T2)> &&f, std::expected<T1, E> const &v1, std::expected<T2, E> const & v2);
```

Она принимает функцию от аргументов которые "хранятся" в передаваемых `std::expected`. Если все переданные `std::expected` содержат значения, то они извлекаются и подставляются в переданную функцию. Результат оборачивается в `std::expected` и возвращается. Если хотя бы одно из переданных значений содержит ошибку, то возвращается ошибка. Полную реализацию можно найти в [репозитории][3].

</spoiler>

Когда использовать такой подход? Везде, где хочется. Есть ограничения на возможные значения, или простой тип имеет какое-то специальное смысловое значение в моделируемой области — это поводы задуматься о создании нового типа. При организации обмена данных можно встретить подходы с использованием XSD-схем или JSON-схем, для описания передаваемых данных. Часто, они содержат ограничения, и вот эти ограничения вполне можно выразить в типах. Да, это потребует некоторой дополнительной работы, но больший пласт делается единожды — определить базовый тип, добавить функции, который будут выполнять сериализацию/десериализацию основываясь на сериализации/десериалиазции нижележащих типов.

Указанные варианты реализации можно расширить, чтобы не только проводить валидацию, но и выполнять какие-то манипуляции. Примером таких манипуляций может быть экранирование символов в строке, для подстановки её в SQL запрос, хотя сейчас, это и нужно довольно редко, так как есть готовые библиотеки для построения запросов, которые берут на себя это.

Полный код примеров можно найти в [репозитории][4].

[1]: <https://docs.scala-lang.org/scala3/book/types-opaque-types.html> 'Scala: Opaque Types'
[2]: <https://go.dev/ref/spec#TypeDef> 'Go: Type Definitions'
[3]: <https://github.com/hokum2004/types-more-types/blob/main/examples/cpp/examples/geo_distance/util/expected_mapn.hpp> 'Реализация `mapn` для `std::expected`'
[4]: <https://github.com/hokum2004/types-more-types/tree/main/examples> 'Примеры'
