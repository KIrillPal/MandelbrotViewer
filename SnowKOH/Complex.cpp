#include "Complex.h"


Complex::Complex(double x, double y) : x_(x), y_(y)
{

}

Complex::~Complex()
{
}

Complex::Complex(Complex & other) : x_(other.x_), y_(other.y_)
{}

Complex::Complex(Complex && other)
{
    x_ = std::exchange(other.x_, 0);
    y_ = std::exchange(other.y_, 0);
}

Complex & Complex::operator=(Complex other)
{
    swap(*this, other);
    return *this;
}

Complex Complex::operator+(Complex other)
{
    other.x_ += x_;
    other.y_ += y_;
    return other;
}

Complex & Complex::operator+=(Complex other)
{
    x_ += other.x_;
    y_ += other.y_;
    return *this;
}

Complex Complex::operator-(Complex other)
{
    other.x_ -= x_;
    other.y_ -= y_;
    return other;
}

Complex & Complex::operator-=(Complex other)
{
    x_ -= other.x_;
    y_ -= other.y_;
    return *this;
}

Complex Complex::operator*(Complex other)
{
    return Complex(x_ * other.x_ - y_ * other.y_, x_ * other.y_ + y_ * other.x_);
}

Complex & Complex::operator*=(Complex other)
{
    *this = *this * other;
    return *this;
}

Complex::operator double()
{
    return x_;
}

void swap(Complex & a, Complex & b)
{
    std::swap(a.x_, b.x_);
    std::swap(a.y_, b.y_);
}

double abs(Complex & a)
{
    return sqrt(a.x_ * a.x_ + a.y_ * a.y_);
}

double abs2(Complex& a)
{
	return a.x_ * a.x_ + a.y_ * a.y_;

}
