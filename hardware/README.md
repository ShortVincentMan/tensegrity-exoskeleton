# Hardware

Everything here is my own design. The original tensegrity arm I started from, and any vendor CAD, isn't included.

## Mechanical

`mechanical/components/` has the 3D-printed parts, grouped by function. Each folder keeps the iterations, because most parts took several tries to print, fit, and survive being worn. `.f3d` files are Fusion 360 sources, and `.stl`/`.3mf` files are ready to print.

Latest version of each part:

| Part | File |
| --- | --- |
| Electronics housing and lid | `enclosure/my own cad/refined housing.stl`, `reifned lid.stl` (Fusion source: `electronics housing.f3d`) |
| Fairlead and cap | `fairleads/my own cad/new fairlead.stl` (source: `new fairlead.f3d`), `revised Fairlead Cap.stl` |
| Spool for the SY20STH30 motor | `spools/my own cad/v2 sy20sth30-0604a motor cap.stl` |
| Spool for the 11HS18 motor | `spools/my own cad/tolerant spool 11hs18-0674s motor cap.stl` |
| Motor mount | `motor-mounts/my own cad/new motor plate.stl` |
| IMU mount | `imu-mounts/my own cad/wider imu plate.stl` |
| Node connector | `connectors-and-caps/my own cad/New Aluminum Connector.stl` |

`mechanical/calculations/Tensegrity_Cable_and_Spring_Calculator.xlsx` is the spreadsheet I used to size the springs and string lengths for each cable.

## Electronics

| Folder | What it is |
| --- | --- |
| `electronics/pcb-v1/` | **Rev A, the board I had fabricated (OSH Park).** KiCad project, plus Gerbers and drill files in `fabrication/`. |
| `electronics/pcb-v2/` | A follow-up revision. Designed but not fabricated or tested. |

Rev A design notes:

- The ESP32 and both A4988 drivers sit in female headers, so a dead part can be swapped without desoldering. (A snapped USB connector on the old perfboard is why.)
- JST-XH connectors for the IMUs and motors.
- Motor power is routed separately from logic. A Mini360 buck converter feeds 5 V from the battery, and there's an optional XT30 LiPo input.
- Trace widths are 0.5 mm for logic, 1 mm for motor power, and 1.5 mm for the XT30 input. There's a copper keep-out under the ESP32 antenna.

If KiCad complains about missing footprints when you open the project, some came from downloaded libraries that aren't bundled here. The pin map is in [docs/wiring.md](../docs/wiring.md).
