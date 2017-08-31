package main

import (
	"strings"
)

// Command interface is exported for implementer of Command
type Command interface {
	Name() string
	Exec(arg string) string
}

// Commander is a manager class of Command
type Commander struct {
	commands []Command
}

// AddCommand adds Command into the Commander
func (cm *Commander) AddCommand(c Command) {
	cm.commands = append(cm.commands, c)
}

// ExecCommand executes Command managed by the Commander
func (cm *Commander) ExecCommand(input string) string {
	var name, arg string
	input = strings.TrimSpace(input)
	i := strings.Index(input, " ")
	if i == -1 {
		name = input
		arg = ""
	} else {
		name = input[:i]
		arg = strings.TrimSpace(input[i:])
	}

	for _, c := range cm.commands {
		if strings.EqualFold(c.Name(), name) {
			return c.Exec(arg)
		}
	}
	return "Command not found: " + name
}

// NewCommander create the instance of NewCommander
func NewCommander() *Commander {
	return &Commander{commands: make([]Command, 0)}
}
