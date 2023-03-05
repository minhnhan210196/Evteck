#include "butterworthfilter.h"

LowpassFilterButterworthImplementation::LowpassFilterButterworthImplementation(double cutoffFrequencyHz, int numSections, double Fs, int order)
{
    for (int i = 0; i < numSections; i++)
    {
        LowpassFilterButterworthSection new_section(cutoffFrequencyHz, i + 1, numSections * 2, Fs, order);
        this->section.append(new_section);
    }
}

double LowpassFilterButterworthImplementation::compute(double input)
{
    double output = input;
    for (int i = 0; i < this->section.length(); i++)
    {
        output = this->section[i].compute(output);
    }
    return output;
}
