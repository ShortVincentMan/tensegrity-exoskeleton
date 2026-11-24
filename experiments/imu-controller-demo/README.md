# IMU controller trials

Six trials (`t1` to `t6`) with the exoskeleton worn, controller in assisted mode, and no added weight. Every row in each CSV is one logged control sample (25 Hz).

| Path | Contents |
| --- | --- |
| `raw/` | The six trial CSVs straight from the logger |
| `processed/` | Combined table, per-trial summary, and my analysis notes |
| `metadata/trials.json` | Load and mode for each trial |
| `protocol.txt` | How the trials were run, and recorder commands |

The `trial_id` column inside the files says `AS_NW_01` for every trial (I didn't change it between runs), so go by the file name.

To regenerate the figures in `results/figures/imu/`, run:

```sh
python analysis/imu/analyze_imu.py --output analysis/reproduced/imu
```

Main takeaways: the IMUs tracked the motion and the controller switched into assist as designed, but the participant didn't feel real lifting help. Trial 4 has a sudden orientation jump, and trials 5 and 6 have short spikes. They're left in as recorded. More in [limitations](../../results/limitations.md).
