
#include "debug.h"
#include "mcon.h"

static double _fabs(double v)
{
    return (v < 0) ? -v : v;
}

void ShowVectord(const mcon::Vectord& v)
{
    const size_t n = v.GetLength();
    for (size_t i = 0; i < n; ++i )
    {
        printf("\t%g", v[i]);
        if ( (i % 8) == 7 )
        {
            printf("\n");
        }
    }
    printf("\n");
}

void test_Vectord(void)
{
    LOG("* [Empty Vector]\n");
    mcon::Vectord dvec;

    // Zero length
    CHECK_VALUE(dvec.GetLength(), 0);
    // IsNull() is true.
    CHECK_VALUE(dvec.IsNull(), true);

    LOG("* [Resize]\n");
    const int length = 6;
    dvec.Resize(length);
    CHECK_VALUE(dvec.GetLength(), length);
    // IsNull() is false.
    CHECK_VALUE(dvec.IsNull(), false);

    for (size_t i = 0; i < length; ++i)
    {
        dvec[i] = i;
    }
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i);
    }

    LOG("* [operator+=(double)]\n");
    // operator+=(T)
    dvec += 1;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], i+1);
    }
    // operator*=(T)
    LOG("* [operator*=(double)]\n");
    dvec *= 10;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*10);
    }
    // operator/=(T)
    LOG("* [operator/=(double)]\n");
    dvec /= 5;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1)*2);
    }
    // operator-=(T)
    LOG("* [operator-=(double)]\n");
    dvec -= 5;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (static_cast<int>(i)+1)*2-5);
    }
    LOG("* [Copy]\n");
    mcon::Vectord dvec2(length*2);

    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = -(i+1);
    }
    // Copy
    dvec2.Copy(dvec);
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        if (0 <= i && i < length)
        {
            CHECK_VALUE(dvec2[i], (static_cast<int>(i)+1)*2-5);
        }
    }
    LOG("* [operator=]\n");
    // Substitution
    dvec2 = dvec;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec2[i], (static_cast<int>(i)+1)*2-5);
    }
    dvec = 10;
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }

    LOG("* [operator?=]\n");
    dvec += dvec2;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = (i & 1) ? 1.0 : 2.0;
    }

    dvec *= dvec2;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], ((i+1) + 10) * ((i & 1) ? 1.0 : 2.0));
    }
    dvec /= dvec2;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], (i+1) + 10);
    }
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        dvec2[i] = i + 1;
    }
    dvec -= dvec2;
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(dvec[i], 10);
    }
    LOG("* [Carveout]\n");
    for (size_t i = 0; i < dvec.GetLength(); ++i)
    {
        dvec[i] = i + 1;
    }
    // dvec = {1, 2, 3, 4, 5, 6};
    dvec2 = dvec(0, 0);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(-1, 1);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(dvec.GetLength(), 1);
    CHECK_VALUE(dvec2.IsNull(), true);
    dvec2 = dvec(dvec.GetLength()-1, 5);
    CHECK_VALUE(dvec2.IsNull(), false);
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], 6);
    }

    dvec2 = dvec(1, 3);
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }
    // Fifo
    LOG("* [Fifo]\n");
    double v = dvec2.Fifo(5);
    CHECK_VALUE(v, 2);
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+3);
    }
    // Unshift
    LOG("* [Unshift]\n");
    v = dvec2.Unshift(2);
    CHECK_VALUE(v, 5);
    for (size_t i = 0; i < dvec2.GetLength(); ++i)
    {
        CHECK_VALUE(dvec2[i], i+2);
    }

    // Maximum/Minimum
    LOG("* [GetMaximum]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = - __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k - n/2;
                if (  ans < v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMaximum());
            // 降順
            ans = - __DBL_MAX__;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n/2 - k;
                if (  ans < v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMaximum());
        }
    }

    LOG("* [GetMinimum]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k - n/2;
                if ( ans > v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMinimum());
            // 降順
            ans = __DBL_MAX__;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n/2 - k;
                if (  ans > v[k] )
                {
                    ans = v[k];
                }
            }
            CHECK_VALUE(ans, v.GetMinimum());
        }
    }
    LOG("* [GetMaximumAbsolute]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = 0;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = (k + 1) * (k & 1 ? -1 : 1);
                if (  ans < _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
            // 昇順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMaximumAbsolute());

            // 降順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n - k;
                if ( ans < _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
            // 降順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMaximumAbsolute());
        }
    }
    LOG("* [GetMinimumAbsolute]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            double ans = __DBL_MAX__;
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            // 昇順
            for ( int k = 0; k < n; ++k )
            {
                v[k] = (k + 1) * (k & 1 ? -1 : 1);
                if (  ans > _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
            // 昇順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMinimumAbsolute());

            // 降順
            ans = __DBL_MAX__;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = n - k;
                if ( ans > _fabs(v[k]) )
                {
                    ans = _fabs(v[k]);
                }
            }
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
            // 降順 (符号が逆)
            // 結果は変わらない
            v *= -1;
            CHECK_VALUE(ans, v.GetMinimumAbsolute());
        }
    }
    LOG("* [GetSum/Average/GetNorm]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            double sum = 0;
            double norm = 0;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k + 1 - n/4;
                sum += v[k];
                norm += v[k]*v[k];
            }
            double ave = sum / n;
            norm = sqrt(norm);
            CHECK_VALUE(sum , v.GetSum());
            CHECK_VALUE(ave , v.GetAverage());
            CHECK_VALUE(norm, v.GetNorm());
        }
    }
    LOG("* [Dot]\n");
    {
        const int lens[] = {1, 2, 3, 4, 5, 7, 8, 9, 31, 32, 33, 63, 64, 65};
        for ( unsigned int i = 0; i < sizeof(lens)/sizeof(int); ++i )
        {
            const int n = lens[i];
            LOG("    Lenght=%d:\n", n);
            mcon::Vectord v(n);
            mcon::Vectord w(n);
            double dot = 0;
            for ( int k = 0; k < n; ++k )
            {
                v[k] = k + 1 - n/4;
                w[k] = 3*n/4 - k - 1;
                dot += v[k]*w[k];
            }
            CHECK_VALUE(dot, v.GetDotProduct(w));
        }
    }
    LOG("* [ToMatrix]\n");
    for (size_t i = 0; i < dvec.GetLength(); ++i)
    {
        dvec[i] = i + 1;
    }
    const mcon::Matrix<double> m = dvec.ToMatrix();
    CHECK_VALUE(m.GetRowLength(), 1);
    CHECK_VALUE(m.GetColumnLength(), dvec.GetLength());
    for (size_t i = 0; i < dvec.GetLength(); ++i)
    {
        CHECK_VALUE(m[0][i], i+1);
    }
    const mcon::Matrix<double> mt = dvec.ToMatrix().Transpose();

    CHECK_VALUE(mt.GetRowLength(), dvec.GetLength());
    CHECK_VALUE(mt.GetColumnLength(), 1);
    for (size_t i = 0; i < dvec.GetLength(); ++i)
    {
        CHECK_VALUE(mt[i][0], i+1);
    }
    LOG("END\n");
}
