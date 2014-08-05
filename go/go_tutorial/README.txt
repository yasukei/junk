Files in this directory are based on Go Tutorial.
http://golang.jp/go_tutorial

hello.go
	$ go run hello.go

echo.go
	$ go tool 8g echo.go
	$ go tool 8l -o echo echo.8
	$ ./echo abc
	$ ./echo -n abc

helloworld3.go & file.go
	$ go tool 8g file.go
	$ go tool 8g helloworld3.go
	$ go tool 8l -o helloworld3 helloworld.8
	$ ./helloworld3

cat.go & file.go
	$ go tool 8g file.go
	$ go tool 8g cat.go
	$ go tool 8l -o cat cat.8
	$ ./cat README.txt
