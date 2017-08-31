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
)

func (cft *CommandForTest) Name() string {
	return commandName
}

func (cft *CommandForTest) Exec(arg string) string {
	return commandResult
}

func TestAddCommand(t *testing.T) {
	cmdr := New()
	cmdr.AddCommand(&CommandForTest{})

	result := cmdr.ExecCommand(commandName)
	if result != commandResult {
		t.Error("got: " + result)
		t.Error("expected: " + commandResult)
	}
}
