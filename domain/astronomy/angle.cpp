#include "domain/astronomy/angle.h"
#include <cmath>

namespace domain {
namespace astronomy {

Angle Angle::FromHMS(int hours, int minutes, double seconds, bool is_negative) {
    double total_hrs = static_cast<double>(hours) + (static_cast<double>(minutes) / 60.0) + (seconds / 3600.0);
    if (is_negative) {
        total_hrs = -total_hrs;
    }
    return FromHours(total_hrs);
}

Angle Angle::FromDMS(int degrees, int arcminutes, double arcseconds, bool is_negative) {
    double total_deg = static_cast<double>(degrees) + (static_cast<double>(arcminutes) / 60.0) + (arcseconds / 3600.0);
    if (is_negative) {
        total_deg = -total_deg;
    }
    return FromDegrees(total_deg);
}

Angle Angle::normalize_360() const {
    double deg = std::fmod(as_degrees(), 360.0);
    if (deg < 0.0) {
        deg += 360.0;
    }
    if (deg >= 360.0) {
        deg = 0.0;
    }
    return FromDegrees(deg);
}

Angle Angle::normalize_180() const {
    double deg = std::fmod(as_degrees(), 360.0);
    if (deg < 0.0) {
        deg += 360.0;
    }
    if (deg >= 180.0) {
        deg -= 360.0;
    }
    return FromDegrees(deg);
}

Angle Angle::normalize_2pi() const {
    double rad = std::fmod(as_radians(), kTwoPi);
    if (rad < 0.0) {
        rad += kTwoPi;
    }
    if (rad >= kTwoPi) {
        rad = 0.0;
    }
    return FromRadians(rad);
}

Angle Angle::normalize_24h() const {
    double hrs = std::fmod(as_hours(), 24.0);
    if (hrs < 0.0) {
        hrs += 24.0;
    }
    if (hrs >= 24.0) {
        hrs = 0.0;
    }
    return FromHours(hrs);
}

HMS Angle::to_hms() const {
    HMS result{};
    double total_hrs = as_hours();
    if (total_hrs < 0.0) {
        result.is_negative = true;
        total_hrs = -total_hrs;
    } else {
        result.is_negative = false;
    }

    result.hours = static_cast<int>(std::floor(total_hrs));
    double rem_m = (total_hrs - result.hours) * 60.0;
    result.minutes = static_cast<int>(std::floor(rem_m));
    result.seconds = (rem_m - result.minutes) * 60.0;

    if (result.seconds >= 60.0) {
        result.seconds -= 60.0;
        result.minutes += 1;
    }
    if (result.minutes >= 60) {
        result.minutes -= 60;
        result.hours += 1;
    }

    return result;
}

DMS Angle::to_dms() const {
    DMS result{};
    double total_deg = as_degrees();
    if (total_deg < 0.0) {
        result.is_negative = true;
        total_deg = -total_deg;
    } else {
        result.is_negative = false;
    }

    result.degrees = static_cast<int>(std::floor(total_deg));
    double rem_m = (total_deg - result.degrees) * 60.0;
    result.arcminutes = static_cast<int>(std::floor(rem_m));
    result.arcseconds = (rem_m - result.arcminutes) * 60.0;

    if (result.arcseconds >= 60.0) {
        result.arcseconds -= 60.0;
        result.arcminutes += 1;
    }
    if (result.arcminutes >= 60) {
        result.arcminutes -= 60;
        result.degrees += 1;
    }

    return result;
}

} // namespace astronomy
} // namespace domain
