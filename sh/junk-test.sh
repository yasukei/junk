#!/bin/sh

# load test target
. ./junk.sh

test_isContainedInArray_return_true()
{
	assertTrue "isContainedInArray bbb bbb"
	assertTrue "isContainedInArray bbb aaa bbb"
	assertTrue "isContainedInArray bbb aaa bbb ccc"
}

test_isContainedInArray_return_false()
{
	assertFalse "isContainedInArray bbb"
	assertFalse "isContainedInArray bbb bb"
	assertFalse "isContainedInArray bbb bbbb"
	assertFalse "isContainedInArray bbb BBB"
	assertFalse "isContainedInArray bbb aaa ccc"
}

# load shuit2
. ~/bin/shunit2/src/shunit2
