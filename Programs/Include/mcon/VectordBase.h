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

#include <cstdint>
#include <cstring>

namespace mcon {

template <typename Type>
class Vector;

template <typename Type>
class Matrix;

class VectordBase
{
    friend class Vector<double>;
    friend class Matrix<double>;
public:

    // Private class variables.
    static const size_t g_Alignment = 32;

    VectordBase(void* addressAligned, size_t length)
        : m_AddressAligned(reinterpret_cast<double*>(addressAligned))
        , m_Length(length)
    {
        ASSERT( addressAligned != NULL );
        ASSERT_ALIGNED( addressAligned, g_Alignment );
    }
    ~VectordBase()
    {
        m_AddressAligned = NULL;
        m_Length = 0;
    }

    // For const object
    const double& operator[](const size_t i) const
    {
        ASSERT (i < m_Length);
        return m_AddressAligned[i];
    }
    // For non-const object
    double& operator[](const size_t i)
    {
        ASSERT (i < m_Length);
        return m_AddressAligned[i];
    }

    // Copy is to copy available data from src to dest without resizing the dest.
    const VectordBase& Copy(const VectordBase& VectordBase);

    template <typename U>
    operator Vector<U>() const
    {
        const size_t n = GetLength();
        Vector<U> v(n);
        for (size_t i = 0; n; ++i )
        {
            v[i] = static_cast<U>((*this)[i]);
        }
        return v;
    }
    operator void*() const
    {
        return reinterpret_cast<void*>(m_AddressAligned);
    }
    operator double*() const
    {
        return reinterpret_cast<double*>(m_AddressAligned);
    }

    double PushFromFront(double v);
    double PushFromBack(double v);

    VectordBase& operator=(double v);

    template <typename U>
    VectordBase& operator=(const Vector<U>& v)
    {
        ASSERT(v.GetLength() == GetLength());
        std::memcpy(*this, v, GetLength() * sizeof(double));
        return *this;
    }
    VectordBase& operator=(const VectordBase& v)
    {
        ASSERT(v.GetLength() == GetLength());
        std::memcpy(*this, v, GetLength() * sizeof(double));
        return *this;
    }
    VectordBase& operator+=(double v);
    VectordBase& operator-=(double v);
    VectordBase& operator*=(double v);
    VectordBase& operator/=(double v);

    VectordBase& operator+=(const VectordBase& v);
    VectordBase& operator-=(const VectordBase& v);
    VectordBase& operator*=(const VectordBase& v);
    VectordBase& operator/=(const VectordBase& v);

    void Initialize(int offset = 0, int step = 1)
    {
        for (size_t k = 0; k < GetLength(); ++k )
        {
            (*this)[k] = offset + step * k;
        }
    }

    void Initialize( double (*initializer)(size_t, size_t) )
    {
        for (size_t k = 0; k < GetLength(); ++k )
        {
            (*this)[k] = initializer(k, GetLength());
        }
    }

    double GetMaximum(void) const;
    double GetMaximumAbsolute(void) const;
    double GetMinimum(void) const;
    double GetMinimumAbsolute(void) const;

    size_t GetMaximumIndex(size_t offset = 0) const;
    size_t GetMaximumAbsoluteIndex(size_t offset = 0) const;
    size_t GetMinimumIndex(size_t offset = 0) const;
    size_t GetMinimumAbsoluteIndex(size_t offset = 0) const;

    int GetLocalMaximumIndex(size_t offset = 0) const;
    int GetLocalMinimumIndex(size_t offset = 0) const;

    double GetSum(void) const;

    double GetNorm(void) const;
    double GetDotProduct(const VectordBase& v) const;
    VectordBase GetCrossProduct(const VectordBase& v) const;

    // Inline functions.
    inline size_t GetLength(void) const
    {
        return m_Length;
    }
    inline double GetAverage(void) const
    {
        return GetSum()/GetLength();
    }
    // Will be depricated.
    inline double Fifo(double v)
    {
        return PushFromBack(v);
    }
    inline double Shift(double v)
    {
        return PushFromBack(v);
    }
    inline double Unshift(double v)
    {
        return PushFromFront(v);
    }
    inline double Dot(const VectordBase& v) const
    {
        return GetDotProduct(v);
    }
    inline VectordBase Cross(VectordBase& v) const
    {
        return GetCrossProduct(v);
    }

private:
    // A special interface for Vectord, which allow m_AddressAligned to be NULL.
    explicit VectordBase(int length = 0)
        : m_AddressAligned(NULL)
        , m_Length(length)
    {}

    // Private member variables.
    double*  m_AddressAligned;
    size_t   m_Length;
};

} // namespace mcon {
