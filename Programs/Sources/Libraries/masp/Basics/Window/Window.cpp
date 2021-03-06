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

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

#define POW2(x) ((x)*(x))

namespace masp { namespace window {

enum WindowFunctionId
{

    WFI_RECTANGULAR = 1,
    WFI_HANNING,
    WFI_HAMMING,
    WFI_GENERALIZED_HAMMING,
    WFI_BLACKMAN,
    WFI_BLACKMAN_HARRIS,
    WFI_NUTTALL,
    WFI_KAISER,
    WFI_FLATTOP
};

namespace {

const double g_Pi(M_PI);

}

typedef double (*WindowFunctionType)(int i, double dt, double arg) ;

double GeneralizedHammingFunction(int i, double dt, double arg)
{
    double& a = arg;
    return a - (1.0 - a) * cos( (2*i+1) * dt / 2.0);
}

double RectangularFunction(int i, double dt, double arg)
{
    //return 0.54 - 0.46 * cos( (2*i+1) * dt / 2.0);
    return 1.0;
}

double HammingFunction(int i, double dt, double arg)
{
    //return 0.54 - 0.46 * cos( (2*i+1) * dt / 2.0);
    return GeneralizedHammingFunction(i, dt, 0.54);
}

double HanningFunction(int i, double dt, double arg)
{
    //return 0.5 - 0.5 * cos( (2*i+1) * dt / 2.0);
    return GeneralizedHammingFunction(i, dt, 0.50);
}

double BlackmanFunction(int i, double dt, double arg)
{
    return 0.42 - 0.5 * cos( (2*i+1) * dt / 2.0) + 0.08 * cos( (2*i+1) * dt);
}

double BlackmanHarrisFunction(int i, double dt, double arg)
{
    return 0.35875 - 0.48829 * cos( (2*i+1) * dt / 2.0) + 0.14128 * cos( (2*i+1) * dt) - 0.01168 * cos((2*i+1) * dt * 1.5);
}

double Factorial(int n)
{
    double ans = 1.0;
    for (int i = 1; i <= n; ++i) { ans *= i; }
    return ans;
}

double I0(double x)
{
    double ans = 0.0;
    for (int k = 0; k < 50; ++k)
    {
#if 1
        ans += pow(x/2, 2 * k) / POW2(Factorial(k));
#else
        double v = 1.0;
        for (int j = 1; j < k; ++j)
        {
            v *= POW2( x / (2 * j) );
        }
        ans += v;
#endif
    }
    return ans;
}

double KaiserFunction(int i, double dt, double a)
{
    return I0( g_Pi * a * sqrt(1 - POW2((2*i+1) * dt / g_Pi / 2.0 - 1) ) ) / I0 (g_Pi * a);
}

double NuttallFunction(int i, double dt, double arg)
{
    return 0.355768 - 0.487396 * cos( (2*i+1) * dt / 2.0) + 0.144232 * cos( (2*i+1) * dt) - 0.012604 * cos((2*i+1) * dt * 1.5);
}

double FlattopFunction(int i, double dt, double arg)
{
    const double f = 1 + 1.93 + 1.29 + 0.388 + 0.032;
    return (1 - 1.93 * cos((2*i+1) * dt / 2.0) + 1.29 * cos((2*i+1) * dt) - 0.388 * cos((2*i+1) * dt * 1.5) + 0.032 * cos(2.0 * (2*i+1) * dt)) / f;
}

WindowFunctionType GetWindowFunction(int type)
{
    WindowFunctionType function = reinterpret_cast<WindowFunctionType>(NULL);
    switch(type)
    {
    case WFI_RECTANGULAR:              function = RectangularFunction; break;
    case WFI_HANNING:             function = HanningFunction; break;
    case WFI_HAMMING:             function = HammingFunction; break;
    case WFI_GENERALIZED_HAMMING: function = GeneralizedHammingFunction; break;
    case WFI_BLACKMAN:            function = BlackmanFunction; break;
    case WFI_BLACKMAN_HARRIS:     function = BlackmanHarrisFunction; break;
    case WFI_NUTTALL:             function = NuttallFunction; break;
    case WFI_KAISER:              function = KaiserFunction; break;
    case WFI_FLATTOP:             function = FlattopFunction; break;
    }
    return function;
}

void GenerateWindow(double w[], size_t N, int type, double arg)
{
    WindowFunctionType function = GetWindowFunction(type);
    ASSERT(function != NULL);
    double dt = 2 * g_Pi / N;
    for (unsigned int i = 0; i < N; ++i)
    {
        w[i] = function(i, dt, arg);
    }
}

void GenerateWindow(mcon::Vector<double>& w, int type, double arg)
{
    const size_t N = w.GetLength();
    WindowFunctionType function = GetWindowFunction(type);
    ASSERT(function != NULL);

    double dt = 2 * g_Pi / N;
    for (unsigned int i = 0; i < N; ++i)
    {
        w[i] = function(i, dt, arg);
    }
}

void Rectangular(double w[], size_t N)        { GenerateWindow(w, N, WFI_RECTANGULAR, 0.0); }
void Rectangular(mcon::Vector<double>& w)     { GenerateWindow(w,    WFI_RECTANGULAR, 0.0); }
void Hanning(double w[], size_t N)            { GenerateWindow(w, N, WFI_HANNING , 0.0); }
void Hanning(mcon::Vector<double>& w)         { GenerateWindow(w,    WFI_HANNING , 0.0); }
void Hamming(double w[], size_t N)            { GenerateWindow(w, N, WFI_HAMMING , 0.0); }
void Hamming(mcon::Vector<double>& w)         { GenerateWindow(w,    WFI_HAMMING , 0.0); }
void GeneralizedHamming(double w[], size_t N, double a)     { GenerateWindow(w, N, WFI_GENERALIZED_HAMMING , a); }
void GeneralizedHamming(mcon::Vector<double>& w, double a)  { GenerateWindow(w,    WFI_GENERALIZED_HAMMING , a); }
void Blackman(double w[], size_t N)           { GenerateWindow(w, N, WFI_BLACKMAN, 0.0); }
void Blackman(mcon::Vector<double>& w)        { GenerateWindow(w,    WFI_BLACKMAN, 0.0); }
void BlackmanHarris(double w[], size_t N)     { GenerateWindow(w, N, WFI_BLACKMAN_HARRIS, 0.0); }
void BlackmanHarris(mcon::Vector<double>& w)  { GenerateWindow(w,    WFI_BLACKMAN_HARRIS, 0.0); }
void Nuttall(double w[], size_t N)            { GenerateWindow(w, N, WFI_NUTTALL, 0.0); }
void Nuttall(mcon::Vector<double>& w)         { GenerateWindow(w,    WFI_NUTTALL, 0.0); }
void Kaiser(double w[], size_t N, double a)   { GenerateWindow(w, N, WFI_KAISER, a); }
void Kaiser(mcon::Vector<double>& w, double a){ GenerateWindow(w,    WFI_KAISER, a); }
void Flattop(double w[], size_t N)            { GenerateWindow(w, N, WFI_FLATTOP, 0.0); }
void Flattop(mcon::Vector<double>& w)         { GenerateWindow(w,    WFI_FLATTOP, 0.0); }

}} // namespace masp { namespace window {
