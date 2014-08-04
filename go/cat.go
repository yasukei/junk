package main

import (
	"./file"
	"flag"
	"fmt"
	"os"
)

var rot13Flag = flag.Bool("rot13", false, "rot13 the input")

func cat(r reader) {
	const NBUF = 512
	var buf [NBUF]byte

	if *rot13Flag {
		r = newRotate13(r)
	}
	for {
		switch nr, er := r.Read(buf[:]); true {
		case nr < 0:
			fmt.Fprintf(os.Stderr, "cat: error reading from %s: %s\n", r.String(), er)
			os.Exit(1)
		case nr == 0:
			return
		case nr > 0:
			if nw, ew := file.Stdout.Write(buf[0:nr]); nw != nr {
				fmt.Fprintf(os.Stderr, "cat: error writing from %s: %s\n", r.String(), ew)
				os.Exit(1)
			}
		}
	}
}

func main() {
	flag.Parse()
	if flag.NArg() == 0 {
		cat(file.Stdin)
	}
	for i := 0; i < flag.NArg(); i++ {
		f, err := file.Open(flag.Arg(i))
		if f == nil {
			fmt.Fprintf(os.Stderr, "cat: can't open %s: error %s\n", flag.Arg(i), err)
			os.Exit(i)
		}
		cat(f)
		f.Close()
	}
}

type reader interface {
	Read(b []byte) (ret int, err error)
	String() string
}

type rotate13 struct {
	source reader
}

func newRotate13(source reader) *rotate13 {
	return &rotate13{source}
}

func rot13(s byte) byte {
	return s + 13
}

func (r13 *rotate13) Read(b []byte) (ret int, err error) {
	ret, err = r13.source.Read(b)
	for i := 0; i < ret; i++ {
		b[i] = rot13(b[i])
	}
	return ret, err
}

func (r13 *rotate13) String() string {
	return r13.source.String()
}
