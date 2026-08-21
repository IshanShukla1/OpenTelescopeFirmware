# OpenTelescopeFirmware

**Open-source ESP32 firmware for a DIY computerized GoTo telescope mount.**

OpenTelescopeFirmware is an independently developed, modular telescope-control firmware designed to retrofit an equatorial telescope mount with computer-assisted motion, astronomical coordinate processing, diagnostics, and eventually autonomous GoTo and tracking capabilities.

The project is being developed from the ground up with a strong emphasis on **open-source engineering, mathematical correctness, hardware abstraction, observability, and testability**.

> **Current version: v0.2.0 — Astronomy Engine & Mount Coordinate Foundation**

---

## Project Status

OpenTelescopeFirmware is under active development.

The project is currently progressing from a software-verified astronomy and mount-coordinate foundation toward a hardware-independent tracking and simulation system.

### Current milestone

**v0.2.0 — Astronomy Engine & Mount Coordinate Foundation**

Completed and verified:

- Strongly typed angular mathematics
- Right Ascension and Declination
- UTC and astronomical time handling
- Julian Date / Modified Julian Date
- Greenwich Mean Sidereal Time
- Local Sidereal Time
- Observer location model
- Motor-step ↔ mount-axis angle conversion
- Equatorial coordinate representation
- Hour Angle calculation
- Equatorial mount coordinate transformations
- Hardware-independent end-to-end astronomy pipeline
- Diagnostic and testing foundations inherited from v0.1

Verification:

- **10/10 native test cases passing**
- **ESP32 target build successful**
- v0.1 functionality preserved

---

## Vision

The long-term goal is to create a complete, open-source telescope-control platform capable of:

```text
Astronomical Coordinates
          ↓
   Mount Coordinate Model
          ↓
       GoTo Solver
          ↓
    Motion Controller
          ↓
      ESP32 + Drivers
          ↓
      Telescope Mount
```

The firmware is intended to eventually support:

- Precision two-axis motion control
- Sidereal tracking
- GoTo
- Star alignment
- Pointing-error correction
- Backlash compensation
- Computer control
- Mobile control
- Comprehensive diagnostics
- Mount characterization
- Future adaptive and intelligent control research

These capabilities are developed incrementally and are **not all available in the current release**.

---

## Hardware Target

The initial hardware target is a DIY retrofit equatorial telescope mount using:

| Component | Current Target |
|---|---|
| Controller | ESP32 |
| RA Driver | LV8729 |
| DEC Driver | LV8729 |
| RA Motor | NEMA17 stepper |
| DEC Motor | NEMA17 stepper |
| Motor Interface | STEP/DIR |
| Motor Supply | ~12 V |
| Connectivity | USB/Serial initially |
| Wireless | ESP32 Wi-Fi for later releases |

Mechanical parameters such as gear ratios, microstepping, axis direction, and steps per revolution are configurable and depend on the specific mount implementation.

The firmware architecture is intentionally designed so additional hardware such as sensors, encoders, limit switches, or other diagnostic hardware can be incorporated later.

---

## Development Philosophy

OpenTelescopeFirmware is **not intended to be a clone or rebranding of an existing telescope firmware project**.

Established open-source telescope-control projects may be studied as engineering references and used as comparison baselines, but the firmware architecture and implementation are developed independently.

The project prioritizes:

- **Correctness over feature count**
- **Deterministic motion control**
- **Mathematically verified astronomy**
- **Hardware abstraction**
- **Modular architecture**
- **Observability**
- **Safety**
- **Automated testing**
- **Reproducible validation**
- **Incremental development**

---

## Architecture

The firmware uses a layered architecture designed to separate telescope-domain logic from physical hardware.

```text
┌─────────────────────────────────────────┐
│              Application                │
├─────────────────────────────────────────┤
│               Services                  │
├─────────────────────────────────────────┤
│          Domain / Telescope             │
│  Astronomy • Mount • Motion • Safety   │
├─────────────────────────────────────────┤
│                 Ports                   │
│       Hardware / Service Interfaces     │
├─────────────────────────────────────────┤
│               Adapters                  │
│   Commands • Diagnostics • Configuration│
├─────────────────────────────────────────┤
│              Platform                  │
│          ESP32 • LV8729 • GPIO          │
└─────────────────────────────────────────┘
```

A central architectural principle is that the astronomy engine must not directly control physical hardware.

This allows the same mathematical and control logic to eventually be exercised through:

