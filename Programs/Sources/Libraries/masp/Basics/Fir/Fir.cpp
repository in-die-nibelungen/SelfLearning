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

#include "debug.h"
#include "types.h"
#include "status.h"
#include "masp/Fir.h"

namespace masp {
namespace fir {

enum FilterTypeId
{
    FTI_LPF = 1,
    FTI_HPF,
    FTI_BPF,
    FTI_BEF
};

enum FilterBaseId
{
    FBI_SINC = 1,
    FBI_LANCZOS
};

#if !defined(M_PI)
#define M_PI (3.14159265358979323846)
#endif

namespace {
    const double g_Pi(M_PI);
}

typedef double (*BaseFunction)(double, double);
typedef double (*FilterFunction)(double m, double fe1, double fe2, double arg, BaseFunction function);

double Sinc(double x)
{
    return (0.0 == x) ? 1.0 : sin(x)/x;
}

double SincFunction(double x, double arg)
{
    return Sinc(x);
}

double LanczosFunction(double x, double n)
{
    return Sinc(x) * Sinc(x/n);
}

BaseFunction GetBaseFunction(int type)
{
    BaseFunction function = NULL;
    switch(type)
    {
    case FBI_SINC   : function = SincFunction   ; break;
    case FBI_LANCZOS: function = LanczosFunction; break;
    }
    return function;
}

double LpfFunction(double m, double fe, double sbz, double arg, BaseFunction function)
{
    return 2 * fe * function( 2.0 * g_Pi * fe * m, arg);
}

double HpfFunction(double m, double fe, double sbz, double arg, BaseFunction function)
{
    return function(g_Pi * m, arg) - 2.0 * fe * function(2.0 * g_Pi * fe * m, arg);
}

double BpfFunction(double m, double fe1, double fe2, double arg, BaseFunction function)
{
    return 2.0 * fe2 * function(2.0 * g_Pi * fe2 * m, arg) - 2.0 * fe1 * function(2.0 * g_Pi * fe1 * m, arg);
}

double BefFunction(double m, double fe1, double fe2, double arg, BaseFunction function)
{
    return function(g_Pi * m, arg) - 2.0 * fe2 * function(2.0 * g_Pi * fe2 * m, arg) + 2.0 * fe1 * function(2.0 * g_Pi * fe1 * m, arg);
}

FilterFunction GetFilterFunction(int type)
{
    FilterFunction function = NULL;

    switch(type)
    {
    case FTI_LPF: function = LpfFunction; break;
    case FTI_HPF: function = HpfFunction; break;
    case FTI_BPF: function = BpfFunction; break;
    case FTI_BEF: function = BefFunction; break;
    }
    return function;
}

void GenerateLpfFilter(double coef[], size_t N, double fe, int type, double arg)
{
    BaseFunction function = GetBaseFunction(type);
    ASSERT(NULL != function);
    for (size_t i = 0; i < N / 2; ++i)
    {
        const double x = 2.0 * fe * static_cast<int>(2 * i + 1 - N) / 2.0 * g_Pi;
        double v = 2.0 * fe * function(x, arg);
        coef[i] = v;
        coef[N - i - 1] = v;
    }
    if (1 == (N & 1))
    {
        coef[N/2] = 2 * fe;
    }
}

static void GetCoefficients(mcon::Vector<double>& coef, double fe1, double fe2, double arg, int typeId, int functionId)
{
    const size_t N = coef.GetLength();
    BaseFunction baseFunction = GetBaseFunction(functionId);
    ASSERT(NULL != baseFunction);
    FilterFunction filterFunction = GetFilterFunction(typeId);
    ASSERT(NULL != filterFunction);
    for (size_t i = 0; i < (N + 1) / 2; ++i)
    {
        const double m = (2 * static_cast<int>(i) + 1 - static_cast<int>(N)) / 2.0;
        double v = filterFunction(m, fe1, fe2, arg, baseFunction);
        coef[i] = v;
        coef[N - i - 1] = v;
    }
}

static void GetCoefficients(double coef[], size_t N, double fe1, double fe2, double arg, int typeId, int functionId)
{
    mcon::Vector<double> _coef(N);
    GetCoefficients(_coef, fe1, fe2, arg, typeId, functionId);
    for (size_t i = 0; i < _coef.GetLength(); ++i)
    {
        coef[i] = _coef[i];
    }
}

void GetCoefficientsLpfSinc(mcon::Vector<double>& coef, double fe)
{
    GetCoefficients(coef, fe, 0.0, 0.0, FTI_LPF, FBI_SINC);
}

void GetCoefficientsHpfSinc(mcon::Vector<double>& coef, double fe)
{
    GetCoefficients(coef, fe, 0.0, 0.0, FTI_HPF, FBI_SINC);
}

void GetCoefficientsBpfSinc(mcon::Vector<double>& coef, double fe1, double fe2)
{
    GetCoefficients(coef, fe1, fe2, 0.0, FTI_BPF, FBI_SINC);
}

void GetCoefficientsBefSinc(mcon::Vector<double>& coef, double fe1, double fe2)
{
    GetCoefficients(coef, fe1, fe2, 0.0, FTI_BEF, FBI_SINC);
}

void GetCoefficientsLpfLanczos(mcon::Vector<double>& coef, double fe, double n)
{
    GetCoefficients(coef, fe, 0.0, n, FTI_LPF, FBI_LANCZOS);
}

void GetCoefficientsHpfLanczos(mcon::Vector<double>& coef, double fe, double n)
{
    GetCoefficients(coef, fe, 0.0, n, FTI_HPF, FBI_LANCZOS);
}

void GetCoefficientsBpfLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n)
{
    GetCoefficients(coef, fe1, fe2, n, FTI_BPF, FBI_LANCZOS);
}

void GetCoefficientsBefLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n)
{
    GetCoefficients(coef, fe1, fe2, n, FTI_BEF, FBI_LANCZOS);
}

