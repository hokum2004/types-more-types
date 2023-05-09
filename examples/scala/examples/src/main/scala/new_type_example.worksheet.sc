trait NewType[T]:
    opaque type Type = T
    def apply(v: T): Type = v

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
