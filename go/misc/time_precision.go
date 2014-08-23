package main

import (
	"fmt"
	"sync"
	"time"
)

const numof_sample = 1000
const interval_millisec = 1

func main() {
	times := make([]time.Time, numof_sample)
	tick := time.Tick(interval_millisec * time.Millisecond)
	var wg sync.WaitGroup

	wg.Add(1)
	go func() {
		for i := 0; i < len(times); i++ {
			times[i] = <-tick
		}
		wg.Done()
	}()
	wg.Wait()

	ave := int64(0)
	for i := 1; i < len(times); i++ {
		diff := times[i].UnixNano() - times[i-1].UnixNano()
		ave += diff
		fmt.Printf("tick interval: %6d [us]\n", diff/1000)
	}
	ave /= int64(len(times) - 1)
	fmt.Printf("ave: %6d [us]\n", ave/1000)
}
