package commander

import (
	"testing"
)

func TestNew(t *testing.T) {
	cmdr := New()

	if cmdr == nil {
		t.Error("got: nil")
		t.Error("expected: not nil")
	}
}

type CommandForTest struct {
}

const (
	commandName   = "CommandName"
	commandResult = "CommandResult"
	commandHelp   = "CommandHelp"
)

func (cft *CommandForTest) Name() string {
	return commandName
}

func (cft *CommandForTest) Exec(arg string) string {
	return commandResult
}

func (cft *CommandForTest) Help(arg string, verbose bool) string {
	return commandHelp
}

func TestAddCommand(t *testing.T) {
	cmdr := New()
	cmdr.AddCommand(&CommandForTest{})

	result := cmdr.Execute(commandName)
	if result != commandResult {
		t.Error("got: " + result)
		t.Error("expected: " + commandResult)
	}
}
