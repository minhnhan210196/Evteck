#ifndef HL_SIMPLEKALMANFILTER_H
#define HL_SIMPLEKALMANFILTER_H


class HL_SimpleKalmanFilter
{
private:
    double _err_measure;
    double _err_estimate;
    double _q;
    double _current_estimate;
    double _last_estimate;
    double _kalman_gain;
public:
    HL_SimpleKalmanFilter(double mea_e, double est_e, double q);
    double update(double mea);
    void setMeasurementError(double mea_e);
    void setEstimateError(double est_e);
    void setProcessNoise(double q);
    double getKalmanGain();
    double getEstimateError();
};


#endif // HL_SIMPLEKALMANFILTER_H
