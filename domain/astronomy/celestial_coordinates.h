#ifndef DOMAIN_ASTRONOMY_CELESTIAL_COORDINATES_H
#define DOMAIN_ASTRONOMY_CELESTIAL_COORDINATES_H

#include "domain/astronomy/angle.h"

namespace domain {
namespace astronomy {

class RightAscension {
public:
    constexpr RightAscension() : angle_(Angle::FromHours(0.0)) {}
    explicit RightAscension(const Angle& angle) : angle_(angle.normalize_24h()) {}

    constexpr Angle angle() const { return angle_; }
    constexpr double as_hours() const { return angle_.as_hours(); }
    constexpr double as_degrees() const { return angle_.as_degrees(); }
    constexpr double as_radians() const { return angle_.as_radians(); }
    HMS to_hms() const { return angle_.to_hms(); }

    constexpr bool operator==(const RightAscension& other) const { return angle_ == other.angle_; }
    constexpr bool operator!=(const RightAscension& other) const { return angle_ != other.angle_; }

private:
    Angle angle_;
};

class Declination {
public:
    constexpr Declination() : angle_(Angle::FromDegrees(0.0)) {}
    explicit Declination(const Angle& angle) : angle_(angle) {}

    constexpr Angle angle() const { return angle_; }
    constexpr double as_degrees() const { return angle_.as_degrees(); }
    constexpr double as_radians() const { return angle_.as_radians(); }
    DMS to_dms() const { return angle_.to_dms(); }

    bool is_valid() const {
        double deg = angle_.as_degrees();
        return (deg >= -90.0 && deg <= 90.0);
    }

    constexpr bool operator==(const Declination& other) const { return angle_ == other.angle_; }
    constexpr bool operator!=(const Declination& other) const { return angle_ != other.angle_; }

private:
    Angle angle_;
};

struct EquatorialCoordinates {
    RightAscension ra;
    Declination dec;

    bool is_valid() const {
        return dec.is_valid();
    }
};

// Calculate Hour Angle HA = (LST - RA), normalized to [-12h, +12h)
Angle calculate_hour_angle(const Angle& lst, const RightAscension& ra);

} // namespace astronomy
} // namespace domain

#endif // DOMAIN_ASTRONOMY_CELESTIAL_COORDINATES_H
