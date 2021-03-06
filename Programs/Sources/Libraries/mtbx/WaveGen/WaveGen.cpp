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

#include "mtbx.h"
#include "debug.h"

#define DEFAULT_BASE_FREQUENCY 440.0f
#define DEFAULT_SAMPLING_RATE  48000
#define DEFAULT_DUTY 0.5f

namespace mtbx {

const double WaveGen::g_Pi(M_PI);

WaveGen::WaveGen()
    : m_SamplingRate(DEFAULT_SAMPLING_RATE),
    m_Tick(1.0f/DEFAULT_SAMPLING_RATE),
    m_Type(WT_SINE),
    m_Duty(0.5),
    m_FrequencyBase(DEFAULT_BASE_FREQUENCY),
    m_SweepEnable(false),
    m_SweepFactor(1.0f),
    m_FrequencyCurrent(DEFAULT_BASE_FREQUENCY),
    m_PhaseCurrent(0),
    m_ValueCurrent(0)
{
    SetWaveType(WT_SINE, DEFAULT_DUTY);
}

WaveGen::WaveGen(int samplingRate, double frequency, WaveType type, double duty)
    : m_SamplingRate(samplingRate),
    m_Tick(1.0f/samplingRate),
    m_Type(WT_SINE),
    m_Duty(0.5),
    m_FrequencyBase(frequency),
    m_SweepEnable(false),
    m_SweepFactor(1.0f),
    m_FrequencyCurrent(frequency),
    m_PhaseCurrent(0),
    m_ValueCurrent(0)
{
    // m_ValueCurrent is updated in SetWaveType
    SetWaveType(type, duty);
}

WaveGen::~WaveGen()
{
}

struct WaveGen::Variable WaveGen::GetVariable(void) const
{
    struct Variable var = {
        m_PhaseCurrent,
        m_ValueCurrent,
        m_FrequencyCurrent
    };

