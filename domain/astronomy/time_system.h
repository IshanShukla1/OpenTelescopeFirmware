#ifndef DOMAIN_ASTRONOMY_TIME_SYSTEM_H
#define DOMAIN_ASTRONOMY_TIME_SYSTEM_H

#include <cstdint>

namespace domain {
namespace astronomy {

// UTC Calendar date and time structure
struct UtcDateTime {
    int year;           // e.g. 2026
    uint8_t month;      // 1..12
    uint8_t day;        // 1..31
    uint8_t hour;       // 0..23
    uint8_t minute;     // 0..59
    double second;      // 0.0 .. <60.0

    bool is_valid() const;
};

class AstroTime {
public:
    constexpr AstroTime() : julian_date_(2451545.0) {} // Default J2000.0 epoch

    // Factory methods
    static AstroTime FromUtc(const UtcDateTime& utc);
    static AstroTime FromJulianDate(double jd);
    static AstroTime FromUnixTimestamp(int64_t seconds, double fractional_seconds = 0.0);

    // Getters
    constexpr double julian_date() const { return julian_date_; }
    constexpr double modified_julian_date() const { return julian_date_ - 2400000.5; }
    constexpr double julian_centuries_j2000() const { return (julian_date_ - 2451545.0) / 36525.0; }

    UtcDateTime to_utc() const;
    int64_t unix_timestamp() const;

    // Comparisons
    constexpr bool operator==(const AstroTime& other) const { return julian_date_ == other.julian_date_; }
    constexpr bool operator!=(const AstroTime& other) const { return julian_date_ != other.julian_date_; }
    constexpr bool operator<(const AstroTime& other) const { return julian_date_ < other.julian_date_; }
    constexpr bool operator<=(const AstroTime& other) const { return julian_date_ <= other.julian_date_; }
    constexpr bool operator>(const AstroTime& other) const { return julian_date_ > other.julian_date_; }
    constexpr bool operator>=(const AstroTime& other) const { return julian_date_ >= other.julian_date_; }

private:
    explicit constexpr AstroTime(double jd) : julian_date_(jd) {}
    double julian_date_;
};

} // namespace astronomy
} // namespace domain

#endif // DOMAIN_ASTRONOMY_TIME_SYSTEM_H
