# Hardware design

Mechanical changes and electronics revisions are preserved with their original filenames. Folder placement groups components; it does not designate any file as the validated final revision.

## Mechanical components

`mechanical/components/` groups the author's own CAD into enclosure, fairleads, motor mounts, IMU mounts, spools, and connectors/caps. Source-folder names are retained within each group to avoid collisions and preserve origin. Fusion `.f3d` sources and printable `.stl`/`.3mf` exports remain distinct. The cable and spring workbook is under `mechanical/calculations/`.

Inherited CAD and assemblies are under the ignored `local-review/mechanical/inherited-cad/` folder on the organizing machine. Assemblies retain their source hierarchy; authorship and native assembly references need review before import into published hardware. Printer-specific jobs remain in local review. Explicitly restricted CAD is only in the original SURP archive.

### Latest revision of each part

Each folder keeps the design iterations. The most recent revision, judged by file name, is:

| Part | File |
| --- | --- |
| Electronics housing and lid | `enclosure/my own cad/refined housing.stl`, `reifned lid.stl` (Fusion source: `electronics housing.f3d`) |
| Fairlead and cap | `fairleads/my own cad/new fairlead.stl` (source `new fairlead.f3d`), `revised Fairlead Cap.stl` |
| Spool / motor cap (SY20STH30 motor) | `spools/my own cad/v2 sy20sth30-0604a motor cap.stl` |
| Spool / motor cap (11HS18 motor) | `spools/my own cad/tolerant spool 11hs18-0674s motor cap.stl` |
| Motor mount | `motor-mounts/my own cad/new motor plate.stl` |
| IMU mount | `imu-mounts/my own cad/wider imu plate.stl` |
| Node connector | `connectors-and-caps/my own cad/New Aluminum Connector.stl` |

## Electronics

`electronics/pcb-v1/` holds the SURP-Motors project; `pcb-v2/` holds SURP_V2. V1 fabrication Gerbers and drill files are under `fabrication/`. V2 is preserved independently and is not assumed fabrication-validated.

KiCad sources contain embedded schematic symbols and board footprints. External footprint lookup and 3D-model dependencies still need review. Original footprint tables pointed to local paths that do not match the downloaded folder layout, and the V2 board references `esp32:fixed_ESP32_30pin` while the downloaded ZIP contains differently named footprints. Do not guess that these are interchangeable.

The original `fp-lib-table` files are in ignored `local-review/electronics/`; they were not installed into the portable projects. Document and license required custom libraries before adding a project-relative table. Native KiCad opening, electrical-rule checks, and 3D models remain to be verified.

Existing wiring and pin assignments are preserved in the [historical firmware README](../archive/documentation/README-before-research-organization.md). The current controller's actual pins remain in `firmware/controller/include/pins.h`; confirm the firmware target when wiring a prototype.
