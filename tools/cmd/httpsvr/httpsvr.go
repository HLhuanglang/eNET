package main

import (
	"fmt"
	"net/http"
)

func main() {
	http.HandleFunc("/Hi", func(w http.ResponseWriter, r *http.Request) {
		fmt.Printf("Request: %v\n", r)
		fmt.Fprint(w, "hello, world")
	})

	http.ListenAndServe(":8888", nil)
}
