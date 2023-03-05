#ifndef NOTCHFILTER_H
#define NOTCHFILTER_H

#include "math.h"

class IIRFilter
{
public:
    IIRFilter(double samplingrate, double frequency, double Bandwidth) {
        double Pi = 3.141592f;
        double BW = Bandwidth / samplingrate;
        double f = frequency / samplingrate;
        double R = 1 - 3 * BW;

        double K = (1 - 2 * R * cos(2 * Pi * f) + R * R) / (2 - 2 * cos(2 * Pi * f));
        a0 = K;
        a1 = -2 * K * cos(2 * Pi * f);
        a2 = K;
        b1 = 2 * R * cos(2 * Pi * f);
        b2 = -R * R;

        x1 = x2 = y1 = y2 = 0;
    }

    IIRFilter(double samplingrate, double frequency)
            {
                const double pi = 3.14159265358979f;
                double wc = tan(frequency * pi / samplingrate);
                double k1 = 1.414213562f * wc;
                double k2 = wc * wc;
                a0 = k2 / (1 + k1 + k2);
                a1 = 2 * a0;
                a2 = a0;
                double k3 = a1 / k2;
                b1 = -2 * a0 + k3;
                b2 = 1 - (2 * a0) - k3;

                x1 = x2 = y1 = y2 = 0;
            }
    IIRFilter(double a0in, double a1in, double a2in, double b1in, double b2in)
            {
                a0 = a0in;
                a1 = a1in;
                a2 = a2in;
                b1 = b1in;
                b2 = b2in;

                x1 = x2 = y1 = y2 = 0;
            }

public:
    double a0;
    double a1;
    double a2;
    double b1;
    double b2;

    double x1;
    double x2;
    double y1;
    double y2;

};

class notchfilter
{
private:
    IIRFilter *Notch50;
    double filter(double x0, IIRFilter *iirfilter)
        {
            double y = iirfilter->a0 * x0 + iirfilter->a1 * iirfilter->x1 + iirfilter->a2 * iirfilter->x2 + iirfilter->b1 * iirfilter->y1 + iirfilter->b2 * iirfilter->y2;

            iirfilter->x2 = iirfilter->x1;
            iirfilter->x1 = x0;
            iirfilter->y2 = iirfilter->y1;
            iirfilter->y1 = y;

            return y;
        }
public:
    notchfilter(int samplerate, int cutoff_freq, int bandwidth)
        {
            Notch50 = new IIRFilter(samplerate, cutoff_freq, bandwidth); // sample rate 6900Hz, Notch at 50Hz with bandwidth of 5
        }
    double step(double x)
        {
            return  filter(x,Notch50);
        }
};

#endif // NOTCHFILTER_H
