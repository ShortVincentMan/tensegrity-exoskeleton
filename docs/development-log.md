# Project development history

The Winter and Spring 2026 ME 400 reports provide earlier design context. SURP work expanded sensing, cable-driven actuation, electronics integration, mechanical parts, and exploratory EMG/IMU evaluation. The handwritten notebook remains the detailed original record; its OCR is not treated as authoritative text.

## Preserved stages

- Earlier Arduino experiments: `archive/firmware/old_arduino_sketches/`.
- Distinct SURP Day 33 controller: `archive/firmware/day33-surp/`.
- Original SURP EMG logger: `archive/firmware/surp-emg-logger/`.
- Historical controller formerly beside the dual-IMU test: `archive/firmware/dual-imu-controller-snapshot/`.
- Current modular controller: `firmware/controller/`.
- Independent collection and logger projects: `firmware/imu-trial-logger/` and `firmware/emg-trial-logger/`.

## Repository organization on 2026 October 13

The existing repository was organized on `codex/surp-research-organization`. All pre-existing local firmware changes were preserved. The old README remains under `archive/documentation/`. SURP originals remain in place; importing unique copies did not delete the source archive.

This change moves files, separates build entry points, updates analysis input/output defaults, and documents provenance. It does not redesign the controller or claim new experimental findings. Publication is a separate review step.
