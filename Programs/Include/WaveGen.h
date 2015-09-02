
#ifndef _WAVE_GEN_H_
#define _WAVE_GEN_H_

#include "types.h"
#include "Buffer.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */

class WaveGen
{
public:

    enum WaveType
    {
        WT_SINE,
        WT_SAWTOOTH,
        WT_TRIANGLE,
        WT_SQUARE,
        WT_WATERSURFACE,
        WT_NOISE_WHITE,
        WT_NOISE_PINK,
        WT_RECTANGULAR = WT_SQUARE,
    };

    struct Variable {
        f64 phase;
        f64 value;
        f64 frequency;
    };

    WaveGen();
    WaveGen(s32 samplingRate, f64 frequency, WaveType type, f64 duty);
    WaveGen(s32 samplingRate, f64 frequency, WaveType type)
    {
        WaveGen(samplingRate, frequency, type, 0.5f);
    }
    ~WaveGen();

    void Reset(void);

    struct Variable GetVariable(void) const;
    f64  GetValue(void) const ;
    bool SetPhase(f64 phase);
    s32  GetSamplingRate(void) const ;
    void SetSamplingRate(s32 samplingRate);
    void SetWaveFrequency(f64 frequency);
    bool SetWaveType(WaveType type, f64 duty=0.5f);
    bool SetWaveParam(f64 frequency, WaveType type, f64 duty=0.5f);
    bool SetSweepParam(f64 targetFrequency, f64 duration, bool enable=false);
    bool EnableSweep(void);
    void DisableSweep(void);

    WaveGen& operator++(void);
    WaveGen  operator++(int);
    WaveGen  operator+(int) const;
    WaveGen& operator+=(int);
    WaveGen& operator--(void);
    WaveGen  operator--(int);
    WaveGen  operator-(int) const;
    WaveGen& operator-=(int);
    WaveGen  operator[](int);

    // Aliases
    bool SetBaseFrequency(f64 frequency) { SetWaveFrequency(frequency); }

    void GenerateValue(f64 buffer[], size_t n, f64 amplitude = 1.0f);
    void GenerateValue(Container::Vector<f64>& buffer, size_t n, f64 amplitude = 1.0f);

private:

    // GenerateOners
    f64  Sine(f64);
    f64  Sawtooth(f64);
    f64  Triangle(f64);
    f64  Square(f64);
    f64  WaterSurface(f64);
    f64  NoiseWhite(f64);
    f64  NoisePink(f64);
    f64  GenerateOne(f64);
    f64  (WaveGen::*m_Generator)(f64);

    // Updates the internal states.
    static void Update(const f64& tick, f64& phase, f64& freq, const f64& factor);

    // Parameters for math
    static const f64 g_Pi;

    // Parameters for quantizing
    s32  m_SamplingRate;
    f64  m_Tick;

    // Parameters for generating waveform
    WaveType m_Type;
    f64  m_Duty;
    f64  m_FrequencyBase;

    bool m_SweepEnable;
    f64  m_SweepFactor;

    // Working variables.
    f64  m_FrequencyCurrent;
    f64  m_PhaseCurrent;
    f64  m_ValueCurrent;
};

#endif // #ifndef _WAVE_GEN_H_
