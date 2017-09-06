package main

import (
	"fmt"
	"sync"
)

func main() {
	num := 1000
	var wg sync.WaitGroup
	wg.Add(num)
	for i := 0; i < num; i++ {
		go func(i int) {
			fmt.Printf("hello %d\n", i)
			wg.Done()
		}(i)
	}

	wg.Wait()
}
