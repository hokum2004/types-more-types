package main

import (
	"example/geo"
	"fmt"
	"log"
)

func readPoint() (geo.Point, error) {
	var (
		rawLat float64
		rawLon float64
	)

	if _, err := fmt.Scanf("%f, %f", &rawLat, &rawLon); err != nil {
		return nil, err
	}

	lat, err := geo.NewLat(rawLat)
	if err != nil {
		return nil, nil
	}

	lon, err := geo.NewLon(rawLon)
	if err != nil {
		return nil, nil
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
