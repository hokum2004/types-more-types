import cats.data.*
import cats.syntax.all.given
import cats.instances.all.given

trait ValidatedNewType[Raw]:
  /** Validation checks whether type can be constructed or not. It returns None
    * if it can be otherwise returns text description of error.
    */
  type Validation = Raw => Option[String]

  opaque type Type = Raw

  private def make(v: Raw): Type = v

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

  extension (t: Type) protected def toRaw(): Raw = t

end ValidatedNewType

trait Degree:
  self: ValidatedNewType[Double] =>
  extension (t: Type) def toRad(): Double = t.toRaw() * Math.PI / 180

object Latitude extends ValidatedNewType[Double] with Degree {
  addValidations(
    v => if v < -85 then Some("latitude must be greater than -85") else None,
    v => if v > 85 then Some("latitude must be less than 85") else None
  )
}
type Latitude = Latitude.Type

object Longitude extends ValidatedNewType[Double] with Degree {
  addValidations(
    v => if v < -85 then Some("longitude must be greater than -85") else None,
    v => if v > 85 then Some("longitude must be less than 85") else None
  )
}
type Longitude = Longitude.Type

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

  return ahaversin(d) * R

val d1 = for {
  p1 <- NewPoint(40.123, -73.456)
  p2 <- NewPoint(-30.456, 60.123)
} yield distance(p1, p2) / 1000.0

println(d1)

val d2 = for {
  p1 <- NewPoint(140.123, -273.456)
  p2 <- NewPoint(-130.456, 260.123)
} yield distance(p1, p2) / 1000.0

println(d2)
