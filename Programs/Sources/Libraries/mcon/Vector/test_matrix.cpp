#include "mcon.h"

void test_matrix_api(void)
{
    const size_t length = 6;
    mcon::Vector<TestType> dvec(length);
    for (size_t i = 0; i < length; ++i)
    {
        dvec[i] = i + 1;
    }
    mcon::Matrix<TestType> m;

    CHECK_VALUE(m.IsNull(), true);

    m = dvec.Transpose();

    CHECK_VALUE(m.GetRowLength(), length);
    CHECK_VALUE(m.GetColumnLength(), 1);
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(m[i][0], i+1);
    }
    mcon::Matrix<TestType> m1(dvec.ToMatrix());

    CHECK_VALUE(m1.GetRowLength(), 1);
    CHECK_VALUE(m1.GetColumnLength(), length);
    for (size_t i = 0; i < length; ++i)
    {
        CHECK_VALUE(m1[0][i], i+1);
    }
}
