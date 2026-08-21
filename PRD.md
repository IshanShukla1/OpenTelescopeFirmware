# Product Requirements Document

## 1. Executive summary

This document specifies the firmware for an open-source retrofit computer-assisted GoTo system for an equatorial telescope mount. The initial known platform is an ESP32 controlling two LV8729 STEP/DIR drivers and NEMA17 motors: one each for right ascension (RA) and declination (DEC). It is an engineering specification, not an implementation commitment.

**Status terms:** **MUST** is mandatory for a release; **SHOULD** is strongly preferred unless a documented reason prevents it; **MAY** is optional. Values marked **TBD** require a recorded decision before the affected capability is released.

## 2. Vision, problem, and scope

### Vision

Provide a modular, observable and safe firmware foundation for precise equatorial-mount motion, tracking, GoTo and computer control, while preserving room for independently designed diagnostics and future research.

### Problem statement

Existing manual mounts require accurate, coordinated axis control and trustworthy position knowledge to track or point a telescope. A retrofit must accommodate unknown mechanics, protect the mount from unsafe commands, and make faults diagnosable without confusing astronomy errors with electrical or mechanical failures.

### Goals

- Independently designed, portable telescope-control firmware.
- Deterministic two-axis motion with configurable mount mechanics.
- Clear separation of astronomy calculations, mount-domain behavior, motion, and hardware.
- Evidence-rich safety, diagnostics, and recovery behavior.

### Non-goals (initial releases)

- Cloning, rebranding, or claiming compatibility with another telescope firmware.
- Assuming encoders, switches, current sensing, IMU, or a specific ESP32 board.
- Finalizing a public protocol, alignment method, pointing model, GPIO map, or mechanical values.
- Autonomous learning, predictive maintenance, or adaptive correction in the initial release.

## 3. Users and representative use cases

| User | Use case |
|---|---|
| Builder | Configure the known mechanics and validate each axis safely. |
| Observer | Start a verified session, align, track, issue GoTo, park, and inspect status. |
| Integrator | Control the mount over a documented serial interface; Wi-Fi is planned. |
| Developer/support contributor | Reproduce a fault using structured evidence and tests. |

## 4. Confirmed context and constraints

| Area | Current fact | Constraint / decision status |
|---|---|---|
| Controller | ESP32 development board | Exact board and pinout: TBD. |
| Drivers | Two LV8729 drivers | STEP/DIR; direction and microstepping configurable. |
| Motors | Two NEMA17 motors | Electrical/mechanical specifications: TBD. |
| Supply | Approximately 12 V motor supply | Protection, grounding and logic interface: TBD hardware design. |
| Mount | Retrofitted equatorial RA/DEC mount | Ratios, worm data, backlash and travel limits configurable. |
| Connectivity | USB/serial; ESP32 Wi-Fi available | Initial command protocol and Wi-Fi scope: TBD. |

Firmware cannot guarantee physical collision avoidance without authoritative travel feedback or conservatively configured software limits.

## 5. Functional requirements

| ID | Requirement | Acceptance criterion |
|---|---|---|
| FR-CFG-001 | The system MUST validate and load a versioned configuration containing axis mechanics, driver settings, direction, backlash and safety limits where available. | Invalid, missing, or incompatible configuration prevents motion and raises a documented fault. |
| FR-MOT-001 | The motion subsystem MUST command each axis through an abstract motion interface without astronomy or command code accessing GPIO. | Architecture/unit tests demonstrate the boundary; code review finds no forbidden access. |
| FR-MOT-002 | The system MUST support controlled acceleration, deceleration, stopping, and coordinated RA/DEC trajectories. | Integration tests verify planned motions reach requested axis targets or issue a defined fault. |
| FR-MOT-003 | The system MUST expose commanded motion and execution state for diagnostics. | A state snapshot contains axis command, planner state, position estimate and inhibition/fault state. |
| FR-TRK-001 | The mount core MUST support sidereal tracking as a distinct mount operating mode. | Test/simulation verifies tracking commands are generated from configured mechanics and time inputs. |
| FR-GOTO-001 | The mount core MUST accept validated GoTo targets, plan coordinated axis motion, and report completion, rejection, interruption, or fault. | Each outcome is externally observable and logged with a correlation ID. |
| FR-ASTRO-001 | Astronomy calculations MUST be independent of hardware interfaces and use explicit time/location inputs. | Host-runnable tests execute astronomy functions without ESP32 or driver dependencies. |
| FR-ALIGN-001 | The design MUST provide an alignment extension boundary; the initial supported model is TBD. | Unsupported alignment operations are rejected explicitly, not silently approximated. |
| FR-SAFE-001 | The system MUST inhibit unsafe motion on active critical safety faults and emergency stop. | Injected critical fault removes step output through the safety path and latches the condition. |
| FR-COM-001 | The system MUST provide a serial development/diagnostic command interface with validation and responses. | Invalid commands do not alter motion state and provide a structured error response. |
| FR-DIAG-001 | The system MUST record structured events, state snapshots, counters, and faults as specified in [DEBUGGING_AND_DIAGNOSTICS.md](DEBUGGING_AND_DIAGNOSTICS.md). | A diagnostic export includes the required available sections and redacts no safety-relevant local data. |
| FR-PARK-001 | The system SHOULD support a configured park procedure after a valid position reference exists. | Park state, prerequisites, result, and interruption are observable. |
| FR-PERSIST-001 | The system SHOULD persist validated configuration and only persist runtime state when its validity conditions are explicit. | Restart behavior is documented and stale/unsafe state is not treated as authoritative. |