- real ESP32 hardware
- a virtual telescope
- automated tests
- simulation tools

---

## Development Roadmap

The project is being developed incrementally.

| Version | Objective | Status |
|---|---|---|
| **v0.1** | Motion-control & firmware foundation | ✅ Complete |
| **v0.2** | Astronomy engine & mount coordinates | ✅ Complete |
| **v0.3** | Tracking engine & interactive simulator | Planned |
| **v0.4** | GoTo engine | Planned |
| **v0.5** | Alignment system | Planned |
| **v0.6** | Pointing model & backlash compensation | Planned |
| **v0.7** | External/PC/mobile control | Planned |
| **v0.8** | Advanced diagnostics & mount characterization | Planned |
| **v0.9** | Full-system beta integration | Planned |
| **v1.0** | First complete release | Planned |

The roadmap is intentionally iterative. Results from simulation and eventual physical testing may change the priorities of later releases.

---

## Testing Strategy

Testing is performed at multiple levels.

### Software testing

Astronomy and domain logic is tested independently of physical hardware.

The project currently uses automated native tests for:

- angle mathematics
- astronomical time
- sidereal time
- mount-axis calculations
- celestial coordinates
- coordinate transformations
- configuration
- command parsing
- safety
- end-to-end astronomy calculations

### Firmware testing

The ESP32 target is compiled independently of native tests to ensure the embedded target remains buildable.

### Hardware validation

Once the electrical and mechanical prototype is available, physical validation will be performed using the same hardware under controlled test conditions.

Where appropriate, OpenTelescopeFirmware will be compared against established telescope-control software such as OnStepX as a **benchmark/reference system**, not as an implementation dependency.

---

## Simulation

A major objective beginning with v0.3 is to reduce dependence on physical hardware during development.

The planned simulator will allow the firmware's astronomical and tracking behaviour to be exercised through a simple graphical interface without requiring:

- an ESP32
- motor drivers
- stepper motors
- a telescope mount
- programming knowledge

The simulator will eventually support controlled and accelerated time so that long-duration tracking behaviour can be evaluated rapidly.

---

## User Control

The initial completed firmware is primarily a development and control platform.

As the firmware matures, temporary user-facing control will be provided through:

- **Stellarium on PC**
- A lightweight Android controller

These are intended as practical interfaces while the firmware is being developed.

A more complete dedicated telescope-control application is planned **after the firmware reaches a stable v1.0 release**.

---

## Repository Structure

```text
OpenTelescopeFirmware/
│
├── core/
├── domain/
│   ├── astronomy/
│   └── mount/
│
├── ports/
├── adapters/
├── platform/
│
├── tests/
│   └── unit/
│
├── docs/
│
├── simulator/          # introduced during v0.3 development
│
├── platformio.ini
├── README.md
├── LICENSE
└── CHANGELOG.md
```

The exact structure may evolve as the project develops, but architectural boundaries are intentionally maintained.

---

## Releases

Releases are versioned using semantic versioning where appropriate.

Development versions:

```text
v0.x.x
```

represent an evolving system where architecture and interfaces may still change.

The first stable complete firmware release will be:

```text
v1.0.0
```

A release package is intended to contain the compiled firmware and the minimum documentation required for installation and configuration, rather than requiring end users to download and build the entire development repository.

---

## Contributing

Contributions are welcome as the project matures.

Useful contributions may include:

- Firmware development
- Embedded systems engineering
- Astronomy and numerical validation
- Testing
- Hardware experimentation
- Documentation
- Simulation
- Bug reports
- Performance analysis

For significant architectural changes, please discuss the proposed change before submitting a large implementation.

---

## License

OpenTelescopeFirmware is released under the **GNU General Public License v3.0 (GPL-3.0)**.

See [`LICENSE`](LICENSE) for the full license text.

---

## Disclaimer

This project controls physical telescope hardware.

Experimental firmware should be tested carefully and with appropriate safety precautions. Incorrect configuration, wiring, motor parameters, or mechanical limits can cause unexpected movement or hardware damage.

The software's diagnostic output should not be interpreted as proof that a physical hardware component is safe or fault-free unless independently validated.

---

## Project Goal

The immediate goal is simple:

> **Build a reliable, open-source, independently developed GoTo telescope-control system from the ground up—and verify it experimentally rather than merely assuming that it works.**

**v0.2 is the mathematical foundation.  
v0.3 begins the transition toward an actual usable telescope-control system.**