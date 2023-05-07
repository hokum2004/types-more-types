val R = 6371e3

def toRad(x: Double): Double =
    x * Math.PI / 180

def haversin(x: Double): Double =
    (1 - Math.cos(x)) / 2

def ahaversin(x: Double): Double =
    Math.asin(Math.sqrt(x)) * 2

case class Point(lat: Double, lon: Double)

def distance(p1: Point, p2: Point): Double =
    val lat1 = toRad(p1.lat)
    val lat2 = toRad(p2.lat)

    val lon1 = toRad(p1.lon)
    val lon2 = toRad(p2.lon)

    val d = haversin(lat2 - lat1) + Math.cos(lat1) * Math.cos(lat2) * haversin(lon2 - lon1)

    val c = ahaversin(d)

    return c * R

val p1 = Point(40.123, -73.456)
val p2 = Point(-30.456, 60.123)

distance(p1, p2) / 1000.0

val p3 = Point(40.123, 73.456)
val p4 = Point(30.456, 60.123)

distance(p3, p4) / 1000.0