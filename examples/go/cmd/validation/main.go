package main

import (
	"example/model"
	"fmt"
	"log"
)

var eventQueue chan model.EventPositionChanged

func UserChangedPosition(uID string, lat float64, lon float64) {
	//event := model.EventPositionChanged{User: uID, Lat: lat, Lon: lon} // <- Ошибка компиляции

	user, err := model.NewUserId(uID)
	if err != nil {
		log.Printf("[E] %v\n", err)
		return
	}

	vLat, err := model.NewLat(lat)
	if err != nil {
		log.Printf("[E] %v\n", err)
		return
	}

	vLon, err := model.NewLon(lon)
	if err != nil {
		log.Printf("[E] %v\n", err)
		return
	}

	event := model.EventPositionChanged{User: user, Lat: vLat, Lon: vLon}

	eventQueue <- event
}

func main() {
	eventQueue = make(chan model.EventPositionChanged, 100)
	defer close(eventQueue)

	badEvent1 := model.EventPositionChanged{}
	badEvent2 := model.EventPositionChanged{User: "", Lat: 190, Lon: 270}
	fmt.Printf("badEvent1: %v\n", badEvent1)
	fmt.Printf("badEvent2: %v\n", badEvent2)

	UserChangedPosition("", 190, 270)

	UserChangedPosition("#1", 190, 270)

	UserChangedPosition("#2", 80, 270)
}
