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

#include <cstring> // std::mem***

#include "debug.h" // ASSERT()

#include "Macros.h"
#include "VectordBase.h"

namespace mcon {

template <>
class Vector<double> : public VectordBase
{
public:

    explicit Vector<double>(size_t length = 0)
        : VectordBase(length)
        , m_AddressBase(NULL)
    {
        ASSERT(0 <= length);
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
    }

    Vector<double>(const Vector<double>& v)
        : VectordBase(v.GetLength())
        , m_AddressBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
        std::memcpy(m_AddressAligned, v, GetLength() * sizeof(double));
    }

    Vector<double>(const VectordBase& v)
        : VectordBase(v.GetLength())
        , m_AddressBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
        std::memcpy(m_AddressAligned, v, GetLength() * sizeof(double));
    }

    template <typename Alian>
    Vector<double>(const Vector<Alian>& v)
        : VectordBase(v.GetLength())
        , m_AddressBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(status == true);
        for (size_t i = 0; i < GetLength(); ++i )
        {
            (*this)[i] = static_cast<double>(v[i]);
        }
    }
    ~Vector<double>();

    template <typename Alian>
    Vector<double>& operator=(const Vector<Alian>& v)
    {
        bool status = Resize(v.GetLength());
        UNUSED(status);
        ASSERT(status == true);
        for (size_t i = 0; i < GetLength(); ++i )
        {
            (*this)[i] = static_cast<double>(v[i]);
        }
        return *this;
    }

    // operator= make the same Vector<double> as the input Vector<double>.
    Vector<double>& operator=(const Vector<double>& v);
    Vector<double>& operator=(double v)
    {
        *dynamic_cast<VectordBase*>(this) = v;
        return *this;
    }

    const Vector<double> operator+(double v) const;
    const Vector<double> operator-(double v) const;
    const Vector<double> operator*(double v) const;
    const Vector<double> operator/(double v) const;

    const Vector<double> operator+(const VectordBase& v) const;
    const Vector<double> operator-(const VectordBase& v) const;
    const Vector<double> operator*(const VectordBase& v) const;
    const Vector<double> operator/(const VectordBase& v) const;

    const Vector<double> operator()(size_t offset, size_t length) const;

    const Matrix<double> ToMatrix() const;

    bool Resize(size_t length);

    // Inline functions.
    inline bool IsNull(void) const
    {
        return m_AddressBase == NULL;
    }
    // Aliases.
    const Matrix<double> M() const;

private:
    // Private member functions.
    bool  Allocate(void);
    double* Aligned(double* ptr, size_t align);

    // Private member variables.
    double*  m_AddressBase;
};

// Type definition.
typedef Vector<double> Vectord;

// Global operators.
MACRO_MCON_GLOBAL_OPERATOR_DEFINITION(Vector<double>, double)

} // namespace mcon {
