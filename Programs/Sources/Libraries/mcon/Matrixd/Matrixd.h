/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
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

#include "debug.h"
#include "Vectord.h"

namespace mcon {

class Vectord;


/*--------------------------------------------------------------------
 * Matrixd
 *--------------------------------------------------------------------*/

class Matrixd
{
public:
    void DumpMatrix(const Matrixd<Type>&m, const char* fmt) const
    {
        for (int i = 0; i < m.GetRowLength(); ++i)
        {
            printf("| ");
            for (int j = 0; j < m.GetColumnLength(); ++j)
            {
                printf(fmt, m[i][j]);
                printf("\t");
            }
            printf(" |\n");
        }
    }
    Matrixd(int rowLength = 0, int columnLength = 0);
    Matrixd(const Matrixd<Type>& m);
    template <typename U>
    Matrixd(const Matrixd<U>& m);

    ~Matrixd();

    const Vectord& operator[](int i) const
    {
        ASSERT(0 <= i && i < m_RowLength);
        return *m_Array[i];
    }

    Vectord& operator[](int i)
    {
        ASSERT(0 <= i && i < m_RowLength);
        return *m_Array[i];
    }

    Matrixd<Type>& operator=(const Matrixd<Type>& m);

    const Matrixd<Type> operator+(Type v) const { Matrixd<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] += v); return mat; }
    const Matrixd<Type> operator-(Type v) const { Matrixd<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] -= v); return mat; }
    const Matrixd<Type> operator*(Type v) const { Matrixd<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] *= v); return mat; }
    const Matrixd<Type> operator/(Type v) const { Matrixd<Type> mat(*this); MCON_ITERATION( i, GetRowLength(), mat[i] /= v); return mat; }

    const Matrixd<Type> operator+(const Matrixd<Type>& m) const { Matrixd<Type> mat(*this); mat += m; return mat; }
    const Matrixd<Type> operator-(const Matrixd<Type>& m) const { Matrixd<Type> mat(*this); mat -= m; return mat; }
    const Matrixd<Type> operator*(const Matrixd<Type>& m) const { Matrixd<Type> mat(*this); mat *= m; return mat; }
    const Matrixd<Type> operator/(const Matrixd<Type>& m) const { Matrixd<Type> mat(*this); mat /= m; return mat; }

    Matrixd<Type>& operator+=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] += v); return *this; }
    Matrixd<Type>& operator-=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] -= v); return *this; }
    Matrixd<Type>& operator*=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] *= v); return *this; }
    Matrixd<Type>& operator/=(Type v) { MCON_ITERATION( i, GetRowLength(), (*this)[i] /= v); return *this; }

    Matrixd<Type>& operator+=(const Matrixd<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] += m[i]); return *this; }
    Matrixd<Type>& operator-=(const Matrixd<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] -= m[i]); return *this; }
    Matrixd<Type>& operator*=(const Matrixd<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] *= m[i]); return *this; }
    Matrixd<Type>& operator/=(const Matrixd<Type>& m) { MCON_ITERATION( i, Smaller(m.GetRowLength()), (*this)[i] /= m[i]); return *this; }

    inline Matrixd<Type> T(void) const { return Transpose(); }
    inline Matrixd<Type> I(void) const { return Inverse(); }
    Type D(void) const { return Determinant(); }
    inline static Matrixd<Type> E(int size) { return Identify(size); }

    Matrixd<Type> Transpose(void) const;
    Matrixd<Type> Multiply(const Matrixd<Type>& m) const;
    Matrixd<Type> Inverse(void) const;
    Type Determinant(void) const;
    Matrixd<Type> GetCofactorMatrix(int row, int col) const;
    Type GetCofactor(int row, int col) const;

    static Matrixd<Type> Identify(int size)
    {
        Matrixd<Type> I(size, size);
        for (int i = 0; i < size; ++i)
        {
            I[i] = 0;
        }
        for (int i = 0; i < size; ++i)
        {
            I[i][i] = 1;
        }
        return I;
    }

    bool IsNull(void) const { return m_RowLength == 0; }
    int GetRowLength(void) const { return m_RowLength; }
    int GetColumnLength(void) const { return m_ColumnLength; }
    bool Resize(int, int);
private:
    // Member functions (private).
    void AllocateRow   (void);
    void AllocateColumn(void);
    void Allocate      (void);
    int Smaller(int length) const { return (length > m_RowLength) ? m_RowLength : length; };

    // Member variables (private).
    int m_RowLength;
    int m_ColumnLength;
    Vectord** m_Array;
};



