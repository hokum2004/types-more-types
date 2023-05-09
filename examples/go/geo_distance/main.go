package main

import (
	"errors"
	"fmt"

	"example/geo"
)

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
		fmt.Printf("[E] reading the start point failed: %v\n", err)
		return
	}

	fmt.Print("Input end point (lat, lon): ")
	p2, err := readPoint()
	if err != nil {
		fmt.Printf("[E] reading the end point failed: %v\n", err)
		return
	}

	d := geo.Distance(*p1, *p2)

	fmt.Printf("Distance between points %v and %v is %.2f km\n", p1, p2, d.ToKilometres())
}
