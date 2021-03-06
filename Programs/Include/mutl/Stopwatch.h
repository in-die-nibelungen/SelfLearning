/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
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

#pragma once

#include <time.h>

#include "Clockwatch.h"

namespace mutl {

class Stopwatch
{
public:
    Stopwatch();
    ~Stopwatch();
    double Tick(void);
    void Correlate();

    inline double GetLastRecord(void) const
    {
       return m_LastScore;
    }
    inline bool IsCorrelated() const
    {
        return m_Correlated;
    }
    inline double Push()
    {
        return Tick();
    }
private:
    double m_LastScore;
    union {
        struct timespec ts; // Time Stamp
    } m_Base;
    double m_CorrelationFactor;
    bool m_Correlated;
    Clockwatch m_Clockwatch;
};

} // namespace mutl {
