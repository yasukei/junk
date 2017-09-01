package commander

import (
	"strings"
)

// Command interface is exported for implementer of Command
type Command interface {
	Name() string
	Exec(arg string) string
	Help(arg string, verbose bool) string
}

// Commander is a manager class of Command
type Commander struct {
	commands []Command
}

const (
	commandNotFound = "Command not found: "
)

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

func (cmdr *Commander) findCommand(name string) Command {
	for _, c := range cmdr.commands {
		if strings.EqualFold(c.Name(), name) {
			return c
		}
	}
	return nil
}

func (cmdr *Commander) command(name string, arg string) string {
	c := cmdr.findCommand(name)
	if c == nil {
		return commandNotFound + name
	}
	return c.Exec(arg)
}

// Execute executes Command managed by the Commander
func (cmdr *Commander) Execute(command string) string {
	name, arg := cmdr.split(command)
	return cmdr.command(name, arg)
}

// Name is a implementation of Command.Name()
func (cmdr *Commander) Name() string {
	return "help"
}

func (cmdr *Commander) getListofAllCommands() string {
	ret := ""
	for _, c := range cmdr.commands {
		ret += c.Name() + "\t\t" + c.Help("", false) + "\n"
	}
	return ret
}

// Exec is a implementation of Command.Exec()
func (cmdr *Commander) Exec(arg string) string {
	if arg == "" {
		return cmdr.getListofAllCommands()
	}

	name, arg := cmdr.split(arg)
	c := cmdr.findCommand(name)
	if c == nil {
		return commandNotFound + name
	}
	return c.Help(arg, true)
}

// Help is a implementation of Command.Help()
func (cmdr *Commander) Help(arg string, verbose bool) string {
	return "display a list of all commands"
}

// New create the instance of Commander
func New() *Commander {
	cmdr := &Commander{commands: make([]Command, 0)}
	cmdr.AddCommand(cmdr)
	return cmdr
}
