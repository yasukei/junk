package main

import (
	"bufio"
	"fmt"
	"os"
)

// EchoCommand is a class which implementes Command interface and does echo
type EchoCommand struct {
}

// Name is a implementation of Command.Name()
func (ec *EchoCommand) Name() string {
	return "Echo"
}

// Exec is a implementation of Command.Exec()
func (ec *EchoCommand) Exec(arg string) string {
	return arg
}

func main() {
	cmdr := NewCommander()
	cmdr.AddCommand(&EchoCommand{})

	s := bufio.NewScanner(os.Stdin)
	for s.Scan() {
		fmt.Println(cmdr.ExecCommand(s.Text()))
	}
}
