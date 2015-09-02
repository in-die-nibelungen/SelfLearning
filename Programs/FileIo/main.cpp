#include <stdlib.h>
#include <stdio.h>

#include "FileIo.h"
#include "WaveGen.h"
#include "types.h"
#include "debug.h"

static void test_test(void)
{
    FileIo wave;

    wave.Test();
}

static void test_read(void)
{
    size_t size;
    int16_t* buffer;
    FileIo wave;
    wave.Read("ding.wav", &buffer, &size);

    printf("Sizeof(PcmFormat)=%d\n", sizeof(FileIo::PcmFormat));
    printf("Sizeof(MetaData)=%d\n", sizeof(FileIo::MetaData));
    {
        int32_t fs, ch, bit;

        wave.GetMetaData(&fs, &ch, &bit);

        CHECK_VALUE_INT( fs, 44100);
        CHECK_VALUE_INT( ch,     2);
        CHECK_VALUE_INT(bit,    16);
    }

    {
        struct FileIo::MetaData metaData = wave.GetMetaData();

        CHECK_VALUE_INT(metaData.samplingRate, 44100);
        CHECK_VALUE_INT(metaData.numChannels ,     2);
        CHECK_VALUE_INT(metaData.bitDepth    ,    16);
    }
}

static void test_write(void)
{
    WaveGen wg;
    int32_t freq = 440;
    int32_t fs = 48000;
    int32_t duration = 10;
    int32_t ch = 1;
    int32_t depth = sizeof(int16_t) * 8;
    int32_t amp = 32767;
    int32_t multi = 20;
    size_t size = duration * fs * ch * depth / 8;
    int16_t* buffer = (int16_t*)malloc(size);

    wg.SetWaveType(WaveGen::WT_SINE);
    wg.SetSamplingRate(fs);
    wg.SetWaveFrequency(freq);
    wg.Reset();
    wg.SetSweepParam(freq*multi, (double)duration, true);

    for (int i = 0; i < duration * fs; ++i, ++wg)
    {
        buffer[i] = static_cast<int16_t>(amp * wg.GetValue());
    }

    {
        char fname[256];
        FileIo wave(fs, ch, depth);
        //wave.SetMetaData(fs, ch, depth);
        sprintf(fname, "sweep_%d-%d.wav", freq, freq*multi);
        wave.Write(fname, buffer, size);
    }
    free(buffer);
}

int main(void)
{
    test_test();
    test_read();
    test_write();

    return 0;
}
