#ifndef DOMAIN_ASTRONOMY_ANGLE_H
#define DOMAIN_ASTRONOMY_ANGLE_H

#include <cmath>
#include <cstdint>

namespace domain {
namespace astronomy {

constexpr double kPi = 3.14159265358979323846;
constexpr double kTwoPi = 6.28318530717958647692;
constexpr double kDegToRad = kPi / 180.0;
constexpr double kRadToDeg = 180.0 / kPi;
constexpr double kHoursToDeg = 15.0;
constexpr double kDegToHours = 1.0 / 15.0;
constexpr double kHoursToRad = kPi / 12.0;
constexpr double kRadToHours = 12.0 / kPi;

struct HMS {
    int hours;
    int minutes;
    double seconds;
    bool is_negative;
};

struct DMS {
    int degrees;
    int arcminutes;
    double arcseconds;
    bool is_negative;
};

class Angle {
public:
    constexpr Angle() : radians_(0.0) {}

    // Factory methods
    static constexpr Angle FromRadians(double rad) {
        return Angle(rad);
    }
    static constexpr Angle FromDegrees(double deg) {
        return Angle(deg * kDegToRad);
    }
    static constexpr Angle FromHours(double hrs) {
        return Angle(hrs * kHoursToRad);
    }
    static Angle FromHMS(int hours, int minutes, double seconds, bool is_negative = false);
    static Angle FromDMS(int degrees, int arcminutes, double arcseconds, bool is_negative = false);

    // Getters
    constexpr double as_radians() const { return radians_; }
    constexpr double as_degrees() const { return radians_ * kRadToDeg; }
    constexpr double as_hours() const { return radians_ * kRadToHours; }

    // Normalizations (returns new normalized Angle)
    Angle normalize_360() const;   // Wraps degrees into [0, 360)
    Angle normalize_180() const;   // Wraps degrees into [-180, +180)
    Angle normalize_2pi() const;   // Wraps radians into [0, 2pi)
    Angle normalize_24h() const;   // Wraps hours into [0, 24)

    // Decompositions
    HMS to_hms() const;
    DMS to_dms() const;

    // Operator Overloads
    constexpr Angle operator+(const Angle& other) const { return Angle(radians_ + other.radians_); }
    constexpr Angle operator-(const Angle& other) const { return Angle(radians_ - other.radians_); }
    constexpr Angle operator-() const { return Angle(-radians_); }
    Angle& operator+=(const Angle& other) { radians_ += other.radians_; return *this; }
    Angle& operator-=(const Angle& other) { radians_ -= other.radians_; return *this; }
    constexpr Angle operator*(double scalar) const { return Angle(radians_ * scalar); }
    constexpr Angle operator/(double scalar) const { return Angle(radians_ / scalar); }

    constexpr bool operator==(const Angle& other) const { return radians_ == other.radians_; }
    constexpr bool operator!=(const Angle& other) const { return radians_ != other.radians_; }
    constexpr bool operator<(const Angle& other) const { return radians_ < other.radians_; }
    constexpr bool operator<=(const Angle& other) const { return radians_ <= other.radians_; }
    constexpr bool operator>(const Angle& other) const { return radians_ > other.radians_; }
    constexpr bool operator>=(const Angle& other) const { return radians_ >= other.radians_; }

private:
    explicit constexpr Angle(double rad) : radians_(rad) {}
    double radians_;
};

constexpr Angle operator*(double scalar, const Angle& angle) {
    return angle * scalar;
}

} // namespace astronomy
} // namespace domain

#endif // DOMAIN_ASTRONOMY_ANGLE_H
