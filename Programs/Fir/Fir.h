
#pragma once

#include <sys/types.h>

#include "Vector.h"

namespace masp {
namespace fir {

//void GetCoefficients(double coef[], size_t N, double fe, int filterType, int functionId);
//void HpfSinc(double coef[], size_t N, double fe);
void FilterSinc(double coef[], size_t N, double fe);
void FilterLanczos(double coef[], size_t N, double fe, double n);

void GetCoefficientsLpfSinc(mcon::Vector<double>& coef, double fe);
void GetCoefficientsHpfSinc(mcon::Vector<double>& coef, double fe);
void GetCoefficientsBpfSinc(mcon::Vector<double>& coef, double fe1, double fe2);
void GetCoefficientsBefSinc(mcon::Vector<double>& coef, double fe1, double fe2);
void GetCoefficientsLpfLanczos(mcon::Vector<double>& coef, double fe, double n);
void GetCoefficientsHpfLanczos(mcon::Vector<double>& coef, double fe, double n);
void GetCoefficientsBpfLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n);
void GetCoefficientsBefLanczos(mcon::Vector<double>& coef, double fe1, double fe2, double n);

inline void GetCoefficientsLpfSinc(mcon::Vector<double>& coef, int cutOff, int samplingRate)
{
    GetCoefficientsLpfSinc(coef, static_cast<double>(cutOff)/samplingRate);
}
inline void GetCoefficientsHpfSinc(mcon::Vector<double>& coef, int cutOff, int samplingRate)
{
    GetCoefficientsHpfSinc(coef, static_cast<double>(cutOff)/samplingRate);
}
inline void GetCoefficientsBpfSinc(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate)
{
    GetCoefficientsBpfSinc(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate);
}
inline void GetCoefficientsBefSinc(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate)
{
    GetCoefficientsBefSinc(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate);
}
inline void GetCoefficientsLpfLanczos(mcon::Vector<double>& coef, int cutOff, int samplingRate, double n)
{
    GetCoefficientsLpfLanczos(coef, static_cast<double>(cutOff)/samplingRate, n);
}
inline void GetCoefficientsHpfLanczos(mcon::Vector<double>& coef, int cutOff, int samplingRate, double n)
{
    GetCoefficientsHpfLanczos(coef, static_cast<double>(cutOff)/samplingRate, n);
}
inline void GetCoefficientsBpfLanczos(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate, double n)
{
    GetCoefficientsBpfLanczos(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate, n);
}
inline void GetCoefficientsBefLanczos(mcon::Vector<double>& coef, int cutOff1, int cutOff2, int samplingRate, double n)
{
    GetCoefficientsBefLanczos(coef, static_cast<double>(cutOff1)/samplingRate, static_cast<double>(cutOff2)/samplingRate, n);
}

// This is for Hanning, not the others.
int GetNumOfTapps(double delta);
inline int GetNumOfTapps(double passband, int samplingRate)
{
    GetNumOfTapps(static_cast<double>(passband)/samplingRate);
}

status_t Convolution(mcon::Vector<double>& out, const mcon::Vector<double>& in, const mcon::Vector<double>& impluse);


} // namespace fir {
} // namespace masp {
