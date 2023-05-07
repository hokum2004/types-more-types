package geo

import (
	"fmt"
	"math"
)

type Metres float64
type Kilometres float64

type latitude float64
type longitude float64

type point struct {
	Lat latitude
	Lon longitude
}

type Point *point

const R = 6371e3 //Earth radius

const degreeToRad float64 = math.Pi / 180

func (m Metres) ToKilometres() Kilometres {
	return Kilometres(m / 1000)
}

func NewLat(v float64) (latitude, error) {
	if v < -85 || v > 85 {
		return 0, fmt.Errorf("latitude must be a value between -85 and 85, but got %v", v)
	}
	return latitude(v), nil
}

func NewLon(v float64) (longitude, error) {
	if v < -180 || v > 180 {
		return 0, fmt.Errorf("longitude must be a value between -180 and 180, but got %v", v)
	}
	return longitude(v), nil
}

func NewPoint(lat latitude, lon longitude) Point {
	return &point{
		Lat: lat,
		Lon: lon,
	}
}

// Distance returns the shortes distance, in meters, between two geo points.
// It uses Haversine formula, see https://en.wikipedia.org/wiki/Haversine_formula
func Distance(p1, p2 point) Metres {
	lat1 := toRad(p1.Lat)
	lat2 := toRad(p2.Lat)

	lon1 := toRad(p1.Lon)
	lon2 := toRad(p2.Lon)

	d := Haversin(lat2-lat1) + math.Cos(lat1)*math.Cos(lat2)*Haversin(lon2-lon1)

	c := Ahaversin(d)

	return Metres(c * R)
}

func toRad[T ~float64](degree T) float64 {
	return float64(degree) * degreeToRad
}

// Haversin returns the haversin of the radian argument x.
// See https://en.wikipedia.org/wiki/Versine#Haversine
func Haversin(x float64) float64 {
	return (1 - math.Cos(x)) / 2
}

// Ahaversin returns the archaversine (inverse haversine), in radians, of x.
func Ahaversin(x float64) float64 {
	return math.Asin(min(1, math.Sqrt(x))) * 2
}

func min(a, b float64) float64 {
	if a < b {
		return a
	}
	return b
}