## 6. Non-functional requirements

| ID | Requirement | Acceptance criterion |
|---|---|---|
| NFR-RT-001 | The critical step-generation path MUST avoid blocking I/O, dynamic allocation, and non-deterministic network work. | Design review and timing instrumentation show prohibited operations are absent from the path. |
| NFR-RT-002 | The motion-control subsystem MUST maintain deterministic step timing within a timing tolerance **TBD** under documented normal load. | Hardware-in-the-loop timing measurement records jitter against the approved tolerance. |
| NFR-REL-001 | Unexpected reset, watchdog, configuration, and driver-related abnormal conditions MUST be recorded when technically possible. | Reset cause and recent event context are available after restart. |
| NFR-TEST-001 | Portable domain logic MUST be unit-testable independent of hardware. | Test suite runs astronomy, configuration validation, planner, and error-model tests against fakes. |
| NFR-EXT-001 | Optional sensors and communications MUST attach through explicit interfaces, not changes to domain ownership. | An extension design can be added without astronomy code importing a hardware driver. |

## 7. Safety, reliability, and real-time policy

- **SAFE-001:** Motion MUST be inhibited until configuration and startup checks required for the enabled hardware profile succeed.
- **SAFE-002:** Emergency stop MUST request immediate step-output disable, cancel active motion, record the event, and require an explicit recovery procedure.
- **SAFE-003:** Configured soft limits MUST be checked during planning and execution where a trusted position reference exists.
- **SAFE-004:** A fault that makes position trust invalid MUST mark position as untrusted; GoTo, park, and limit enforcement based on it MUST be governed by the fault policy.
- **REL-001:** Failures must be isolated to the smallest safe subsystem; unrelated diagnostic access should remain available where safe.
- **RT-001:** High-rate motion execution owns timing; command parsing, persistence, logging transport, and Wi-Fi must not delay it.

Detailed severity, latching, recovery, and emergency behavior are normative in [ERROR_MANAGEMENT.md](ERROR_MANAGEMENT.md).

## 8. Communication, configuration, and diagnostics

Communication commands MUST be authenticated/authorized only if the chosen deployment requires it (TBD), validated before dispatch, correlated with resulting events, and never bypass safety. USB/serial is the initial required diagnostic path; Wi-Fi is planned. Configuration is declarative, versioned, validated, and snapshotable. Diagnostic observability, log retention, and report contents are defined in [DEBUGGING_AND_DIAGNOSTICS.md](DEBUGGING_AND_DIAGNOSTICS.md).

## 9. Extensibility and future research

Planned extension points include limit/homing sensors, encoders, power sensing, environmental inputs, and Wi-Fi control. The following are **research**, not current capabilities: adaptive pointing correction, mount-specific error models, periodic-error characterization, learned mechanical behavior, health monitoring, predictive maintenance, automated fault classification, and self-characterization.

## 10. Milestones

| Milestone | Scope | Exit condition |
|---|---|---|
| M0 Specification | These four documents and decisions register | Reviewed; TBD owners identified. |
| M1 Bring-up | Board profile, driver interface, safe axis exercise | No telescope operation until hardware safety checks pass. |
| M2 Motion foundation | Planner, controlled motion, telemetry, HIL tests | Determinism and stop behavior measured. |
| M3 Mount functions | Tracking, coordinates, basic GoTo, configuration | Acceptance tests for supported assumptions pass. |
| M4 Integration | Stable command interface, alignment/parking choices | End-to-end user workflow verified. |
| M5 Extensions | Sensors, Wi-Fi, research prototypes | Each feature separately specified and validated. |

## 11. Risks, assumptions, and open decisions

**Risks:** unmeasured mechanics can cause missed steps; lack of sensors limits position certainty; 12 V power/wiring issues can mimic firmware faults; Wi-Fi may compromise timing if improperly isolated; alignment/coordinate convention mistakes can cause unsafe pointing.

**Assumptions:** the mount is mechanically suitable for motorization; LV8729 modules are correctly configured for their motor supply/current requirements; the user can establish a safe initial mechanical envelope.

**Decisions pending:** board/pinout; driver enable/sleep/fault wiring; power and emergency-stop topology; steps/revolution, ratios, microstepping and backlash; whether any position reference exists; timing tolerance; command protocol; location/time source; coordinate conventions; alignment and parking models; persistence medium/format; diagnostic storage budget.

## 12. Traceability and glossary

Implementation locations and dependency controls for these requirements are specified in [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md). Fault semantics and IDs are in [ERROR_MANAGEMENT.md](ERROR_MANAGEMENT.md).

| Term | Meaning |
|---|---|
| Axis position | Firmware estimate of mechanical axis angle, with a stated trust state. |
| Mount coordinates | Coordinates expressed in the mount/axis model. |
| Position reference | Valid basis (homing, alignment, encoder, or declared manual procedure) for treating an estimate as trusted. |
| Motion inhibition | Safety-controlled refusal or prevention of step generation. |
