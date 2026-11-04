#ifndef PLAN_TIMING_H
#define PLAN_TIMING_H

#include <QVector>
#include <QVector3D>

namespace PlanTiming {

enum Result {
    Success = 0,
    InvalidControlPointData,
    InvalidChannelTotalTime,
    InvalidFinalCumulativeTimeWeight,
    NonmonotonicCumulativeTimeWeight,
    NoActiveDwell,
    FinalCumulativeTimeWeightMismatch,
    UnsupportedTransitTime,
    InvalidControlPointOrientation
};

struct Channel {
    bool hasChannelTotalTime = false;
    double channelTotalTime = 0.0;
    bool hasFinalCumulativeTimeWeight = false;
    double finalCumulativeTimeWeight = 0.0;
    QVector<QVector3D> controlPointPositions;
    QVector<QVector3D> controlPointOrientations;
    QVector<double> cumulativeTimeWeights;
};

Result appendTemporaryChannel(
    const Channel &channel,
    QVector<QVector3D> *seedPositions,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
);

Result appendTemporaryChannel(
    const Channel &channel,
    QVector<QVector3D> *seedPositions,
    QVector<QVector3D> *seedOrientations,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
);

Result appendPermanentSeed(
    const QVector<QVector3D> &controlPointPositions,
    const QVector<double> &controlPointWeights,
    QVector<QVector3D> *seedPositions,
    QVector<double> *seedTimes
);

}

#endif