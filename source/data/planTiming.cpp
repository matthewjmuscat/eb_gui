#include "planTiming.h"

#include <algorithm>
#include <cmath>

namespace {

bool nearlyEqual(double left, double right) {
    double scale = std::max(1.0, std::max(std::abs(left), std::abs(right)));
    return std::abs(left-right) <= 1.0e-9*scale;
}

bool sameDwellPosition(const QVector3D &left, const QVector3D &right) {
    const float positionToleranceCm = 1.0e-5f;
    return (left-right).lengthSquared() <= positionToleranceCm*positionToleranceCm;
}

bool normalizedOrientation(const QVector3D &input, QVector3D *output) {
    if (!std::isfinite(input.x()) || !std::isfinite(input.y()) ||
        !std::isfinite(input.z()) || input.lengthSquared() <= 1.0e-12f) {
        return false;
    }
    *output = input.normalized();
    return true;
}

bool sameOrientation(const QVector3D &left, const QVector3D &right) {
    const float orientationTolerance = 1.0e-5f;
    return (left-right).lengthSquared() <= orientationTolerance*orientationTolerance;
}

PlanTiming::Result appendTemporaryChannelImpl(
    const PlanTiming::Channel &channel,
    QVector<QVector3D> *seedPositions,
    QVector<QVector3D> *seedOrientations,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
);

}

namespace PlanTiming {

Result appendTemporaryChannel(
    const Channel &channel,
    QVector<QVector3D> *seedPositions,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
) {
    return appendTemporaryChannelImpl(
        channel, seedPositions, nullptr, seedTimes, channelTreatmentTimeSeconds
    );
}

Result appendTemporaryChannel(
    const Channel &channel,
    QVector<QVector3D> *seedPositions,
    QVector<QVector3D> *seedOrientations,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
) {
    return appendTemporaryChannelImpl(
        channel, seedPositions, seedOrientations, seedTimes, channelTreatmentTimeSeconds
    );
}

}

namespace {

PlanTiming::Result appendTemporaryChannelImpl(
    const PlanTiming::Channel &channel,
    QVector<QVector3D> *seedPositions,
    QVector<QVector3D> *seedOrientations,
    QVector<double> *seedTimes,
    double *channelTreatmentTimeSeconds
) {
    using namespace PlanTiming;
    if (!seedPositions || !seedTimes || !channelTreatmentTimeSeconds ||
        channel.controlPointPositions.size() < 2 ||
        channel.controlPointPositions.size() != channel.cumulativeTimeWeights.size()) {
        return InvalidControlPointData;
    }
    QVector<QVector3D> normalizedOrientations;
    if (seedOrientations) {
        if (channel.controlPointOrientations.size() != channel.controlPointPositions.size())
            return InvalidControlPointOrientation;
        normalizedOrientations.resize(channel.controlPointOrientations.size());
        for (int i = 0; i < channel.controlPointOrientations.size(); i++)
            if (!normalizedOrientation(
                    channel.controlPointOrientations[i], &normalizedOrientations[i]
                ))
                return InvalidControlPointOrientation;
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
    QVector<QVector3D> pendingOrientations;
    QVector<double> pendingTimes;
    double pendingTreatmentTime = 0.0;
    for (int i = 1; i < channel.controlPointPositions.size(); i++) {
        double deltaWeight = weights[i]-weights[i-1];
        bool samePosition = sameDwellPosition(
            channel.controlPointPositions[i], channel.controlPointPositions[i-1]
        );
        if (!samePosition) {
            if (!nearlyEqual(deltaWeight, 0.0)) {
                return UnsupportedTransitTime;
            }
            continue;
        }
        if (nearlyEqual(deltaWeight, 0.0)) {
            continue;
        }
        if (seedOrientations && !sameOrientation(
                normalizedOrientations[i], normalizedOrientations[i-1]
            )) {
            return InvalidControlPointOrientation;
        }

        double dwellSeconds = channel.channelTotalTime*deltaWeight/
                              channel.finalCumulativeTimeWeight;
        pendingPositions.append(channel.controlPointPositions[i]);
        if (seedOrientations)
            pendingOrientations.append(normalizedOrientations[i]);
        pendingTimes.append(dwellSeconds);
        pendingTreatmentTime += dwellSeconds;
    }

    if (pendingTimes.isEmpty()) {
        return NoActiveDwell;
    }
    if (!nearlyEqual(pendingTreatmentTime, channel.channelTotalTime)) {
        return FinalCumulativeTimeWeightMismatch;
    }

    seedPositions->append(pendingPositions);
    if (seedOrientations)
        seedOrientations->append(pendingOrientations);
    seedTimes->append(pendingTimes);
    *channelTreatmentTimeSeconds = pendingTreatmentTime;
    return Success;
}

}

namespace PlanTiming {

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