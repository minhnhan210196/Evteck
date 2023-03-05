#ifndef BUTTERWORTHFILTER_H
#define BUTTERWORTHFILTER_H

#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "QVector"
class FIRFilterImplementation{
    double *z;
public:
    FIRFilterImplementation(int order)
    {
        this->z = new double[order];
    }

public:
    double compute(double input, double *a,uint16_t length)
    {
        // computes y(t) = a0*x(t) + a1*x(t-1) + a2*x(t-2) + ... an*x(t-n)
        double result = 0;

        for (int t = length - 1; t >= 0; t--)
        {
            if (t > 0)
            {
                this->z[t] = this->z[t - 1];
            }
            else
            {
                this->z[t] = input;
            }
            result += a[t] * this->z[t];
        }
        return result;
    }
};

class IIRFilterImplementation{
protected:
    double *z;
public:
    IIRFilterImplementation(int order)
    {
        this->z = (double*)malloc(order);
    }
    double compute(double input, double *a,uint16_t length)
            {
                // computes y(t) = x(t) + a1*y(t-1) + a2*y(t-2) + ... an*y(t-n)
                // z-transform: H(z) = 1 / (1 - sum(1 to n) [an * y(t-n)])
                // a0 is assumed to be 1
                // y(t) is not stored, so y(t-1) is stored at z[0],
                // and a1 is stored as coefficient[0]

                double result = input;

                for (int t = 0; t < length; t++)
                {
                    result += a[t] * this->z[t];
                }
                for (int t = length - 1; t >= 0; t--)
                {
                    if (t > 0)
                    {
                        this->z[t] = this->z[t - 1];
                    }
                    else
                    {
                        this->z[t] = result;
                    }
                }
                return result;
            }
};

class LowpassFilterButterworthSection{
protected:
    FIRFilterImplementation *firFilter;
    IIRFilterImplementation *iirFilter;

    double a[3]; //3
    double b[2];//2
    double gain;

public:
    LowpassFilterButterworthSection
                   (double cutoffFrequencyHz, double k, double n, double Fs, int order)
            {
                firFilter = new FIRFilterImplementation(order);
                iirFilter = new IIRFilterImplementation(order);

                // compute the fixed filter coefficients
                double omegac = 2.0 * Fs * tan(3.14 * cutoffFrequencyHz / Fs);
                double zeta = -cos(3.14 * (2.0 * k + n - 1.0) / (2.0 * n));

                // fir section
                this->a[0] = omegac * omegac;
                this->a[1] = 2.0 * omegac * omegac;
                this->a[2] = omegac * omegac;

                //iir section
                //normalize coefficients so that b0 = 1,
                //and higher-order coefficients are scaled and negated
                double b0 = (4.0 * Fs * Fs) + (4.0 * Fs * zeta * omegac) + (omegac * omegac);
                this->b[0] = ((2.0 * omegac * omegac) - (8.0 * Fs * Fs)) / (-b0);
                this->b[1] = ((4.0 * Fs * Fs) -
                             (4.0 * Fs * zeta * omegac) + (omegac * omegac)) / (-b0);
                this->gain = 1.0 / b0;
            }

    double compute(double input)
            {
                // compute the result as the cascade of the fir and iir filters
                return this->iirFilter->compute
                       (this->firFilter->compute(this->gain * input, this->a,3), this->b,2);
            }
};

class LowpassFilterButterworthImplementation
{
protected:
    QVector<LowpassFilterButterworthSection> section;
public:
    LowpassFilterButterworthImplementation(double cutoffFrequencyHz, int numSections, double Fs, int order);
    double compute(double input);

};

class HighpassFilterButterworthSection{
protected:
    FIRFilterImplementation *firFilter;
    IIRFilterImplementation *iirFilter;
protected:
    double a[3];
    double b[2];
    double gain;

public:
    HighpassFilterButterworthSection(double cutoffFrequencyHz, double k, double n, double Fs, int order)
    {
        firFilter = new FIRFilterImplementation(order);
        iirFilter = new IIRFilterImplementation(order);

        // pre-warp omegac and invert it
        double omegac = 1.0 / (2.0 * Fs * tan(3.14 * cutoffFrequencyHz / Fs));

        // compute zeta
        double zeta = -cos(3.14 * (2.0 * k + n - 1.0) / (2.0 * n));

        // fir section
        this->a[0] = 4.0 * Fs * Fs;
        this->a[1] = -8.0 * Fs * Fs;
        this->a[2] = 4.0 * Fs * Fs;

        //iir section
        //normalize coefficients so that b0 = 1
        //and higher-order coefficients are scaled and negated
        double b0 = (4.0 * Fs * Fs) + (4.0 * Fs * zeta / omegac) + (1.0 / (omegac * omegac));
        this->b[0] = ((2.0 / (omegac * omegac)) - (8.0 * Fs * Fs)) / (-b0);
        this->b[1] = ((4.0 * Fs * Fs)
                   - (4.0 * Fs * zeta / omegac) + (1.0 / (omegac * omegac))) / (-b0);
        this->gain = 1.0 / b0;
    }

public:
    double compute(double input)
    {
        // compute the result as the cascade of the fir and iir filters
        return this->iirFilter->compute
            (this->firFilter->compute(this->gain * input, this->a,3), this->b,2);
    }
};

class HighpassFilterButterworthImplementation{
protected:
    QVector<HighpassFilterButterworthSection> section;
public:
    HighpassFilterButterworthImplementation
    (double cutoffFrequencyHz, int numSections, double Fs, int order)
    {
        for (int i = 0; i < numSections; i++)
        {
            HighpassFilterButterworthSection new_section(cutoffFrequencyHz, i + 1, numSections * 2, Fs, order);
            this->section.append(new_section);
        }
    }
    double compute(double input)
    {
        double output = input;
        for (int i = 0; i < this->section.length(); i++)
        {
            output = this->section[i].compute(output);
        }
        return output;
    }
};

class BandpassFilterButterworthImplementation{
protected:
    LowpassFilterButterworthImplementation *lowpassFilter;
protected:
    HighpassFilterButterworthImplementation *highpassFilter;

public:
    BandpassFilterButterworthImplementation(double bottomFrequencyHz, double topFrequencyHz, int numSections, double Fs, int order)
    {
        this->lowpassFilter = new LowpassFilterButterworthImplementation
                             (topFrequencyHz, numSections, Fs, order);
        this->highpassFilter = new HighpassFilterButterworthImplementation
                              (bottomFrequencyHz, numSections, Fs, order);
    }

    double compute(double input)
    {
        // compute the result as the cascade of the highpass and lowpass filters
        return this->highpassFilter->compute(this->lowpassFilter->compute(input));
    }
};




#endif // BUTTERWORTHFILTER_H
