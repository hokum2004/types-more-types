trait NewType[T]:
    opaque type Type = T
    def apply(v: T): Type = v

object Timestamp extends NewType[Long]
type Timestamp = Timestamp.Type

type TimestampAlias = Long

val t1: TimestampAlias = 1000 // <- OK

val t2: Timestamp = Timestamp(1000) // <- OK

val t3: Timestamp = 1000 // <- Ошибка при копиляции