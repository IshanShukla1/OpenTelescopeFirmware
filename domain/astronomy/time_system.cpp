#include "domain/astronomy/time_system.h"
#include <cmath>

namespace domain {
namespace astronomy {

namespace {

bool is_leap_year(int year) {
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    return (year % 400 == 0);
}

uint8_t days_in_month(int year, uint8_t month) {
    static const uint8_t kDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) return 0;
    if (month == 2 && is_leap_year(year)) return 29;
    return kDays[month - 1];
}

} // namespace

bool UtcDateTime::is_valid() const {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > days_in_month(year, month)) return false;
    if (hour > 23) return false;
    if (minute > 59) return false;
    if (second < 0.0 || second >= 60.0) return false;
    return true;
}

AstroTime AstroTime::FromUtc(const UtcDateTime& utc) {
    int Y = utc.year;
    int M = utc.month;
    if (M <= 2) {
        Y -= 1;
        M += 12;
    }

    int A = Y / 100;
    int B = 2 - A + (A / 4);

    double day_fraction = static_cast<double>(utc.day) +
                          (static_cast<double>(utc.hour) / 24.0) +
                          (static_cast<double>(utc.minute) / 1440.0) +
                          (utc.second / 86400.0);

    double jd = static_cast<double>(static_cast<int64_t>(365.25 * (Y + 4716))) +
                static_cast<double>(static_cast<int64_t>(30.6001 * (M + 1))) +
                day_fraction + B - 1524.5;

    return AstroTime(jd);
}

AstroTime AstroTime::FromJulianDate(double jd) {
    return AstroTime(jd);
}

AstroTime AstroTime::FromUnixTimestamp(int64_t seconds, double fractional_seconds) {
    double jd = 2440587.5 + (static_cast<double>(seconds) + fractional_seconds) / 86400.0;
    return AstroTime(jd);
}

UtcDateTime AstroTime::to_utc() const {
    double jd_offset = julian_date_ + 0.5;
    int64_t Z = static_cast<int64_t>(std::floor(jd_offset));
    double F = jd_offset - static_cast<double>(Z);

    int64_t A = Z;
    if (Z >= 2299161) {
        int64_t alpha = static_cast<int64_t>(std::floor((Z - 1867216.25) / 36524.25));
        A = Z + 1 + alpha - (alpha / 4);
    }

    int64_t B = A + 1524;
    int64_t C = static_cast<int64_t>(std::floor((B - 122.1) / 365.25));
    int64_t D = static_cast<int64_t>(std::floor(365.25 * C));
    int64_t E = static_cast<int64_t>(std::floor((B - D) / 30.6001));

    double day_with_fraction = static_cast<double>(B - D - static_cast<int64_t>(std::floor(30.6001 * E))) + F;
    int day = static_cast<int>(std::floor(day_with_fraction));
    double rem_day = day_with_fraction - static_cast<double>(day);

    double total_seconds = std::round(rem_day * 86400.0 * 1e6) / 1e6;
    int hour = static_cast<int>(total_seconds / 3600.0);
    total_seconds -= static_cast<double>(hour) * 3600.0;

    int minute = static_cast<int>(total_seconds / 60.0);
    double second = total_seconds - static_cast<double>(minute) * 60.0;

    if (second >= 59.999) {
        second = 0.0;
        minute += 1;
    }
    if (minute >= 60) {
        minute -= 60;
        hour += 1;
    }
    if (hour >= 24) {
        hour -= 24;
        day += 1;
    }

    int month = (E < 14) ? static_cast<int>(E - 1) : static_cast<int>(E - 13);
    int year = (month > 2) ? static_cast<int>(C - 4716) : static_cast<int>(C - 4715);

    UtcDateTime utc{};
    utc.year = year;
    utc.month = static_cast<uint8_t>(month);
    utc.day = static_cast<uint8_t>(day);
    utc.hour = static_cast<uint8_t>(hour);
    utc.minute = static_cast<uint8_t>(minute);
    utc.second = second;

    return utc;
}

int64_t AstroTime::unix_timestamp() const {
    double seconds = (julian_date_ - 2440587.5) * 86400.0;
    return static_cast<int64_t>(std::round(seconds));
}

} // namespace astronomy
} // namespace domain
