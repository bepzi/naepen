from typing import List

import pickle
import numpy as np
import scipy as sp
import matplotlib.pyplot as plt
from matplotlib import rcParams
rcParams['font.family'] = 'monospace'
rcParams['font.monospace'] = ['Roboto Mono', 'Ubuntu Mono', 'Source Code Pro', 'monospace']

R = 48000  # sample rate, samples per second
T = 2048


def make_sawtooth(freq: float):
    out: List[float] = []

    num_harmonics: int = int(R / (2 * freq))

    phase = 0.0
    phase_incr: float = np.pi * 2 / (T - 1)

    for _ in range(T):
        sample = 0.0
        for h in range(1, num_harmonics + 1):
            amp = 1.0 / h
            sample += (np.sin(phase * h) * amp)

        phase += phase_incr
        out.append(sample)

    return np.array(out)


freqs = [1.0, 20.0, 220.0, 440.0, 5000.0, 10000.0]
# for f in freqs:
#     pickle.dump(make_sawtooth(f), open(f'{T}pt_{f}Hz.pickle', 'wb'))
sawtooth_tables = [pickle.load(open(f'{T}pt_{f}Hz.pickle', 'rb')) for f in freqs]

# fig, axes = plt.subplots(ncols=len(freqs))
# for f, table, ax in zip(freqs, sawtooth_tables, axes):
#     ax.plot(np.arange(T), table)
#     ax.set_xlabel('Samples')
#     ax.set_ylabel('Amplitude')
#     ax.set_ylim(-2, 2)
#     ax.set_title(f'{T}-pt Wavetable ({f} Hz)')
# plt.show()

base_wave = np.array(sawtooth_tables[1])
ft = sp.fft.rfft(base_wave)

max_harmonics = ft.shape[0]
print(0)
