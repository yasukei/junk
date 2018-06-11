#pragma once

#include <stdio.h>
#include <string>

class Variable
{
public:
	Variable(const char* name) :
		name_(name), value_(0.0)
	{}
	~Variable() {}

	void setValue(double value) { value_ = value; }
	double getValue() { return value_; }

	void display(FILE* fp)
	{
		// TODO: change FILE* to stream
		fprintf(fp, "name:  [%s]\n", name_.c_str());
		fprintf(fp, "value: [%lf]\n", value_);
	}

protected:
	std::string name_;
	double value_;
};