    return var;
}

bool WaveGen::SetWaveType(WaveGen::WaveType type, double duty)
{
    ASSERT(
        type == WT_SINE     ||
        type == WT_SAWTOOTH ||
        type == WT_TRIANGLE ||
        type == WT_SQUARE   ||
        type == WT_WATERSURFACE ||
        type == WT_NOISE_WHITE  ||
        type == WT_NOISE_PINK
    );

    m_Type = type;
    m_Duty = duty;

    switch(type)
    {
    case WT_SINE        : m_Generator = &WaveGen::Sine         ; break;
    case WT_SAWTOOTH    : m_Generator = &WaveGen::Sawtooth     ; break;
    case WT_TRIANGLE    : m_Generator = &WaveGen::Triangle     ; break;
    case WT_SQUARE      : m_Generator = &WaveGen::Square       ; break;
    case WT_WATERSURFACE: m_Generator = &WaveGen::WaterSurface ; break;
    case WT_NOISE_WHITE : m_Generator = &WaveGen::NoiseWhite   ; break;
    case WT_NOISE_PINK  : m_Generator = &WaveGen::NoisePink    ; break;
    }
    // Updating the current value so that it can return the right value when called GetValue() right after called SetWaveType().
    m_ValueCurrent = GenerateOne(m_PhaseCurrent);

    return true;
}

bool WaveGen::SetWaveParam(double base, WaveType type, double duty)
{
    SetWaveFrequency(base);
    return SetWaveType(type, duty);
}

void WaveGen::SetWaveFrequency(double base)
{
    ASSERT(base > 0.0);

    // Setting the current frequency, as well as the base frequency.
    m_FrequencyBase = base;
    m_FrequencyCurrent = base;
}

bool WaveGen::SetPhase(double phase)
{
    if (phase < 0.0f || phase >= 1.0)
    {
        return false;
    }
    // Updating the current phase and value.
    m_PhaseCurrent = phase;
    m_ValueCurrent = GenerateOne(m_PhaseCurrent);

    return true;
}

bool WaveGen::EnableSweep(void)
{
    if (m_SweepFactor == 1.0f)
    {
        m_SweepEnable = false;
    }
    else
    {
        m_SweepEnable = true;
    }

    return m_SweepEnable;
}

void WaveGen::DisableSweep(void)
{
    m_SweepEnable = false;
}

bool WaveGen::SetSweepParam(double targetFrequency, double duration, bool flag)
{
    ASSERT(duration > 0.0f);
    ASSERT(targetFrequency > 0.0f);

    if (m_FrequencyBase == targetFrequency)
    {
        m_SweepFactor = 1.0f;
        m_SweepEnable = false;
    }
    else
    {
        m_SweepFactor = pow(targetFrequency/m_FrequencyBase,
            1.0f/ (m_SamplingRate * duration) );
        m_SweepEnable = flag;
    }
    return m_SweepEnable;
}

void WaveGen::Reset(void)
{
    m_PhaseCurrent = 0.0f;
    m_FrequencyCurrent = m_FrequencyBase;
    m_SweepEnable = false;
    m_SweepFactor = 1.0f;
    m_ValueCurrent = GenerateOne(m_PhaseCurrent);
}

void WaveGen::SetSamplingRate(int fs)
{
    ASSERT(fs > 0);
    m_SamplingRate = fs;
    m_Tick = 1.0f / fs;
}

double WaveGen::Sine(double nt)
{
    return sin(2 * g_Pi * nt);
}

double WaveGen::Sawtooth(double nt)
{
    return (nt >= 0.5f) ? 2.0f * nt - 2.0f : 2.0f * nt;
}

double WaveGen::Triangle(double nt)
{
    double v = 4.0 * nt;
    if ( nt >= 0.75)
    {
        v = 4.0 * nt - 4.0;
    }
    else if ( nt >= 0.25)
    {
        v = -4.0 * nt + 2.0;
    }

    return v;
}

double WaveGen::Square(double nt)
{
    ASSERT(0.0 < m_Duty && m_Duty < 1.0);

    return (nt >= m_Duty) ? -1.0f : 1.0f;
}

double WaveGen::WaterSurface(double t)
{
    return 0.0;
}

double WaveGen::NoiseWhite(double t)
{
    return 0.0;
}

double WaveGen::NoisePink(double t)
{
    return 0.0;
}

double WaveGen::GenerateOne(double p)
{
    ASSERT(NULL != m_Generator);
    return (this->*m_Generator)(p);
}

void WaveGen::GenerateWaveform(mcon::Vector<double>& buffer, double amp)
{
    for (size_t i = 0; i < buffer.GetLength(); ++i, ++(*this))
    {
        buffer[i] = amp * GetValue();
    }
}

void WaveGen::GenerateWaveform(double buffer[], size_t n, double amp)
{
    for (unsigned int i = 0; i < n; ++i, ++(*this))
    {
        buffer[i] = amp * GetValue();
    }
}

WaveGen WaveGen::operator++(int)
{
    WaveGen old = *this;
    *this += 1;
    return old;
}

WaveGen& WaveGen::operator++(void)
{
    *this += 1;
    return *this;
}

WaveGen WaveGen::operator+(int c) const
{
    double tick = c * m_Tick;
    double phase = m_PhaseCurrent;
    double freq = m_FrequencyCurrent;
    double factor = m_SweepEnable ? pow(m_SweepFactor, c) : 1.0f;

    Update(tick, phase, freq, factor);

    WaveGen ret = *this;
    ret.m_FrequencyCurrent = freq;
    ret.SetPhase(phase); // m_ValueCurrent is updated in SetPhase

    return ret;
}

WaveGen& WaveGen::operator+=(int c)
{
    double tick = c * m_Tick;
    double factor = m_SweepEnable ? pow(m_SweepFactor, c) : 1.0f;
    Update(tick, m_PhaseCurrent, m_FrequencyCurrent, factor);
    m_ValueCurrent = GenerateOne(m_PhaseCurrent);
    return *this;
}

WaveGen  WaveGen::operator--(int)
{
    WaveGen old = *this;
    *this -= 1;
    return old;
}

WaveGen& WaveGen::operator--(void)
{
    *this -= 1;
    return *this;
}

WaveGen  WaveGen::operator-(int c) const
{
    return *this + (-c);
}

WaveGen& WaveGen::operator-=(int c)
{
    *this += (-c);
    return *this;
}

WaveGen  WaveGen::operator[](int c)
{
    return *this + c;
}

void WaveGen::Update(const double& tick, double& phase, double& freq, const double& factor)
{
    phase += (tick * freq);
    phase -= static_cast<int>(phase);
    freq  *= factor;
}

double WaveGen::GetValue(void) const
{
    return m_ValueCurrent;
}

int WaveGen::GetSamplingRate(void) const
{
    return m_SamplingRate;
}

} // namespace mtbx {
