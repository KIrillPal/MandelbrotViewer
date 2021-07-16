#pragma once
#include <algorithm>
#include <cmath>
class Complex
{
public:
    Complex(double x = 0, double y = 0);
    ~Complex();
    Complex(Complex & other);
    Complex(Complex && other);
    Complex & operator =(Complex other);
    Complex operator +(Complex other);
    Complex & operator +=(Complex other);
    Complex operator -(Complex other);
    Complex & operator -=(Complex other);
    Complex operator *(Complex other);
    Complex & operator *=(Complex other);
    friend void swap(Complex &a, Complex &b);
	friend double abs(Complex& a);
	friend double abs2(Complex &a);
    operator double();
private:
    double x_ = 0, y_ = 0;
};

