#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "debug.h"
#include "Ft.h"
#include "mtbx.h"
#include "mfio.h"

#define POW2(v) ((v)*(v))

#define FREQ_BASE 440.0
#define NUM_SAMPLES 4800

static void test_ft(void)
{
    double* buffer = NULL;
    double* fft = NULL, *ifft = NULL;
    int fs = 48000;
    int n = NUM_SAMPLES;
    mtbx::WaveGen wg(fs, FREQ_BASE, mtbx::WaveGen::WT_SINE, 1.0);
    wg.Reset();
    wg.SetSweepParam(FREQ_BASE*20, (double)n/fs, true);

    buffer = (double*)malloc(sizeof(double) * n);
    for (int i = 0; i < n; ++i, ++wg)
    {
        mtbx::WaveGen::Variable var = wg.GetVariable();
        buffer[i] = var.value;
        //printf("%f,%f\n", (double)i/fs, var.value);
    }
    LOG("Ft\n");
    fft = (double*)malloc(sizeof(double) * n * 2);
    double *real = fft;
    double *imag = fft + n;
    masp::ft::Ft(real, imag, buffer, n);

    {
        double df = (double)fs/n;
        printf("freq,gain,phase\n");
        for (int i = 1; i < n/2; ++i)
        {
            double gain = 10 * log10(POW2(real[i]) + POW2(imag[i]));
            double phase = atan(real[i]/imag[i]);
            printf("%f,%f,%f\n", i*df, gain, phase);
        }
    }
    ifft = (double*)malloc(sizeof(double) * n * 2);
    double *td = ifft;
    printf("Ift\n");
    masp::ft::Ift(td, real, imag, n);
    {
        printf("time,orig,td,td1\n");
        for (int i = 1; i < n/10; ++i)
        {
            printf("%d,%f,%f,%f\n", i, buffer[i],td[i], 0.0);
        }
    }

    free(buffer);
    free(fft);
    free(ifft);
}

static void test_ft_buffer(void)
{
    int fs = 48000;
    int n = NUM_SAMPLES;
    mtbx::WaveGen wg(fs, FREQ_BASE, mtbx::WaveGen::WT_SINE);
    wg.Reset();
    wg.SetSweepParam(FREQ_BASE*20, (double)n/fs, true);

    mcon::Vector<double> buffer(n);

    wg.GenerateWaveform(buffer);

    mcon::Matrix<double> fft(2, n);

    masp::ft::Ft(fft, buffer);

    {
        double df = (double)fs/n;
        mcon::Matrix<double> gp(2, n);

        masp::ft::ConvertToGainPhase(gp, fft);

        printf("freq,gain,phase\n");
        for (int i = 1; i < n; ++i)
        {
            printf("%f,%f,%f\n", i*df, gp[0][i], gp[1][i]);
        }
        printf("Max gain: %f\n", gp[0].GetMaximum());

        FILE* fp = fopen("fft.csv", "w");
        if (NULL != fp)
        {
            printf("i,fft_real,fft_imag,gain,phase\n");
            for (int i = 1; i < n; ++i)
            {
                fprintf(fp, "%d,%f,%f%f,%f\n", i, fft[0][i], fft[1][i], gp[0][i], gp[1][i]);
            }
            fclose(fp);
        }
    }
    mcon::Vector<double> ifft(n);
    masp::ft::Ift(ifft, fft);
    {
        printf("time,orig,td,td1\n");
        for (int i = 1; i < n/10; ++i)
        {
            printf("%d,%f,%f,%f\n", i, buffer[i],ifft[i], 0.0);
        }
    }
}

static int sign(double v)
{
    return (v < 0) ? -1 : 1;
}

static void test_gp_complex(void)
{
    const char* fname = "sweep_440-3520_1s.wav";
    mfio::Wave wave;
    mcon::Vector<double> buffer;
    wave.Read(fname, buffer);
    mcon::Matrix<double> complex;
    mcon::Matrix<double> gp;
    mcon::Matrix<double> icomplex;

    LOG("Ft\n");
    masp::ft::Ft(complex, buffer);
    LOG("Polar\n");
    masp::ft::ConvertToPolarCoords(gp, complex);
    LOG("Complex\n");
    masp::ft::ConvertToComplex(icomplex, gp);
    for ( int i = 0; i < icomplex.GetColumnLength(); ++i)
    {
        icomplex[0][i] = fabs(icomplex[0][i]) * sign(complex[0][i]);
        icomplex[1][i] = fabs(icomplex[1][i]) * sign(complex[1][i]);
    }
    LOG("Saving\n");
    {
        const int n = buffer.GetLength();
        mcon::Matrix<double> matrix(4, n);
        matrix[0] = complex[0];
        matrix[1] = complex[1];
        matrix[2] = icomplex[0];
        matrix[3] = icomplex[1];
        mfio::Csv::Write("sweep_440-3520_1s.csv", matrix);
    }
}

static void test_fft(void)
{
    const int fs = 4800;
    const int n = 128; //fs;
    mtbx::WaveGen wg(fs, FREQ_BASE, mtbx::WaveGen::WT_SINE);
    wg.Reset();

    mcon::Vector<double> buffer(n);
    wg.GenerateWaveform(buffer);

    mcon::Matrix<double> ft(2, n);
    mcon::Matrix<double> fft(2, n);

    masp::ft::Ft (ft , buffer);
    masp::ft::Fft(fft, buffer);
    // Check
#define POW2(v) ((v)*(v))
    {
        double err = 0;
        for ( int i = 0; i < n; ++i )
        {
            err += POW2(ft[0][i] - fft[0][i]);
            err += POW2(ft[1][i] - fft[1][i]);
        }
        err = sqrt(err);
        CHECK_VALUE(err, 0);
    }
#undef POW2
    // Save to check.
    {
        mcon::Matrix<double> saved(5, n);
        const double df = (double)fs/n;

        for ( int i = 0; i < n; ++i )
        {
            saved[0][i] = df * i;
        }
        saved[1] = ft[0];
        saved[2] = fft[0];
        saved[3] = ft[1];
        saved[4] = fft[1];

        {
            mcon::Matrix<double> gp(2, n);
            masp::ft::ConvertToGainPhase(gp, ft);
            saved[1] = gp[0];
            saved[3] = gp[1];

            masp::ft::ConvertToGainPhase(gp, fft);
            saved[2] = gp[0];
            saved[4] = gp[1];
        }

        mfio::Csv csv("fft.csv");

        csv.Write(",freq,gain1,gain2,phase1,phase2\n");
        csv.Write(saved);
        csv.Close();
    }
    return ;

    // Not, yet.
    mcon::Vector<double> ifft(n);
    masp::ft::Ift(ifft, fft);
    {
        printf("time,orig,td,td1\n");
        for (int i = 1; i < n/10; ++i)
        {
            printf("%d,%f,%f,%f\n", i, buffer[i],ifft[i], 0.0);
        }
    }
}

void test_Ft(void)
{
    test_ft();
    test_ft_buffer();
    test_gp_complex();
    test_fft();
}
