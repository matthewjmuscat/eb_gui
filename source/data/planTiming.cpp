#include "planTiming.h"

#include <algorithm>
#include <cmath>

namespace {

bool nearlyEqual(double left, double right) {
    double scale = std::max(1.0, std::max(std::abs(left), std::abs(right)));
    return std::abs(left-right) <= 1.0e-9*scale;
}

}

namespace PlanTiming {

Result appendTemporaryChannel(
    const Channel &channel,
    const QSet<int> &sourceNumbers,
    QVector<QVector3D> *seedPositions,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
) {
    if (!seedPositions || !seedTimes || !channelTreatmentTimeSeconds ||
        channel.controlPointPositions.size() < 2 ||
        channel.controlPointPositions.size() != channel.cumulativeTimeWeights.size()) {
        return InvalidControlPointData;
    }
    if (!channel.hasChannelTotalTime || !std::isfinite(channel.channelTotalTime) ||
        channel.channelTotalTime <= 0.0) {
        return InvalidChannelTotalTime;
    }
    if (!channel.hasFinalCumulativeTimeWeight ||
        !std::isfinite(channel.finalCumulativeTimeWeight) ||
        channel.finalCumulativeTimeWeight <= 0.0) {
        return InvalidFinalCumulativeTimeWeight;
    }
    if (sourceNumbers.size() != 1 || !channel.hasReferencedSourceNumber ||
        !sourceNumbers.contains(channel.referencedSourceNumber)) {
        return UnsupportedSourceReference;
    }

    const QVector<double> &weights = channel.cumulativeTimeWeights;
    if (!std::isfinite(weights.first()) || !nearlyEqual(weights.first(), 0.0)) {
        return InvalidControlPointData;
    }
    for (int i = 1; i < weights.size(); i++) {
        if (!std::isfinite(weights[i]) ||
            (weights[i] < weights[i-1] && !nearlyEqual(weights[i], weights[i-1]))) {
            return NonmonotonicCumulativeTimeWeight;
        }
    }
    if (!nearlyEqual(weights.last(), channel.finalCumulativeTimeWeight)) {
        return FinalCumulativeTimeWeightMismatch;
    }

    QVector<QVector3D> pendingPositions;
    QVector<double> pendingTimes;
    double pendingTreatmentTime = 0.0;
    for (int i = 1; i < channel.controlPointPositions.size(); i++) {
        double deltaWeight = weights[i]-weights[i-1];
        bool samePosition = channel.controlPointPositions[i] == channel.controlPointPositions[i-1];
        if (!samePosition) {
            if (!nearlyEqual(deltaWeight, 0.0)) {
                return UnsupportedTransitTime;
            }
            continue;
        }
        if (nearlyEqual(deltaWeight, 0.0)) {
            return ZeroDurationDwell;
        }

        double dwellSeconds = channel.channelTotalTime*deltaWeight/
                              channel.finalCumulativeTimeWeight;
        pendingPositions.append(channel.controlPointPositions[i]);
        pendingTimes.append(dwellSeconds);
        pendingTreatmentTime += dwellSeconds;
    }

    if (pendingTimes.isEmpty() || !nearlyEqual(pendingTreatmentTime, channel.channelTotalTime)) {
        return FinalCumulativeTimeWeightMismatch;
    }

    seedPositions->append(pendingPositions);
    seedTimes->append(pendingTimes);
    *channelTreatmentTimeSeconds = pendingTreatmentTime;
    return Success;
}

Result appendPermanentSeed(
    const QVector<QVector3D> &controlPointPositions,
    const QVector<double> &controlPointWeights,
    QVector<QVector3D> *seedPositions,
    QVector<double> *seedTimes
) {
    if (!seedPositions || !seedTimes || controlPointPositions.isEmpty() ||
        controlPointPositions.size() != controlPointWeights.size()) {
        return InvalidControlPointData;
    }

    double totalWeight = 0.0;
    QVector3D weightedPosition;
    for (int i = 0; i < controlPointPositions.size(); i++) {
        if (!std::isfinite(controlPointWeights[i]) || controlPointWeights[i] < 0.0) {
            return InvalidControlPointData;
        }
        weightedPosition += controlPointPositions[i]*controlPointWeights[i];
        totalWeight += controlPointWeights[i];
    }
    if (!std::isfinite(totalWeight) || totalWeight <= 0.0) {
        return InvalidControlPointData;
    }

    seedPositions->append(weightedPosition/totalWeight);
    seedTimes->append(totalWeight);
    return Success;
}

}