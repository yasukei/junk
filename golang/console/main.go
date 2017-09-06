package main

import (
	"bufio"
	"fmt"
	"os"

	"./commander"
)

// EchoCommand is a class which implementes Command interface and does echo
type EchoCommand struct {
}

// Name is a implementation of Command.Name()
func (ec *EchoCommand) Name() string {
	return "echo"
}

// Exec is a implementation of Command.Exec()
func (ec *EchoCommand) Exec(arg string) string {
	return arg
}

// Help is a implementation of Command.Help()
func (ec *EchoCommand) Help(arg string, verbose bool) string {
	return "echoes its argument"
}

func main() {
	cmdr := commander.New()
	cmdr.AddCommand(&EchoCommand{})

	s := bufio.NewScanner(os.Stdin)
	console := func() bool { fmt.Print("\nconsole:>"); return true }
	for console() && s.Scan() {
		fmt.Println(cmdr.Execute(s.Text()))
	}
}
