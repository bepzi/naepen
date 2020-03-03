from typing import List

import pickle
import sounddevice as sd
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

    return np.array(out) / 1.5741594205752307


freqs = [1.0, 20.0, 220.0, 440.0, 800.0, 5000.0, 10000.0]
# for f in freqs:
#     pickle.dump(make_sawtooth(f), open(f'{T}pt_{f}Hz.pickle', 'wb'))
sawtooth_tables = [pickle.load(open(f'{T}pt_{f}Hz.pickle', 'rb')) for f in freqs]

# fig, axes = plt.subplots(nrows=len(freqs))
# for f, table, ax in zip(freqs, sawtooth_tables, axes):
#     ax.plot(np.arange(T), table)
#     ax.set_xlabel('Samples')
#     ax.set_ylabel('Amplitude')
#     ax.set_ylim(-1.1, 1.1)
#     ax.set_title(f'{T}-pt Wavetable ({f} Hz)')
# plt.show()

base_wave = sawtooth_tables[1]
wavetable = [(base_wave.copy(), 20.0)]

ft = sp.fft.rfft(base_wave)
ft[0] = ft[T // 2] = 0  # Zero-out DC offset and Nyquist

max_harmonics = T // 2
min_val = 0.000001  # -120 dB

while abs(ft[max_harmonics]) < min_val and max_harmonics > 0:
    max_harmonics -= 1

for _ in range(1, 32):
    max_harmonics = max_harmonics // 2
    if max_harmonics <= 0:
        break

    ft[max_harmonics:(max_harmonics * 2) + 1] = 0
    wavetable.append((sp.fft.irfft(ft), R // (2 * max_harmonics)))

fig, axes = plt.subplots(nrows=len(wavetable))
for table, ax in zip(wavetable, axes):
    ax.plot(np.arange(T), table[0])
    ax.set_title(f'Safe For <= {table[1]} Hz')
    ax.set_ylim(-1.1, 1.1)
plt.show()


samples = np.zeros((10 * R,))
freqs = np.linspace(20.0, 20000.0, samples.shape[0])

phase = 0.0

for i in range(0, samples.shape[0]):
    # TODO: Choose the correct wavetable and interpolate between samples and wavetables
    samples[i] = wavetable[1][0][int(phase)]

    f = freqs[i]
    phase_incr = f * (T / R)

    phase += phase_incr
    if phase > T:
        phase -= T

samples *= 0.1
sd.play(samples, R, blocking=True)
