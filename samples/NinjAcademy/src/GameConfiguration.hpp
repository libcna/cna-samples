// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "System/TimeSpan.hpp"

namespace NinjAcademy {

struct NinjAcademyContentReaderRegistrationEXT;

class GamePhase {
public:
    System::TimeSpan getDurationProperty() const { return duration_; }
    void setDurationProperty(System::TimeSpan value) { duration_ = value; }

    const std::vector<System::TimeSpan>& getTargetAppearanceIntervalsProperty() const {
        return targetAppearanceIntervals_;
    }
    void setTargetAppearanceIntervalsProperty(std::vector<System::TimeSpan> value) {
        targetAppearanceIntervals_ = std::move(value);
    }

    const std::vector<double>& getTargetAppearanceProbabilitiesProperty() const {
        return targetAppearanceProbabilities_;
    }
    void setTargetAppearanceProbabilitiesProperty(std::vector<double> value) {
        targetAppearanceProbabilities_ = std::move(value);
    }

    double getGoldTargetProbablityProperty() const { return goldTargetProbablity_; }
    void setGoldTargetProbablityProperty(double value) { goldTargetProbablity_ = value; }

    System::TimeSpan getBambooAppearanceIntervalProperty() const { return bambooAppearanceInterval_; }
    void setBambooAppearanceIntervalProperty(System::TimeSpan value) { bambooAppearanceInterval_ = value; }

    double getBambooAppearanceProbablityProperty() const { return bambooAppearanceProbablity_; }
    void setBambooAppearanceProbablityProperty(double value) { bambooAppearanceProbablity_ = value; }

    System::TimeSpan getDynamiteAppearanceIntervalProperty() const { return dynamiteAppearanceInterval_; }
    void setDynamiteAppearanceIntervalProperty(System::TimeSpan value) { dynamiteAppearanceInterval_ = value; }

    double getDynamiteAppearanceProbablityProperty() const { return dynamiteAppearanceProbablity_; }
    void setDynamiteAppearanceProbablityProperty(double value) { dynamiteAppearanceProbablity_ = value; }

    const std::vector<double>& getDynamiteAmountProbabilitiesProperty() const {
        return dynamiteAmountProbabilities_;
    }
    void setDynamiteAmountProbabilitiesProperty(std::vector<double> value) {
        dynamiteAmountProbabilities_ = std::move(value);
    }

private:
    friend struct NinjAcademyContentReaderRegistrationEXT;

    System::TimeSpan duration_ = System::TimeSpan::Zero;
    std::vector<System::TimeSpan> targetAppearanceIntervals_;
    std::vector<double> targetAppearanceProbabilities_;
    double goldTargetProbablity_ = 0.0;
    System::TimeSpan bambooAppearanceInterval_ = System::TimeSpan::Zero;
    double bambooAppearanceProbablity_ = 0.0;
    System::TimeSpan dynamiteAppearanceInterval_ = System::TimeSpan::Zero;
    double dynamiteAppearanceProbablity_ = 0.0;
    std::vector<double> dynamiteAmountProbabilities_;
};

class GameConfiguration {
public:
    int getPlayerLivesProperty() const { return playerLives_; }
    void setPlayerLivesProperty(int value) { playerLives_ = value; }

    int getPointsPerTargetProperty() const { return pointsPerTarget_; }
    void setPointsPerTargetProperty(int value) { pointsPerTarget_ = value; }

    int getPointsPerGoldTargetProperty() const { return pointsPerGoldTarget_; }
    void setPointsPerGoldTargetProperty(int value) { pointsPerGoldTarget_ = value; }

    int getPointsPerBambooProperty() const { return pointsPerBamboo_; }
    void setPointsPerBambooProperty(int value) { pointsPerBamboo_ = value; }

    const std::vector<std::shared_ptr<GamePhase>>& getPhasesProperty() const { return phases_; }
    void setPhasesProperty(std::vector<std::shared_ptr<GamePhase>> value) { phases_ = std::move(value); }

private:
    friend struct NinjAcademyContentReaderRegistrationEXT;

    int playerLives_ = 0;
    int pointsPerTarget_ = 0;
    int pointsPerGoldTarget_ = 0;
    int pointsPerBamboo_ = 0;
    std::vector<std::shared_ptr<GamePhase>> phases_;
};

} // namespace NinjAcademy