void Matrixd<Type>::Allocate(void)
{
    AllocateRow();
    AllocateColumn();
}


void Matrixd<Type>::AllocateRow(void)
{
    if (0 < m_RowLength)
    {
        m_Array = new Vectord*[m_RowLength];
        ASSERT(NULL != m_Array);
    }
}


void Matrixd<Type>::AllocateColumn(void)
{
    for (int i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vectord(m_ColumnLength);
        ASSERT(m_Array[i] != NULL);
    }
}


Matrixd<Type>::Matrixd(int rowLength, int columnLength)
    : m_RowLength(rowLength),
    m_ColumnLength(columnLength),
    m_Array(NULL)
{
    Allocate();
}


Matrixd<Type>::Matrixd(const Matrixd<Type>& m)
    : m_RowLength(m.GetRowLength()),
    m_ColumnLength(m.GetColumnLength()),
    m_Array(NULL)
{
    AllocateRow();
    for (int i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vectord(m[i]);
        ASSERT(m_Array[i] != NULL);
    }
}


template <typename U>
Matrixd<Type>::Matrixd(const Matrixd<U>& m)
    : m_RowLength(m.GetRowLength()),
    m_ColumnLength(m.GetColumnLength()),
    m_Array(NULL)
{
    AllocateRow();
    for (int i = 0; i < m_RowLength; ++i)
    {
        m_Array[i] = new Vectord(m[i]);
        ASSERT(m_Array[i] != NULL);
    }
}


Matrixd<Type>::~Matrixd()
{
    if (m_Array != NULL)
    {
        for (int i = 0; i < m_RowLength; ++i)
        {
            delete m_Array[i];
        }
        delete[] m_Array;
    }
    m_Array = PTR_CAST(Vectord**, NULL);
    m_RowLength = 0;
    m_ColumnLength = 0;
}


bool Matrixd<Type>::Resize(int rowLength, int columnLength)
{
    if (rowLength < 0 || columnLength < 0)
    {
        return false;
    }

    if (rowLength != m_RowLength)
    {
        Vectord** ptr = NULL;
        if (rowLength > 0)
        {
            ptr = new Vectord*[rowLength];
            ASSERT(NULL != ptr);
            for (int i = 0; i < rowLength; ++i)
            {
                ptr[i] = NULL;
            }
        }

        for (int i = 0; i < Smaller(rowLength); ++i)
        {
            ptr[i] = m_Array[i];
            m_Array[i] = NULL;
        }
        for (int i = 0; i < rowLength; ++i)
        {
            if (ptr[i] == NULL)
            {
                ptr[i] = new Vectord(columnLength);
                ASSERT(ptr[i] != NULL);
            }
        }
        for (int i = 0; i < m_RowLength; ++i)
        {
            if (m_Array[i] != NULL)
            {
                delete m_Array[i];
                m_Array[i] = NULL;
            }
        }
        delete[] m_Array;
        m_Array = ptr;
        m_RowLength = rowLength;
    }
    m_ColumnLength = columnLength;

    bool status = true;
    for (int i = 0; i < m_RowLength; ++i)
    {
        status &= (*m_Array[i]).Resize(m_ColumnLength);
    }
    return status;
}


Matrixd<Type>& Matrixd<Type>::operator=(const Matrixd<Type>& m)
{
    Resize(m.GetRowLength(), m.GetColumnLength());
    for (int i = 0; i < m_RowLength; ++i)
    {
        *m_Array[i] = m[i];
    }
    return *this;
}


Matrixd<Type> Matrixd<Type>::Transpose(void) const
{
    Matrixd<Type> transposed(GetColumnLength(), GetRowLength());
    for (int i = 0; i < GetRowLength(); ++i)
    {
        for (int j = 0; j < GetColumnLength(); ++j)
        {
            transposed[j][i] = (*this)[i][j];
        }
    }
    return transposed;
}


Matrixd<Type> Matrixd<Type>::Multiply(const Matrixd<Type>& m) const
{
    if ( GetColumnLength() != m.GetRowLength() )
    {
        return *this;
    }
    Matrixd<Type> multiplied(GetRowLength(), m.GetColumnLength());
    for (int row = 0; row < multiplied.GetRowLength(); ++row)
    {
        for (int col = 0; col < multiplied.GetColumnLength(); ++col)
        {
            Type v = 0;
            for (int k = 0; k < GetColumnLength(); ++k)
            {
                v += (*this)[row][k] * m[k][col];
            }
            multiplied[row][col] = v;
        }
    }
    return multiplied;
}


