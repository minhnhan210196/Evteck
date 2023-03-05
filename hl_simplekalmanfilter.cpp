#include "hl_simplekalmanfilter.h"
#include "math.h"
HL_SimpleKalmanFilter::HL_SimpleKalmanFilter(double mea_e, double est_e, double q)
{
    _err_measure=mea_e;      // Measurement Uncertainty - How much do we expect to our measurement vary
    _err_estimate =est_e;   //  Estimation Uncertainty  - Can be initilized with the same value as e_mea since the kalman filter will adjust its value.
    _q = q;                 //  Process Variance ( 1/ sample frequency)
}

double HL_SimpleKalmanFilter::update(double mea)
{
    _kalman_gain = _err_estimate / (_err_estimate + _err_measure);
    _current_estimate = _last_estimate + _kalman_gain * (mea - _last_estimate);
    _err_estimate = (1.0 - _kalman_gain) * _err_estimate + abs(_last_estimate - _current_estimate) * _q;
    _last_estimate = _current_estimate;

    return _current_estimate;
}

void HL_SimpleKalmanFilter::setMeasurementError(double mea_e)
{
    _err_measure = mea_e;
}

void HL_SimpleKalmanFilter::setEstimateError(double est_e)
{
    _err_estimate = est_e;
}

void HL_SimpleKalmanFilter::setProcessNoise(double q)
{
    _q = q;
}

double HL_SimpleKalmanFilter::getKalmanGain()
{
    return _kalman_gain;
}

double HL_SimpleKalmanFilter::getEstimateError()
{
    return _err_estimate;
}
