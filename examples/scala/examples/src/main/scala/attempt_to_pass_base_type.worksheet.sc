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