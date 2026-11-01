#include "../source/data/planTiming.h"

#include <QCoreApplication>

#include <cmath>
#include <iostream>

namespace {

bool closeTo(double left, double right) {
    return std::abs(left-right) < 1.0e-9;
}

PlanTiming::Channel channel(
    double totalTime,
    double finalWeight,
    int sourceNumber,
    const QVector<QVector3D> &positions,
    const QVector<double> &weights
) {
    PlanTiming::Channel value;
    value.hasChannelTotalTime = true;
    value.channelTotalTime = totalTime;
    value.hasFinalCumulativeTimeWeight = true;
    value.finalCumulativeTimeWeight = finalWeight;
    value.hasReferencedSourceNumber = true;
    value.referencedSourceNumber = sourceNumber;
    value.controlPointPositions = positions;
    value.cumulativeTimeWeights = weights;
    return value;
}

bool testSingleChannel() {
    QVector<QVector3D> seeds;
    QVector<double> times;
    double treatmentTime = 0.0;
    PlanTiming::Channel value = channel(
        60.0,
        100.0,
        1,
        {QVector3D(0, 0, 0), QVector3D(0, 0, 0),
         QVector3D(1, 0, 0), QVector3D(1, 0, 0)},
        {0.0, 20.0, 20.0, 100.0}
    );
    PlanTiming::Result result = PlanTiming::appendTemporaryChannel(
        value, QSet<int>{1}, &seeds, &times, &treatmentTime
    );
    return result == PlanTiming::Success && seeds.size() == 2 &&
           times.size() == 2 && closeTo(times[0], 12.0) &&
           closeTo(times[1], 48.0) && closeTo(treatmentTime, 60.0);
}

bool testIndependentChannelNormalization() {
    QVector<QVector3D> seeds;
    QVector<double> times;
    double firstTime = 0.0;
    double secondTime = 0.0;
    PlanTiming::Channel first = channel(
        100.0,
        100.0,
        1,
        {QVector3D(0, 0, 0), QVector3D(0, 0, 0),
         QVector3D(1, 0, 0), QVector3D(1, 0, 0)},
        {0.0, 25.0, 25.0, 100.0}
    );
    PlanTiming::Channel second = channel(
        40.0,
        20.0,
        1,
        {QVector3D(2, 0, 0), QVector3D(2, 0, 0),
         QVector3D(3, 0, 0), QVector3D(3, 0, 0)},
        {0.0, 5.0, 5.0, 20.0}
    );
    PlanTiming::Result firstResult = PlanTiming::appendTemporaryChannel(
        first, QSet<int>{1}, &seeds, &times, &firstTime
    );
    PlanTiming::Result secondResult = PlanTiming::appendTemporaryChannel(
        second, QSet<int>{1}, &seeds, &times, &secondTime
    );
    return firstResult == PlanTiming::Success && secondResult == PlanTiming::Success &&
           times.size() == 4 && closeTo(times[0], 25.0) &&
           closeTo(times[1], 75.0) && closeTo(times[2], 10.0) &&
           closeTo(times[3], 30.0) && closeTo(firstTime, 100.0) &&
           closeTo(secondTime, 40.0);
}

bool testInvalidDenominators() {
    QVector<QVector3D> positions{QVector3D(), QVector3D()};
    QVector<double> weights{0.0, 1.0};
    QVector<QVector3D> seeds;
    QVector<double> times;
    double treatmentTime = 0.0;

    PlanTiming::Channel missing = channel(1.0, 1.0, 1, positions, weights);
    missing.hasFinalCumulativeTimeWeight = false;
    PlanTiming::Channel zero = channel(1.0, 0.0, 1, positions, weights);
    PlanTiming::Channel invalidTime = channel(0.0, 1.0, 1, positions, weights);
    return PlanTiming::appendTemporaryChannel(
               missing, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::InvalidFinalCumulativeTimeWeight &&
           PlanTiming::appendTemporaryChannel(
               zero, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::InvalidFinalCumulativeTimeWeight &&
           PlanTiming::appendTemporaryChannel(
               invalidTime, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::InvalidChannelTotalTime;
}

bool testInvalidControlPointTiming() {
    QVector<QVector3D> seeds;
    QVector<double> times;
    double treatmentTime = 0.0;
    QVector<QVector3D> positions{
        QVector3D(0, 0, 0), QVector3D(0, 0, 0),
        QVector3D(1, 0, 0), QVector3D(1, 0, 0)
    };
    PlanTiming::Channel nonmonotonic = channel(
        10.0, 10.0, 1, positions, {0.0, 6.0, 5.0, 10.0}
    );
    PlanTiming::Channel zeroDwell = channel(
        10.0, 10.0, 1, positions, {0.0, 0.0, 0.0, 10.0}
    );
    PlanTiming::Channel transitTime = channel(
        10.0, 10.0, 1, positions, {0.0, 4.0, 5.0, 10.0}
    );
    PlanTiming::Channel finalMismatch = channel(
        10.0, 12.0, 1, positions, {0.0, 4.0, 4.0, 10.0}
    );
    PlanTiming::Channel countMismatch = channel(
        10.0, 10.0, 1, positions, {0.0, 4.0, 4.0}
    );
    return PlanTiming::appendTemporaryChannel(
               nonmonotonic, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::NonmonotonicCumulativeTimeWeight &&
           PlanTiming::appendTemporaryChannel(
               zeroDwell, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::ZeroDurationDwell &&
           PlanTiming::appendTemporaryChannel(
               transitTime, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::UnsupportedTransitTime &&
           PlanTiming::appendTemporaryChannel(
               finalMismatch, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::FinalCumulativeTimeWeightMismatch &&
           PlanTiming::appendTemporaryChannel(
               countMismatch, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::InvalidControlPointData;
}

bool testUnsupportedSourceReferences() {
    QVector<QVector3D> positions{QVector3D(), QVector3D()};
    QVector<double> weights{0.0, 1.0};
    QVector<QVector3D> seeds;
    QVector<double> times;
    double treatmentTime = 0.0;
    PlanTiming::Channel value = channel(1.0, 1.0, 2, positions, weights);
    PlanTiming::Channel missing = value;
    missing.hasReferencedSourceNumber = false;
    return PlanTiming::appendTemporaryChannel(
               value, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::UnsupportedSourceReference &&
           PlanTiming::appendTemporaryChannel(
               value, QSet<int>{1, 2}, &seeds, &times, &treatmentTime
           ) == PlanTiming::UnsupportedSourceReference &&
           PlanTiming::appendTemporaryChannel(
               missing, QSet<int>{1}, &seeds, &times, &treatmentTime
           ) == PlanTiming::UnsupportedSourceReference;
}

bool testErrorDoesNotAppendOutput() {
    QVector<QVector3D> seeds{QVector3D(9, 9, 9)};
    QVector<double> times{9.0};
    double treatmentTime = 9.0;
    PlanTiming::Channel value = channel(
        10.0,
        10.0,
        1,
        {QVector3D(), QVector3D(), QVector3D(1, 0, 0), QVector3D(1, 0, 0)},
        {0.0, 0.0, 0.0, 10.0}
    );
    PlanTiming::Result result = PlanTiming::appendTemporaryChannel(
        value, QSet<int>{1}, &seeds, &times, &treatmentTime
    );
    return result == PlanTiming::ZeroDurationDwell && seeds.size() == 1 &&
           times.size() == 1 && seeds[0] == QVector3D(9, 9, 9) &&
           closeTo(times[0], 9.0) && closeTo(treatmentTime, 9.0);
}

bool testPermanentSeedRegression() {
    QVector<QVector3D> seeds;
    QVector<double> times;
    PlanTiming::Result result = PlanTiming::appendPermanentSeed(
        {QVector3D(0, 0, 0), QVector3D(10, 0, 0)},
        {1.0, 3.0},
        &seeds,
        &times
    );
    return result == PlanTiming::Success && seeds.size() == 1 &&
           times.size() == 1 && closeTo(seeds[0].x(), 7.5) &&
           closeTo(times[0], 4.0);
}

}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    const struct {
        const char *name;
        bool (*run)();
    } tests[] = {
        {"single-channel HDR", testSingleChannel},
        {"independent multi-channel HDR", testIndependentChannelNormalization},
        {"invalid denominators", testInvalidDenominators},
        {"invalid control-point timing", testInvalidControlPointTiming},
        {"unsupported source references", testUnsupportedSourceReferences},
        {"atomic error output", testErrorDoesNotAppendOutput},
        {"permanent LDR regression", testPermanentSeedRegression},
    };

    for (const auto &test : tests) {
        if (!test.run()) {
            std::cerr << "FAILED: " << test.name << "\n";
            return 1;
        }
    }
    std::cout << "Passed " << sizeof(tests)/sizeof(tests[0])
              << " HDR channel timing tests\n";
    return 0;
}