package main

import (
	"fmt"
	"net/http"
)

func handler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hi there!\n")
	fmt.Fprintf(w, "Host:          [%s]\n", r.Host)
	fmt.Fprintf(w, "RemoteAddr:    [%s]\n", r.RemoteAddr)
	fmt.Fprintf(w, "RequestURI:    [%s]\n", r.RequestURI)
	fmt.Fprintf(w, "Method:        [%s]\n", r.Method)
	fmt.Fprintf(w, "Protocol:      [%s]\n", r.Proto)
	fmt.Fprintf(w, "Header:        [%s]\n", r.Header)
	fmt.Fprintf(w, "Body:          [%s]\n", r.Body)
	fmt.Fprintf(w, "ContentLength: [%d]\n", r.ContentLength)
	//fmt.Fprintf(w, "Hi there, I love %s!", r.URL.Path[1:])
}

func main() {
	http.HandleFunc("/", handler)
	http.ListenAndServe(":8080", nil)
}
