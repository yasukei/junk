package main

import (
	"strings"
)

// Command interface is exported for implementer of Command
type Command interface {
	Name() string
	Exec(arg string) string
}

// CommandManager is a manager class of Command
type CommandManager struct {
	commands []Command
}

// AddCommand adds Command into the CommandManager
func (cm *CommandManager) AddCommand(c Command) {
	cm.commands = append(cm.commands, c)
}

// ExecCommand executes Command managed by the CommandManager
func (cm *CommandManager) ExecCommand(input string) string {
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

// NewCommandManager create the instance of NewCommandManager
func NewCommandManager() *CommandManager {
	return &CommandManager{commands: make([]Command, 0)}
}
