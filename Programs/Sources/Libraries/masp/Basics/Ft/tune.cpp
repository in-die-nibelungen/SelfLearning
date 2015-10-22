#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string>

#include "Fft.h"
#include "FileIo.h"
#include "Vector.h"
#include "BenchUtil.h"

static void tune_ft(void)
{
    std::string fbody("sweep_440-3520_1s");
    int fs;

    mcon::Vector<double> sweep;
    {
        std::string wfile = fbody + std::string(".wav");
        mfio::Wave wave;
        wave.Read(wfile.c_str(), sweep);
        fs = wave.GetSamplingRate();
    }
    LOG("SamplingRate: %d\n", fs);
    LOG("Length: %d\n", sweep.GetLength());

    status_t status;
    const int n = sweep.GetLength();
    mcon::Matrix<double> complex(2, n);
    mcon::Matrix<double> gp(2, n);
    mcon::Vector<double> ift(n);
    mbut::Stopwatch sw;
    status = Fft::Ft(complex, sweep);
    if (NO_ERROR != status)
    {
        LOG("An error occured: error=%d\n", status);
    }
    LOG("Time consumed for Ft : %f [sec]\n", sw.Tick());
    status = Fft::ConvertToPolarCoords(gp, complex);
    if (NO_ERROR != status)
    {
        LOG("An error occured: error=%d\n", status);
    }
    LOG("Time consumed for Gp : %f [sec]\n", sw.Tick());
    status = Fft::Ift(ift, complex);
    if (NO_ERROR != status)
    {
        LOG("An error occured: error=%d\n", status);
    }
    LOG("Time consumed for Ift: %f [sec]\n", sw.Tick());
    {
        std::string csv = fbody + std::string(".csv");

        FILE* fp = fopen(csv.c_str(), "w");

        if (NULL != fp)
        {
            for (int i = 0; i < complex.GetColumnLength(); ++i)
            {
                fprintf(fp, "%d,%g,%g\n", i, complex[0][i], complex[1][i]);
            }
            fprintf(fp, "\n");
            const double df = static_cast<double>(fs) / sweep.GetLength();
            for (int i = 0; i < gp.GetColumnLength(); ++i)
            {
                fprintf(fp, "%g,%g,%g\n", i*df, gp[0][i], gp[1][i]);
            }
            fprintf(fp, "\n");
            for (int i = 0; i < ift.GetLength(); ++i)
            {
                fprintf(fp, "%d,%g,%g,%g\n", i, sweep[i], ift[i], sqrt( pow(sweep[i] - ift[i], 2) ) );
            }
            fclose(fp);
        }
    }
}

static void tunes(void)
{
    tune_ft();
}