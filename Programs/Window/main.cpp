#include <stdio.h>

#include "debug.h"
#include "Window.h"

static void test_hanning(void)
{
    int SamplingRate = 8000;
    int PassBand = 500;
    int StopBand = 1500;
    int TransferBandWidth = StopBand - PassBand;
    int cutOff = PassBand + TransferBandWidth/2;
    double coef[256];
    size_t N = 24;

    Container::Vector<double> han(N), ham(N), b(N), bh(N), n(N), k(N);
    masp::window::Hanning(coef, N);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i]);
    }
    N = 25;
    han.Reallocate(N);
    ham.Reallocate(N);
    b.Reallocate(N);
    bh.Reallocate(N);
    n.Reallocate(N);
    k.Reallocate(N);
    masp::window::Hanning(coef, N);
    masp::window::Hanning(han);
    masp::window::Hamming(ham);
    masp::window::Blackman(b);
    masp::window::BlackmanHarris(bh);
    masp::window::Nuttall(n);
    masp::window::Kaiser(k, 3.0);
    printf(",Hanning,Hamming,Blackman,BlackmanHarris,Nuttall,Kaiser\n");
    for (int i = 0; i < N; ++i)
    {
        printf("%d,%f,%f,%f,%f,%f,%f\n", i, han[i], ham[i], b[i], bh[i], n[i], k[i]);
    }
    return ;
}

int main(void)
{
    test_hanning();
    return 0;
}