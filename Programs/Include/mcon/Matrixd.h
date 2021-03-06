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

#include "debug.h"
#include "Macros.h"
#include "VectordBase.h"

namespace mcon {

// こんなんしたい
// typedef Matrix<double>(1, n) Vectord(n);

/*--------------------------------------------------------------------
 * Matrix<double>
 *--------------------------------------------------------------------*/

template <>
class Matrix<double>
{
public:
    Matrix<double>(size_t rowLength = 0, size_t columnLength = 0)
        : m_RowLength(rowLength)
        , m_ColumnLength(columnLength)
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        ASSERT( (rowLength == 0 && columnLength == 0) || (rowLength != 0 && columnLength != 0));
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
    }

    // Will be depricated.
    Matrix<double>(const VectordBase& v, bool transpose = false)
        : m_RowLength(transpose ? v.GetLength() : 1)
        , m_ColumnLength(transpose ? 1 : v.GetLength())
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
        if (true == transpose)
        {
            const size_t _0 = 0;
            for (size_t i = 0; i < v.GetLength(); ++i)
            {
                m_ObjectBase[i][_0] = v[i];
            }
        }
        else
        {
            m_ObjectBase[0] = v;
        }
    }
    Matrix<double>(const Matrix<double>& m)
        : m_RowLength(m.GetRowLength())
        , m_ColumnLength(m.GetColumnLength())
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
        for (size_t i = 0; i < GetRowLength(); ++i)
        {
            m_ObjectBase[i] = m[i];
        }
    }

    template <typename U>
    Matrix<double>(const Matrix<U>& m)
        : m_RowLength(m.GetRowLength())
        , m_ColumnLength(m.GetColumnLength())
        , m_Address(NULL)
        , m_ObjectBase(NULL)
    {
        bool status = Allocate();
        UNUSED(status);
        ASSERT(true == status);
        for (size_t i = 0; i < GetRowLength(); ++i)
        {
            m_ObjectBase[i] = m[i];
        }
    }

    ~Matrix<double>();

    const VectordBase& operator[](size_t i) const
    {
        ASSERT(i < m_RowLength);
        return *(m_ObjectBase + i);
    }

    VectordBase& operator[](size_t i)
    {
        ASSERT(i < m_RowLength);
        return *(m_ObjectBase + i);
    }

    const Matrix<double> SubMatrix(
        size_t rowBegin,
        size_t rowEnd,
        size_t columnBegin,
        size_t columnEnd
    ) const;

    inline const Matrix<double> operator()(
        size_t rowBegin,
        size_t rowEnd,
        size_t columnBegin,
        size_t columnEnd
    ) const
    {
        return SubMatrix(rowBegin, rowEnd, columnBegin, columnEnd);
    }

    Matrix<double>& operator=(double v);

    const Matrix<double> operator+(double v) const;
    const Matrix<double> operator-(double v) const;
    const Matrix<double> operator*(double v) const;
    const Matrix<double> operator/(double v) const;

    Matrix<double>& operator+=(double v);
    Matrix<double>& operator-=(double v);
    Matrix<double>& operator*=(double v);
    Matrix<double>& operator/=(double v);

    Matrix<double>& operator=(const Matrix<double>& m);

    const Matrix<double> operator+(const Matrix<double>& m) const;
    const Matrix<double> operator-(const Matrix<double>& m) const;
    const Matrix<double> operator*(const Matrix<double>& m) const;
    const Matrix<double> operator/(const Matrix<double>& m) const;

    Matrix<double>& operator+=(const Matrix<double>& m);
    Matrix<double>& operator-=(const Matrix<double>& m);
    Matrix<double>& operator*=(const Matrix<double>& m);
    Matrix<double>& operator/=(const Matrix<double>& m);

    double Determinant(void) const;
    Matrix<double> Transpose(void) const;
    Matrix<double> Multiply(const Matrix<double>& m) const;
    Matrix<double> Inverse(void) const;
    Matrix<double> GetCofactorMatrix(size_t row, size_t col) const;
    double GetCofactor(size_t row, size_t col) const;

    bool Resize(size_t, size_t);
    void Initialize( double (*initializer)(size_t, size_t, size_t, size_t) );

    static Matrix<double> Identify(size_t size);

    // Inline functions.
    inline bool IsNull(void) const { return m_Address == NULL; }
    inline size_t GetRowLength(void) const { return m_RowLength; }
    inline size_t GetColumnLength(void) const { return m_ColumnLength; }

    // Aliases
    inline Matrix<double> T(void) const { return Transpose(); }
    inline Matrix<double> I(void) const { return Inverse(); }
    inline double  D(void) const { return Determinant(); }
    inline static Matrix<double> E(size_t size) { return Identify(size); }

private:
    // Member functions (private).
    bool Allocate(void);
    size_t Smaller(size_t length) const { return (length > m_RowLength) ? m_RowLength : length; };

    static const size_t g_Alignment = 32;
    // Member variables (private).
    size_t m_RowLength;
    size_t m_ColumnLength;
    void* m_Address;
    VectordBase* m_ObjectBase;
};

// Type definition.
typedef Matrix<double> Matrixd;

// Global operators.
MACRO_MCON_GLOBAL_OPERATOR_DEFINITION(Matrix<double>, double)

} // namespace mcon {
