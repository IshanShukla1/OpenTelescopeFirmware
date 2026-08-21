#ifndef DOMAIN_ASTRONOMY_SIDEREAL_TIME_H
#define DOMAIN_ASTRONOMY_SIDEREAL_TIME_H

#include "domain/astronomy/angle.h"
#include "domain/astronomy/time_system.h"

namespace domain {
namespace astronomy {

// Observer geographic coordinates
struct ObserverLocation {
    Angle latitude;   // [-90 deg, +90 deg] (Positive = North, Negative = South)
    Angle longitude;  // [-180 deg, +180 deg] (Positive = East, Negative = West)

    bool is_valid() const;
};

// Calculate Greenwich Mean Sidereal Time (GMST) for a given AstroTime
Angle calculate_gmst(const AstroTime& time);

// Calculate Local Sidereal Time (LST) for a given AstroTime and ObserverLocation
Angle calculate_lst(const AstroTime& time, const ObserverLocation& observer);

} // namespace astronomy
} // namespace domain

#endif // DOMAIN_ASTRONOMY_SIDEREAL_TIME_H
