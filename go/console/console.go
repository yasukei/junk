package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type Command interface {
	Name() string
	Exec(arg string) string
}

type CommandManager struct {
	commands []Command
}

func (self *CommandManager) AddCommand(c Command) {
	self.commands = append(self.commands, c)
}

func (self *CommandManager) ExecCommand(name string, arg string) string {
	for _, c := range self.commands {
		if c.Name() == name {
			return c.Exec(arg)
		}
	}
	return "Command not found: " + name
}

type EchoCommand struct {
}

func (self *EchoCommand) Name() string {
	return "Echo"
}

func (self *EchoCommand) Exec(arg string) string {
	return arg
}

func main() {
	cmdmgr := CommandManager{commands: make([]Command, 0)}

	cmdmgr.AddCommand(&EchoCommand{})

	s := bufio.NewScanner(os.Stdin)
	for s.Scan() {
		fmt.Println(s.Text())
		temp := strings.TrimSpace(s.Text())
		temps := strings.SplitN(temp, " ", 2)
		fmt.Println(cmdmgr.ExecCommand(temps[0], temps[1]))
	}
}
