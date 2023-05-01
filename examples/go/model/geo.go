package model

import "fmt"

type latitude float64

type longitude float64

type userId string

type EventPositionChanged struct {
	User userId
	Lat  latitude
	Lon  longitude
}

func NewLat(v float64) (latitude, error) {
	if v < -90 || v > 90 {
		return 0, fmt.Errorf("latitude must be a value between -90 and 90, but got %v", v)
	}
	return latitude(v), nil
}

func NewLon(v float64) (longitude, error) {
	if v < -180 || v > 180 {
		return 0, fmt.Errorf("longitude must be a value between -180 and 180, but got %v", v)
	}
	return longitude(v), nil
}

func NewUserId(v string) (userId, error) {
	if v == "" {
		return "", fmt.Errorf("user ID must be non empty string")
	}
	return userId(v), nil
}
