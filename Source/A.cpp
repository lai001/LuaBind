#include "A.h"
#include <spdlog/spdlog.h>
#include "Base.hpp"
#include "RegisterHelper.h"

A::A()
    : data(0), prop(0), b(nullptr)
{
}

A::A(int data, int prop)
    : data(data), prop(prop), b(nullptr)
{
}

A::~A()
{
}

std::string A::mf_const(double const a, int b) const
{
    return std::string("std::string A::mf_const(double const a, int b) const");
}

std::string A::mf1(double const a, int b)
{
    return std::string("std::string A::mf1(double const a, int b)");
}

void A::mf2(double const a, int b)
{
    spdlog::debug("void A::mf2(double const a, int b)");
}

std::string A::getName() const
{
    return std::string("A");
}