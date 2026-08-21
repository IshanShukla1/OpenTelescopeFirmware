#include "domain/astronomy/sidereal_time.h"
#include <cmath>

namespace domain {
namespace astronomy {

bool ObserverLocation::is_valid() const {
    double lat = latitude.as_degrees();
    double lon = longitude.as_degrees();
    if (lat < -90.0 || lat > 90.0) return false;
    if (lon < -180.0 || lon > 180.0) return false;
    return true;
}

Angle calculate_gmst(const AstroTime& time) {
    double jd = time.julian_date();
    double D = jd - 2451545.0;
    double T = time.julian_centuries_j2000();

    // Meeus Astronomical Algorithms (2nd Ed, Chapter 12, Eq 12.4)
    double theta_deg = 280.46061837 + (360.98564736629 * D) + (0.000387933 * T * T) - ((T * T * T) / 38710000.0);

    return Angle::FromDegrees(theta_deg).normalize_24h();
}

Angle calculate_lst(const AstroTime& time, const ObserverLocation& observer) {
    Angle gmst = calculate_gmst(time);
    Angle lst = (gmst + observer.longitude).normalize_24h();
    return lst;
}

} // namespace astronomy
} // namespace domain
