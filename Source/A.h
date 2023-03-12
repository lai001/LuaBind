#pragma once
#include <memory>
#include <string>

#include "B.h"

struct A : public std::enable_shared_from_this<A>
{
public:
	A();

	A(int data, int prop);

	~A();

	std::string mf_const(double const a, int b) const;

	std::string mf1(double const a, int b);

	void mf2(double const a, int b);

	virtual std::string getName() const;

	int data;

	int prop;

	B* b;
};
