#pragma once

#include <stdio.h>
#include <string>
#include <vector>

namespace iec
{

//-------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------
class Variable
{
public:
	Variable(const std::string& name) :
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

//-------------------------------------------------------------------
// Pou
//-------------------------------------------------------------------
class Pou
{
public:
	Pou(const std::string& name) :
		name_(name),
		variables_()
	{}
	virtual ~Pou() {}

	const std::string& getName() { return name_; }

protected:
	std::string name_;
	std::vector<Variable*> variables_;
};

//-------------------------------------------------------------------
// Program
//-------------------------------------------------------------------
class Program : public Pou
{
public:
	Program(const std::string& name) :
		Pou(name)
	{}
	~Program() {}
};

} // namespace iec
