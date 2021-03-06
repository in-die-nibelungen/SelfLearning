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

#include <math.h>

#include "mcon.h"
#include "types.h"
#include "status.h"
#include "debug.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

namespace masp {
namespace ft {

namespace
{
    const double g_Pi(M_PI);
    int Ilog2(int v)
    {
        for (int i = sizeof(v) * 8 - 1; i >= 0 ; --i)
        {
            if ((v >> i) & 0x1)
            {
                return i;
            }
        }
        return -ERROR_ILLEGAL;
    }
    int Count1(int v)
    {
        int ans = 0;
        for (unsigned int i = 0; i < sizeof(v) * 8; ++i)
        {
            ans += ((v >> i) & 0x1) ? 1 : 0;
        }
        return ans;
    }
    int BitReverse(int v, int w) // value, width
    {
        int x = 0;
        for (int i = 0; i < w; ++i)
        {
            x |= (((v >> i) & 0x1) << (w - i - 1));
        }
        return x;
    }
}

status_t Fft(mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries)
{
    const size_t N = timeSeries.GetLength();
    if (Count1(N) != 1)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = complex.Resize(2, N);
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    mcon::VectordBase& real = complex[0];
    mcon::VectordBase& imag = complex[1];

    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable(N/2);
    mcon::Vector<double> cosTable(N/2);

    for (size_t i = 0; i < N / 2; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }
    // Substitute beforehand
    real = timeSeries;
    imag = 0;

    size_t innerLoop = N / 2;
    int outerLoop = 1;
    //DEBUG_LOG("N=%d\n", N);

    // Decimation in frequency.
    for ( ; 0 < innerLoop; innerLoop >>= 1, outerLoop <<= 1)
    {
        //DEBUG_LOG("inner=%d, outer=%d\n", innerLoop, outerLoop);
        for ( int outer = 0; outer < outerLoop; ++outer )
        {
            const int& step = innerLoop;
            const int ofs = outer * step * 2;
#if 0
            for (size_t k = 0; k < innerLoop; ++k )
            {
                const double r1 = real[k+ofs];
                const double i1 = imag[k+ofs];
                const double r2 = real[k+step+ofs];
                const double i2 = imag[k+step+ofs];

                real[k+ofs] = r1 + r2;
                imag[k+ofs] = i1 + i2;
                const int idx = k * outerLoop;
                //DEBUG_LOG("step=%d, ofs=%d, k=%d, idx=%d\n", step, ofs, k, idx);
                real[k+step+ofs] =   (r1 - r2) * cosTable[idx] + (i1 - i2) * sinTable[idx];
                imag[k+step+ofs] = - (r1 - r2) * sinTable[idx] + (i1 - i2) * cosTable[idx];
            }
#else
            // Easier to look into.
            void* _pReal = real;
            void* _pImag = imag;
            double* pRealL = reinterpret_cast<double*>(_pReal) + ofs;
            double* pImagL = reinterpret_cast<double*>(_pImag) + ofs;
            double* pRealH = reinterpret_cast<double*>(_pReal) + ofs + step;
            double* pImagH = reinterpret_cast<double*>(_pImag) + ofs + step;
            for (size_t k = 0; k < innerLoop; ++k )
            {
                const double r1 = pRealL[k];
                const double i1 = pImagL[k];
                const double r2 = pRealH[k];
                const double i2 = pImagH[k];

                const size_t idx = k * outerLoop;
                pRealL[k] = r1 + r2;
                pImagL[k] = i1 + i2;
                pRealH[k] =   (r1 - r2) * cosTable[idx] + (i1 - i2) * sinTable[idx];
                pImagH[k] = - (r1 - r2) * sinTable[idx] + (i1 - i2) * cosTable[idx];
                //DEBUG_LOG("step=%d, ofs=%d, k=%d, idx=%d\n", step, ofs, k, idx);
            }
#endif
        }
    }
    // Bit-reverse
    const int width = Ilog2(N);
    //DEBUG_LOG("width=%d\n", width);
    for (size_t i = 0; i < N; ++i )
    {
        const size_t k = BitReverse(i, width);
        if (k == i || k < i)
        {
            continue;
        }
        const double real_temp = real[k];
        const double imag_temp = imag[k];
        real[k] = real[i];
        imag[k] = imag[i];
        real[i] = real_temp;
        imag[i] = imag_temp;
    }
    return NO_ERROR;
}

status_t Ifft(mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex)
{
    const size_t N = complex.GetColumnLength();
    if (Count1(N) != 1)
    {
        return -ERROR_ILLEGAL;
    }
    mcon::Vector<double> tsPair;
    mcon::Vector<double> sinTable;
    mcon::Vector<double> cosTable;

    if ( false == timeSeries.Resize(N)
        || false == tsPair.Resize(N)
        || false == sinTable.Resize(N)
        || false == cosTable.Resize(N) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    const double df = 2.0 * g_Pi / N;

    for (size_t i = 0; i < N / 2; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }
    // Substitute beforehand
    timeSeries = complex[0];
    tsPair = complex[1];

    size_t innerLoop = N / 2;
    size_t outerLoop = 1;

    // Decimation in frequency.
    for ( ; 0 < innerLoop; innerLoop >>= 1, outerLoop <<= 1)
    {
        //DEBUG_LOG("inner=%d, outer=%d\n", innerLoop, outerLoop);
        for (size_t outer = 0; outer < outerLoop; ++outer )
        {
            const int& step = innerLoop;
            const int ofs = outer * step * 2;
            // Easier to look into.
            void* _pTs = timeSeries;
            void* _pTsPair = tsPair;
            double* pTsL     = reinterpret_cast<double*>(_pTs) + ofs;
            double* pTsPairL = reinterpret_cast<double*>(_pTsPair) + ofs;
            double* pTsH     = reinterpret_cast<double*>(_pTs) + ofs + step;
            double* pTsPairH = reinterpret_cast<double*>(_pTsPair) + ofs + step;
            for (size_t k = 0; k < innerLoop; ++k )
            {
                const double r1 = pTsL[k];
                const double i1 = pTsPairL[k];
                const double r2 = pTsH[k];
                const double i2 = pTsPairH[k];

                const size_t idx = k * outerLoop;
                pTsL[k]     = r1 + r2;
                pTsPairL[k] = i1 + i2;
                pTsH[k]     =   (r1 - r2) * cosTable[idx] - (i1 - i2) * sinTable[idx];
                pTsPairH[k] =   (r1 - r2) * sinTable[idx] + (i1 - i2) * cosTable[idx];
                //DEBUG_LOG("step=%d, ofs=%d, k=%d, idx=%d\n", step, ofs, k, idx);
            }
        }
    }
    // Bit-reverse
    const int width = Ilog2(N);
    //DEBUG_LOG("width=%d\n", width);
    for (size_t i = 0; i < N; ++i )
    {
        const size_t k = BitReverse(i, width);
        if (k < i)
        {
            continue;
        }
        const double ts_temp = timeSeries[k] / N;
        timeSeries[k] = timeSeries[i] / N;
        timeSeries[i] = ts_temp;
    }
    return NO_ERROR;
}

status_t Fft(double real[], double imag[], double td[], int n)
{
    mcon::Vector<double> input;
    mcon::Matrix<double> complex;

    if ( false == input.Resize(n) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    if ( false == complex.Resize(2, n) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    memcpy(input, td, sizeof(double) * n);

    status_t status;
    status = Fft(complex, input);

    if ( NO_ERROR == status )
    {
        memcpy(real, complex[0], n * sizeof(double));
        memcpy(imag, complex[1], n * sizeof(double));
    }
    return status;
}

status_t Ifft(double td[], double real[], double imag[], int n)
{
    return NO_ERROR;
}

status_t Ft(double real[], double imag[], const double td[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        double df = (double)i * g_Pi * 2/ n;
        real[i] = 0.0;
        imag[i] = 0.0;
        for (int j = 0; j < n; ++j)
        {
            real[i] += (td[j] * cos(df * j));
            imag[i] -= (td[j] * sin(df * j));
        }
    }
    return NO_ERROR;
}

status_t Ft(mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries)
{
    bool status = complex.Resize(2, timeSeries.GetLength());
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    mcon::VectordBase& real = complex[0];
    mcon::VectordBase& imag = complex[1];

    const size_t N = timeSeries.GetLength();
    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable;
    mcon::Vector<double> cosTable;

    if ( false == sinTable.Resize(N) ||
         false == cosTable.Resize(N) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    for (size_t i = 0; i < N; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }

    for (size_t i = 0; i < N; ++i)
    {
        real[i] = 0.0;
        imag[i] = 0.0;
        for (size_t j = 0; j < N; ++j)
        {
            const size_t k = ( static_cast<int64_t>(i) * static_cast<int64_t>(j) ) % N;
            real[i] += (timeSeries[j] * cosTable[k]);
            imag[i] -= (timeSeries[j] * sinTable[k]);
        }
    }
    return NO_ERROR;
}

status_t Ift(double td[], const double real[], const double imag[], int N)
{
    for (int i = 0; i < N; ++i)
    {
        double df = (double)i * g_Pi * 2 / N;
        td[i] = 0.0;
        for (int j = 0; j < N; ++j)
        {
            td[i]  += (real[j] * cos(df * j) - imag[j] * sin(df * j));
        }
        td[i] /= N;
    }
    return NO_ERROR;
}

status_t Ift(mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex)
{
    if (complex.GetRowLength() < 2)
    {
        return -ERROR_ILLEGAL;
    }
    if ( false == timeSeries.Resize(complex.GetColumnLength()) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    const size_t N = complex.GetColumnLength();
    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable(N);
    mcon::Vector<double> cosTable(N);

    for (size_t i = 0; i < N; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }

    const mcon::Vector<double>& real = complex[0];
    const mcon::Vector<double>& imag = complex[1];

    for (size_t i = 0; i < timeSeries.GetLength(); ++i)
    {
        const size_t N = complex.GetColumnLength();
        timeSeries[i] = 0.0;
        for (size_t j = 0; j < N; ++j)
        {
            const size_t k = ( static_cast<int64_t>(i) * static_cast<int64_t>(j) ) % N;
            timeSeries[i] += (real[j] * cosTable[k] - imag[j] * sinTable[k]);
        }
    }
    timeSeries /= N;
    return NO_ERROR;
}

#define POW2(v) ((v)*(v))

status_t ConvertToPolarCoords(mcon::Matrix<double>& polar, const mcon::Matrix<double>& complex)
{
    if (complex.GetRowLength() < 2)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = polar.Resize(2, complex.GetColumnLength());
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    mcon::VectordBase& r  = polar[0];
    mcon::VectordBase& arg = polar[1];
    const mcon::Vector<double>& real  = complex[0];
    const mcon::Vector<double>& imag  = complex[1];

    for (size_t i = 0; i < complex.GetColumnLength(); ++i)
    {
        r[i] = sqrt(POW2(real[i]) + POW2(imag[i]));
        arg[i] = atan(imag[i]/real[i]);
    }
    return NO_ERROR;
}

status_t ConvertToComplex(mcon::Matrix<double>& complex, const mcon::Matrix<double>& polar)
{
    if (polar.GetRowLength() < 2)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = complex.Resize(2, polar.GetColumnLength());
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    const mcon::Vector<double>& r = polar[0];
    const mcon::Vector<double>& arg = polar[1];
    mcon::VectordBase& real  = complex[0];
    mcon::VectordBase& imag  = complex[1];

    for (size_t i = 0; i < polar.GetColumnLength(); ++i)
    {
        const double v = r[i];
        real[i] = v * cos(arg[i]);
        imag[i] = v * sin(arg[i]);
    }
    return NO_ERROR;
}

} // namespace ft {
} // namespace masp {