Matrixd<Type> Matrixd<Type>::GetCofactorMatrix(int row, int col) const
{
    int rowCount = GetRowLength();
    int colCount = GetColumnLength();
    Matrixd<Type> cofactorMatrix(rowCount-1, colCount-1);

    for (int r = 0, ri = 0; ri < rowCount; ++ri)
    {
        if (ri == row)
        {
            continue;
        }
        for (int c = 0, ci = 0; ci < colCount; ++ci)
        {
            if (ci != col)
            {
                cofactorMatrix[r][c] = (*this)[ri][ci];
                ++c;
            }
        }
        ++r;
    }
    return cofactorMatrix;
}


Type Matrixd<Type>::GetCofactor(int row, int col) const
{
    int sign = ( (row + col) & 1) ? -1 : 1;
    return GetCofactorMatrix(row, col).Determinant() * sign;
}


Type Matrixd<Type>::Determinant(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return 0;
    }
    int dimension = GetColumnLength();
    Type det = 0;

    // Sarrus
    if ( 3 == dimension )
    {
        const Matrixd<Type>&m = *this;
        det = m[0][0] * m[1][1] * m[2][2]
              + m[0][1] * m[1][2] * m[2][0]
              + m[0][2] * m[1][0] * m[2][1]
              - m[0][2] * m[1][1] * m[2][0]
              - m[0][1] * m[1][0] * m[2][2]
              - m[0][0] * m[1][2] * m[2][1];
    }
    else if ( 2 == dimension )
    {
        const Matrixd<Type>&m = *this;
        det = m[0][0] * m[1][1]
              - m[0][1] * m[1][0];
    }
    else if ( 1 == dimension )
    {
        const Matrixd<Type>&m = *this;
        det = m[0][0];
    }
    else
    {
        for (int row = 0; row < GetRowLength(); ++row)
        {
            //Matrixd<Type> m(CoFactor(row, 0));
            //DumpMatrix(m, "%f");
            //double v = m.Determinant() * sign * (*this)[row][0];
            det += GetCofactor(row, 0) * (*this)[row][0];
        }
    }
    return det;
}


Matrixd<Type> Matrixd<Type>::Inverse(void) const
{
    if ( GetColumnLength() != GetRowLength() )
    {
        return *this;
    }

    Type det = Determinant();
    if ( 0 == det )
    {
        printf("Det=Zero\n");
        return *this;
    }
    // Calculate Inversed-Matrixd by Cofactors.
    printf("Determinant=%f\n", det);
    int rowCount = GetRowLength();
    int colCount = GetColumnLength();
    Matrixd<Type> inversed(rowCount, colCount);

#if 0
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            int sign = ((row + col) & 1) ? -1 : 1;
            Matrixd<Type> m(GetCofactorMatrix(row, col));
            printf("cof[%d, %d]\n", row, col);
            //DumpMatrix(m, "%f");
            inversed[col][row] = GetCofactor(row, col) / det;
            printf("det=%f\n", inversed[row][col]);
        }
    }
#else
    // Calculate by Wipe-out
    static const Type threshold = 1.0e-10; // TBD

    Matrixd<Type> m(rowCount, colCount*2);
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            m[row][col] = (*this)[row][col];
        }
    }
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = colCount; col < colCount*2; ++col)
        {
            if (row == (col - colCount))
            {
                m[row][col] = 1.0;
            }
            else
            {
                m[row][col] = 0.0;
            }
        }
    }
    for (int iter = 0; iter < rowCount; ++iter)
    {
        // Find a row which m[row][i] is zero.
        bool isFound = false;
        for (int i = iter; i < rowCount; ++i)
        {
            if (fabs(m[i][iter]) > threshold)
            {
                Vectord vec(m[i]);
                m[i] = m[iter];
                vec /= vec[iter];
                m[iter] = vec;
                isFound = true;
                break;
            }
        }
        // Give-up...
        if (false == isFound)
        {
            return *this;
        }
        for (int i = 0; i < rowCount; ++i)
        {
            if (i != iter)
            {
                Vectord vec(m[iter]);
                vec *= m[i][iter];
                m[i] -= vec;
            }
        }
    }
    for (int row = 0; row < rowCount; ++row)
    {
        for (int col = 0; col < colCount; ++col)
        {
            inversed[row][col] = m[row][col+colCount];
        }
    }
#endif
    return inversed;
}

} // namespace mcon {