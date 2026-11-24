# Analysis

## Python (IMU trials)

```sh
python -m venv .venv && source .venv/bin/activate
pip install -r analysis/requirements.txt
python analysis/imu/analyze_imu.py --output analysis/reproduced/imu
```

`imu/analyze_imu.py` reads the six trial CSVs from `experiments/imu-controller-demo/raw/` and writes per-trial summaries, a combined table, and plots. Use `--input` and `--output` to point it somewhere else.

`acquisition/record_imu.py` is the recorder I used during trials. It reads the ESP32's serial output and saves a CSV (for example: `python analysis/acquisition/record_imu.py --port COM5 --out t7.csv --trial t7 --load 0`). See the [protocol](../experiments/imu-controller-demo/protocol.txt).

## MATLAB (EMG pilot)

- `emg/process_all_emg.m`: removes the DC offset from channel 2 and computes rolling RMS for the 12 condition recordings. It asks you to pick the folder of spreadsheets.
- `emg/combined-channel-preview/`: an earlier two-channel version that made the poster comparison plot.
- `legacy/emg/`: my first attempts, as Live Scripts. They're kept for reference, but they don't handle the timestamp problems.

The raw EMG recordings aren't published, so these scripts are here to show the processing.
