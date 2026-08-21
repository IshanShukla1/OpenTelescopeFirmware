# Changelog

All notable changes to OpenTelescopeFirmware are documented in this file.

The project follows a milestone-based development approach. Versions before `1.0.0` represent active development and may introduce architectural changes.

---

## [0.2.0] — 2026-08-21

### Added

#### Astronomy & Mathematics
- Strongly typed `Angle` abstraction using double-precision radians.
- Degree, radian, hour-angle, HMS, and DMS construction.
- Angle normalization and boundary handling.
- Right Ascension and Declination representations.
- Hour Angle calculation.

#### Astronomical Time
- UTC calendar date/time representation and validation.
- Julian Date (JD) calculations.
- Modified Julian Date (MJD) calculations.
- Julian Centuries (`T`) calculations.
- Unix timestamp conversion.

#### Sidereal Time
- Observer latitude/longitude representation and validation.
- Greenwich Mean Sidereal Time (GMST).
- Local Sidereal Time (LST).

#### Mount Model
- Motor-step to physical-axis-angle conversion.
- Physical-axis-angle to motor-step conversion.
- Configurable microstepping.
- Configurable gear ratios.
- Axis direction inversion.
- Step and angular soft-limit validation.

#### Coordinate System
- Equatorial coordinate representation.
- RA/DEC validation.
- Equatorial-to-mount coordinate transformation.
- Mount-to-equatorial coordinate transformation.

#### Testing
- Added unit tests for:
  - Angle mathematics
  - Astronomical time
  - Sidereal time
  - Mount-axis model
  - Celestial coordinates
  - Mount transformations
  - End-to-end astronomy pipeline
- Added hardware-independent end-to-end astronomy pipeline verification.

### Verification

- **10/10 native test cases passed.**
- ESP32 target firmware compiled successfully.
- v0.1 command, safety, configuration, and motion foundations remain intact.
- End-to-end astronomy pipeline verified to the project's configured step-resolution tolerance.

### Status

**Software verified.**

Physical telescope validation is pending availability of the electrical and mechanical prototype.

---

## [0.1.0] — 2026-08-21

### Added

#### Firmware Foundation
- ESP32 firmware foundation.
- Version identification.
- Initialization framework.
- Configuration system.
- Hardware abstraction foundation.

#### Motion Control
- RA axis abstraction.
- DEC axis abstraction.
- LV8729 STEP/DIR control architecture.
- Direction control.
- Basic non-blocking motion control.
- Velocity control.
- Acceleration/deceleration.
- Coordinated RA/DEC movement.
- Motion stop.
- Emergency stop.

#### Command Interface
- Serial command interface.
- Help/status functionality.
- RA and DEC motion commands.
- Synchronized axis movement.
- Emergency-stop command.
- Event and fault inspection.

#### Diagnostics & Safety
- Structured diagnostic foundation.
- Event logging.
- Fault handling.
- Safety state management.
- Reset-cause reporting.
- Runtime status reporting.

#### Testing
- Native unit-test infrastructure.
- Configuration validation tests.
- Command parser tests.
- Safety controller tests.

### Verification

- v0.1 software foundation verified through automated testing.
- ESP32 target build successfully established.
- Physical hardware validation remains dependent on completion of the electrical prototype.

### Status

**Software foundation complete.**

---

## [Unreleased]

### Planned — v0.3

**Tracking Engine & Interactive Telescope Simulator**

Planned capabilities include:

- Sidereal tracking engine.
- Tracking state machine.
- Virtual motor and virtual mount.
- Hardware-independent tracking simulation.
- Accelerated simulation time.
- Small graphical simulator/control window.
- Human-readable diagnostics.
- Tracking-error visualization.
- Automated long-duration tracking tests.
- Continued preservation of v0.1/v0.2 functionality.

---

## Future Roadmap

| Version | Planned Focus |
|---|---|
| **v0.3** | Tracking Engine & Interactive Simulator |
| **v0.4** | GoTo Engine |
| **v0.5** | Alignment System |
| **v0.6** | Pointing Model & Backlash Compensation |
| **v0.7** | External PC/Android Control |
| **v0.8** | Advanced Diagnostics & Mount Characterization |
| **v0.9** | Full-System Beta Integration |
| **v1.0** | First Complete Release |

Future features are subject to change based on simulation results, hardware testing, and development findings.

---

## Versioning

Versions before `1.0.0` represent active development milestones.

A version is considered complete only when its defined software tests and verification requirements have been satisfied.

Physical hardware validation is reported separately from software verification.