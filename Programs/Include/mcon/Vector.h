/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <string> // std::min
#include <cmath> // std::sqrt

#include "debug.h" // ASSERT()

#include "VectordBase.h"

namespace mcon {

template <typename Type> class Matrix;

#define MCON_ITERATION(var, iter, statement)    \
    do {                                        \
        for (size_t var = 0; var < iter; ++var) \
        {                                       \
            statement;                          \
        }                                       \
    } while(0)

template <typename Type>
class Vector
{
public:

    explicit Vector(const int length = 0);
    Vector(const Vector<Type>& v);
    Vector(const VectordBase& v);
    template <typename U> Vector(const Vector<U>& v);
    ~Vector();

    // For const object
    const Type& operator[](const size_t i) const
    {
        ASSERT (i < m_Length);
        return m_Address[i];
    }
    // For non-const object
    Type& operator[](const size_t i)
    {
        ASSERT (i < m_Length);
        return m_Address[i];
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const Vector<Type>& Copy(const Vector<Type>& vector);
    // operator= make the same vector as the input vector.
    Vector<Type>& operator=(const Vector<Type>& vector);

    template <typename U>
    operator Vector<U>() const
    {
        Vector<U> v(GetLength());
        MCON_ITERATION(i, m_Length, v[i] = static_cast<U>((*this)[i]));
        return v;
    }
    // This cast doesn't seem called... Why?
    // Are default ones already defined and called?
    operator Vector<Type>() const
    {
        ASSERT(0);
        Vector<Type> v(GetLength());
        MCON_ITERATION(i, m_Length, v[i] = (*this)[i]);
        return v;
    }

    operator void*() const
    {
        return reinterpret_cast<void*>(m_Address);
    }

    Vector<Type> operator()(size_t offset, size_t length) const
    {
        Vector<Type> carveout;
        if (GetLength() <= offset)
        {
            // Null object.
            return carveout;
        }
        // Smaller value as length
        carveout.Resize( std::min(GetLength() - offset, length) );
        for (size_t i = offset; i < Smaller(offset + length); ++i)
        {
            carveout[i-offset] = (*this)[i];
        }
        return carveout;
    }

    Type Fifo(Type v)
    {
        Type ret = (*this)[0];
        for (size_t i = 0; i < GetLength() - 1; ++i)
        {
            (*this)[i] = (*this)[i+1];
        }
        (*this)[GetLength()-1] = v;
        return ret;
    }

    Type Unshift(Type v)
    {
        Type ret = (*this)[GetLength()-1];
        for (size_t i = GetLength() - 1; i > 0; --i)
        {
            (*this)[i] = (*this)[i-1];
        }
        (*this)[0] = v;
        return ret;
    }

    Vector<Type>& operator=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] = v); return *this; };
    Vector<Type>& operator=(const VectordBase& v);

    const Vector<Type> operator+(Type v) const { Vector<Type> vec(*this);  vec += v; return vec; }
    const Vector<Type> operator-(Type v) const { Vector<Type> vec(*this);  vec -= v; return vec; }
    const Vector<Type> operator*(Type v) const { Vector<Type> vec(*this);  vec *= v; return vec; }
    const Vector<Type> operator/(Type v) const { Vector<Type> vec(*this);  vec /= v; return vec; }

    const Vector<Type> operator+(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec += v; return vec; }
    const Vector<Type> operator-(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec -= v; return vec; }
    const Vector<Type> operator*(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec *= v; return vec; }
    const Vector<Type> operator/(const Vector<Type>& v) const { Vector<Type> vec(*this);  vec /= v; return vec; }

    Vector<Type>& operator+=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] += v); return *this; }
    Vector<Type>& operator-=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] -= v); return *this; }
    Vector<Type>& operator*=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] *= v); return *this; }
    Vector<Type>& operator/=(Type v) { MCON_ITERATION(i, m_Length, (*this)[i] /= v); return *this; }

    Vector<Type>& operator+=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] += v[i]); return *this; }
    Vector<Type>& operator-=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] -= v[i]); return *this; }
    Vector<Type>& operator*=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] *= v[i]); return *this; }
    Vector<Type>& operator/=(const Vector<Type>& v) { MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] /= v[i]); return *this; }

    void Initialize(int offset = 0, int step = 1)
    {
        for (size_t k = 0; k < GetLength(); ++k )
        {
            (*this)[k] = offset + step * k;
        }
    }

    void Initialize( Type (*initializer)(size_t, size_t) )
    {
        for (size_t k = 0; k < GetLength(); ++k )
        {
            (*this)[k] = initializer(k, GetLength());
        }
    }

    inline Matrix<Type> T(void) const { return Transpose(); }
    Matrix<Type> Transpose(void) const
    {
        Matrix<Type> m(GetLength(), 1);
        MCON_ITERATION(i, GetLength(), m[i][0] = (*this)[i]);
        return m;
    }
    Matrix<Type> ToMatrix(void) const
    {
        Matrix<Type> m(1, GetLength());
        m[0] = *this;
        return m;
    }

    inline Type GetMaximum(void) const
    {
        Type max = (*this)[0];
        for (size_t i = 1; i < GetLength(); ++i)
        {
            if (max < (*this)[i])
            {
                max = (*this)[i];
            }
        }
        return max;
    }

    inline Type GetMaximumAbsolute(void) const
    {
        Type max = Absolute((*this)[0]);
        for (size_t i = 1; i < GetLength(); ++i)
        {
            const Type v = Absolute((*this)[i]);
            if (max < v)
            {
                max = v;
            }
        }
        return max;
    }

    inline Type GetMinimum(void) const
    {
        Type min = (*this)[0];
        for (size_t i = 1; i < GetLength(); ++i)
        {
            if (min > (*this)[i])
            {
                min = (*this)[i];
            }
        }
        return min;
    }

    inline Type GetMinimumAbsolute(void) const
    {
        Type min = Absolute((*this)[0]);
        for (size_t i = 1; i < GetLength(); ++i)
        {
            const Type v = Absolute((*this)[i]);
            if (min > v)
            {
                min = v;
            }
        }
        return min;
    }


    inline Type GetSum(void) const
    {
        Type sum = 0;
        MCON_ITERATION( i, GetLength(), sum += (*this)[i] );
        return sum;
    }

    inline double GetAverage(void) const
    {
        return GetSum()/GetLength();
    }

    inline double GetNorm(void) const
    {
        double squareSum = 0;
        MCON_ITERATION( i, GetLength(), squareSum += (*this)[i] * (*this)[i]);
        return std::sqrt(squareSum);
    }
    inline double GetDotProduct(const Vector<Type>& v) const
    {
        double dot = 0;
        MCON_ITERATION( i, GetLength(), dot += (*this)[i] * v[i]);
        return dot;
    }

    size_t GetLength(void) const { return m_Length; }
    bool IsNull(void) const { return m_Length == 0; }
    bool Resize(size_t length);

