
#include <math.h>
#include <sys/types.h>

#include "debug.h"
#include "Iir.h"

#define POW2(x) ((x)*(x))


namespace masp {
namespace iir {

static const double g_Pi(M_PI);

Biquad::Biquad()
{
    Initialize();
};

Biquad::Biquad(double Q, double fc1, double fc2, int type, int fs)
{
    Initialize();

    CalculateCoefficients(Q, fc1, fc2, type, fs);
}

void Biquad::Initialize(void)
{
    double* a = m_Coefficients.a;
    double* b = m_Coefficients.b;
    double* x = m_Context.x;
    double* y = m_Context.y;
    // Clear context data.
    y[0] = 0.0; y[1] = 0.0;
    x[0] = 0.0; x[1] = 0.0;
    // Set through-pass coefficients.
    b[0] = 1.0; b[1] = 0.0; b[2] = 0.0;
    a[0] = 0.0; a[1] = 0.0;
}

void Biquad::CalculateCoefficients(double Q, double _fc1, double _fc2, int type, int fs)
{
    double fc1 = ConvertD2A(_fc1, fs);
    double fc2 = ConvertD2A(_fc2, fs);

    double* a = m_Coefficients.a;
    double* b = m_Coefficients.b;
    switch(type)
    {
    case LPF:
        {
            const double c = POW2(2.0 * g_Pi * fc1);
            const double d = 1 + 2 * g_Pi * fc1 / Q + c;
            b[0] = c / d;
            b[1] = 2.0 * c / d;
            b[2] = c / d;
            a[0] = (2.0 * c - 2.0) / d;
            a[1] = (1 - 2 * g_Pi * fc1 / Q + c) / d;
        }
        break;
    case HPF:
        {
            const double c = POW2(2.0 * g_Pi * fc1);
            const double d = 1 + 2 * g_Pi * fc1 / Q + c;
            b[0] = 1.0 / d;
            b[1] = -2.0 * c / d;
            b[2] = 1.0 / d;
            a[0] = (2.0 * c - 2.0) / d;
            a[1] = (1 - 2 * g_Pi * fc1 / Q + c) / d;
        }
        break;
    case BPF:
        {
            const double c = POW2(2.0 * g_Pi) * fc1 * fc2;
            const double d = 2.0 * g_Pi * (fc2 - fc1);
            const double e = 1 + d + c;
            b[0] = d / e;
            b[1] = 0.0;
            b[2] = -d / e;
            a[0] = (2.0 * c - 2.0) / e;
            a[1] = (1 - d + c) / e;
        }
        break;
    case BEF:
        {
            const double c = POW2(2.0 * g_Pi) * fc1 * fc2;
            const double d = 2.0 * g_Pi * (fc2 - fc1);
            const double e = 1 + d + c;
            b[0] = (c + 1.0) / e;
            b[1] = (2.0 * c - 2.0) / e;
            b[2] = (c + 1.0) / e;
            a[0] = (2.0 * c - 2.0) / e;
            a[1] = (1 - d + c) / e;
        }
        break;
    }
    printf("b0=%f,b1=%f,b2=%f,a0=%f,a1=%f\n", b[0], b[1], b[2], a[0], a[1]);
}

double Biquad::ApplyFilter(double xn)
{
    double* a = m_Coefficients.a;
    double* b = m_Coefficients.b;
    double* x = m_Context.x;
    double* y = m_Context.y;

    double yn = b[0] * xn
               + b[1] * x[0]
               + b[2] * x[1]
               - a[0] * y[0]
               - a[1] * y[1];
    y[1] = y[0];
    y[0] = yn;
    x[2] = x[1];
    x[1] = x[0];
    x[0] = xn;
    return yn;
}

double Biquad::ConvertD2A(double fd, int fs)
{
    return tan(g_Pi * fd / fs) / (2.0 * g_Pi);
}

} // namespace iir {
} // namespace masp {
