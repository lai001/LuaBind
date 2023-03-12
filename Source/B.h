#pragma once
#include <memory>

struct B : public std::enable_shared_from_this<B>
{
    B();
    ~B();

    int data;
};