private:
    // Private member functions.
    size_t   Smaller(size_t input) const { return GetLength() < input ? GetLength() : input; }
    void     Allocate(void);
    Type     Absolute(Type v) const { return (v < 0) ? -v : v; }
    // Private member variables.
    Type*    m_Address;
    size_t   m_Length;
};

template <typename Type>
void Vector<Type>::Allocate(void)
{
    m_Address = NULL;
    if (m_Length > 0)
    {
        m_Address = new Type[m_Length];
        ASSERT(NULL != m_Address);
    }
}

template <typename Type>
Vector<Type>::Vector(int length)
    : m_Address(NULL),
    m_Length(length)
{
    Allocate();
}

template <typename Type>
Vector<Type>::Vector(const Vector<Type>& v)
    : m_Address(NULL),
    m_Length(v.GetLength())
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = v[i]);
}

template <typename Type>
Vector<Type>::Vector(const VectordBase& v)
    : m_Address(NULL),
    m_Length(v.GetLength())
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = v[i]);
}

template <typename Type>
template <typename U>
Vector<Type>::Vector(const Vector<U>& v)
    : m_Address(NULL),
    m_Length(v.GetLength())
{
    Allocate();
    MCON_ITERATION(i, m_Length, (*this)[i] = static_cast<Type>(v[i]));
}

template <typename Type>
Vector<Type>::~Vector()
{
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = NULL;
    }
    m_Length = 0;
}

template <typename Type>
const Vector<Type>& Vector<Type>::Copy(const Vector<Type>& v)
{
    MCON_ITERATION(i, Smaller(v.GetLength()), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(const Vector<Type>& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    MCON_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
Vector<Type>& Vector<Type>::operator=(const VectordBase& v)
{
    // m_Length is updated in Resize().
    Resize(v.GetLength());
    MCON_ITERATION(i, v.GetLength(), (*this)[i] = v[i]);
    return *this;
}

template <typename Type>
bool Vector<Type>::Resize(size_t length)
{
    if (length == m_Length)
    {
        return true;
    }
    if (NULL != m_Address)
    {
        delete[] m_Address;
        m_Address = NULL;
    }
    m_Length = length;
    m_Address = new Type[length];
    ASSERT (NULL != m_Address);
    return true;
}

} // namespace mcon {

#include "Vectord.h"
