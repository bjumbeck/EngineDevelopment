#pragma once

#include <complex>

template<typename T>
struct TrigonometricTraits
{
};

// Trigonometric traits: Specialization for float
template<>
struct TrigonometricTraits<float>
{
    using Type = float;

    static Type Sin(Type deg)
    {
        return std::sin(DegToRad(deg));
    }

    static Type Cos(Type deg)
    {
        return std::cos(DegToRad(deg));
    }

    static Type Tan(Type deg)
    {
        return std::tan(DegToRad(deg));
    }

    static Type ArcSin(Type value)
    {
        return RadToDeg(std::asin(value));
    }

    static Type ArcCos(Type value)
    {
        return RadToDeg(std::acos(value));
    }

    static Type ArcTan2(Type valY, Type valX)
    {
        return RadToDeg(std::atan2(valY, valX));
    }

    static Type Sqrt(Type value)
    {
        return std::sqrt(value);
    }

    static Type Pi()
    {
        return 3.141592653589793238462643383f;
    }

    static Type RadToDeg(Type rad)
    {
        return 180 / Pi() * rad;
    }

    static Type DegToRad(Type deg)
    {
        return Pi() / 180 * deg;
    }
};

// Trigonometric traits: Specialization for double
template<>
struct TrigonometricTraits<double>
{
    using Type = double;

    static Type Sin(Type deg)
    {
        return std::sin(DegToRad(deg));
    }

    static Type Cos(Type deg)
    {
        return std::cos(DegToRad(deg));
    }

    static Type Tan(Type deg)
    {
        return std::tan(DegToRad(deg));
    }

    static Type ArcSin(Type value)
    {
        return RadToDeg(std::asin(value));
    }

    static Type ArcCos(Type value)
    {
        return RadToDeg(std::acos(value));
    }

    static Type ArcTan2(Type valY, Type valX)
    {
        return RadToDeg(std::atan2(valY, valX));
    }

    static Type Sqrt(Type value)
    {
        return std::sqrt(value);
    }

    static Type Pi()
    {
        return 3.141592653589793238462643383;
    }

    static Type RadToDeg(Type rad)
    {
        return 180 / Pi() * rad;
    }

    static Type DegToRad(Type deg)
    {
        return Pi() / 180 * deg;
    }
};

// Trigonometric traits: Specialization for long double
template<>
struct TrigonometricTraits<long double>
{
    using Type = long double;

    static Type Sin(Type deg)
    {
        return std::sin(DegToRad(deg));
    }

    static Type Cos(Type deg)
    {
        return std::cos(DegToRad(deg));
    }

    static Type Tan(Type deg)
    {
        return std::tan(DegToRad(deg));
    }

    static Type ArcSin(Type value)
    {
        return RadToDeg(std::asin(value));
    }

    static Type ArcCos(Type value)
    {
        return RadToDeg(std::acos(value));
    }

    static Type ArcTan2(Type valY, Type valX)
    {
        return RadToDeg(std::atan2(valY, valX));
    }

    static Type Sqrt(Type value)
    {
        return std::sqrt(value);
    }

    static Type Pi()
    {
        return 3.141592653589793238462643383l;
    }

    static Type RadToDeg(Type rad)
    {
        return 180 / Pi() * rad;
    }

    static Type DegToRad(Type deg)
    {
        return Pi() / 180 * deg;
    }
};

template<typename T>
T ToDegree(T radian)
{
    return TrigonometricTraits<T>::RadToDeg(radian);
}

template<typename T>
T ToRadian(T degree)
{
    return TrigonometricTraits<T>::DegToRad(degree);
}