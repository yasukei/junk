package commander

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
func (cmdr *Commander) AddCommand(c Command) {
	cmdr.commands = append(cmdr.commands, c)
}

func (cmdr *Commander) split(command string) (name string, arg string) {
	delim := " "
	command = strings.TrimSpace(command)
	i := strings.Index(command, delim)
	if i == -1 {
		name = command
		arg = ""
	} else {
		name = command[:i]
		arg = strings.TrimSpace(command[i:])
	}
	return name, arg
}

func (cmdr *Commander) command(name string, arg string) string {
	for _, c := range cmdr.commands {
		if strings.EqualFold(c.Name(), name) {
			return c.Exec(arg)
		}
	}
	return "Command not found: " + name
}

// Execute executes Command managed by the Commander
func (cmdr *Commander) Execute(command string) string {
	name, arg := cmdr.split(command)
	return cmdr.command(name, arg)
}

// New create the instance of Commander
func New() *Commander {
	return &Commander{commands: make([]Command, 0)}
}