void FilterSinc(double coef[], size_t N, double fe)
{
    //GenerateLpfFilter(coef, _N, fe, FBI_SINC, 0.0);
    GetCoefficients(coef, N, fe, 0.0, 0.0, FTI_LPF, FBI_SINC);
#if 0
    for (int i = 0; i < N/2; ++i)
    {
        const double x = 2.0 * fe * (2 * i + 1 - N) / 2.0 * g_Pi;
        double v = 2.0 * fe * Sinc(x);
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (1 == (N & 1))
    {
        coef[N/2] = 2 * fe;
    }
#endif
}

void FilterLanczos(double coef[], size_t N, double fe, double n)
{
    if (n < 1.0)
    {
        n = 1.0;
    }
    GenerateLpfFilter(coef, N, fe, FBI_LANCZOS, n);
    GetCoefficients(coef, N, fe, 0.0, n, FTI_LPF, FBI_LANCZOS);
#if 0
    for (int i = 0; i < N/2; ++i)
    {
        const double x = 2.0 * fe * (2 * i + 1 - N) / 2.0 * g_Pi;
        double v = 2.0 * fe * Sinc(x) * Sinc(x/n);
        coef[i] = v;
        coef[N-i-1] = v;
    }
    if (1 == (N & 1))
    {
        coef[N/2] = 2 * fe;
    }
#endif
}

int GetNumOfTapps(double delta)
{
    int tapps = static_cast<int>((3.1f / delta) + 0.5 - 1);
    if ( (tapps & 1) == 0)
    {
        ++tapps;
    }
    return tapps;
}

status_t Convolution(mcon::Vector<double>& out, const mcon::Vector<double>& in, const mcon::Vector<double>& impulse)
{
    const size_t M = impulse.GetLength();
    if (in.GetLength() < M)
    {
        return -ERROR_ILLEGAL;
    }

    if ( false == out.Resize(in.GetLength()) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    for (size_t i = 0; i < in.GetLength(); ++i)
    {
        out[i] = 0.0;
        for (size_t k = 0; k < ( (M - 1 > i) ? i + 1 : M ); ++k)
        {
            out[i] += in[i - k] * impulse[k];
        }
    }
    return NO_ERROR;
}

} // namespace fir {
} // namespace masp {
