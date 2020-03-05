from typing import List

import pickle
import sounddevice as sd
import numpy as np
import scipy as sp
from scipy.io import wavfile
from scipy import signal
import matplotlib.pyplot as plt
from matplotlib import rcParams

rcParams['font.family'] = 'monospace'
rcParams['font.monospace'] = ['Roboto Mono', 'Ubuntu Mono', 'Source Code Pro', 'monospace']

R = 48000  # sample rate, samples per second
T = 2048


def pad_waveform(w: np.ndarray) -> np.ndarray:
    if w.shape[0] == T:
        cpy = np.empty((w.shape[0] + 1,))
        cpy[:-1] = w
        cpy[-1] = cpy[0]
        w = cpy
    assert (w.shape[0] == T + 1)
    return w


def play_waveform_sweep(waveform: np.ndarray, duration_sec: float = 5.0, do_play: bool = True) -> np.ndarray:
    waveform = pad_waveform(waveform)

    samples = np.zeros((duration_sec * R,))
    freqs = np.geomspace(16.0, R / 2, samples.shape[0])

    phase = 0.0
    for i in range(0, samples.shape[0]):
        idx0 = int(phase)
        idx1: int = idx0 + 1
        frac: float = phase - idx0

        s0 = waveform[idx0]
        s1 = waveform[idx1]

        # Interpolate between samples
        samples[i] = s0 + frac * (s1 - s0)

        f = freqs[i]
        phase_incr = f * (T / R)

        phase += phase_incr
        if phase > T:
            phase -= T

    samples *= 0.1
    if do_play:
        sd.play(samples, R, blocking=True)
    return samples


def play_wavetable_sweep(wavetable: np.ndarray, duration_sec: float = 5.0, do_play: bool = True) -> np.ndarray:
    padded_table = [(pad_waveform(wave), top_freq) for wave, top_freq in wavetable]

    samples = np.zeros((duration_sec * R,))
    freqs = np.geomspace(16.0, R / 2, samples.shape[0])

    best_table = padded_table[0][0]

    phase = 0.0
    for i in range(0, samples.shape[0]):
        f = freqs[i]

        for j, (table, top_freq) in enumerate(padded_table):
            if j == len(padded_table) - 1:
                best_table = padded_table[-1][0]

            if top_freq >= f:
                best_table = table
                break

        idx0 = int(phase)
        idx1: int = idx0 + 1
        frac: float = phase - idx0

        s0 = best_table[idx0]
        s1 = best_table[idx1]

        # Interpolate between samples
        samples[i] = s0 + frac * (s1 - s0)

        phase_incr = f * (T / R)
        phase += phase_incr
        if phase > T:
            phase -= T

    samples *= 0.1
    if do_play:
        sd.play(samples, R, blocking=True)
    return samples


def show_spectrogram(waveform: np.ndarray):
    f, t, Zxx = sp.signal.stft(waveform, R, nperseg=512)

    fig, ax = plt.subplots()
    ax.pcolormesh(t, f, np.abs(Zxx), cmap='Greens')
    ax.set_ylabel("Frequency (Hz)")
    ax.set_xlabel("Time (seconds)")
    plt.show()


def make_sawtooth(top_freq: float) -> np.ndarray:
    out = []

    num_harmonics = int(R / (2 * top_freq))

    phase = 0.0
    phase_incr: float = 2 * np.pi / T

    for _ in range(T):
        sample = 0.0
        for h in range(1, num_harmonics + 1):
            amp = 1.0 / h
            sample += (np.sin(phase * h) * amp)

        phase += phase_incr
        out.append(sample)
    out = np.array(out)

    return out / np.max(out)


freqs = [20.0, 40.0, 80.0, 160.0, 320.0, 640.0, 1280.0, 2560.0, 5120.0, 10240.0, 20480.0]
# for f in freqs:
#     pickle.dump(make_sawtooth(f), open(f'{T}pt_{f}Hz.pickle', 'wb'))
sawtooth_tables = [pickle.load(open(f'{T}pt_{f}Hz.pickle', 'rb')) for f in freqs]

show_spectrogram(play_waveform_sweep(sawtooth_tables[0], 10, do_play=False))


def generate_wavetable(initial_waveform: np.ndarray) -> np.ndarray:
    fft = sp.fft.rfft(initial_waveform)
    fft[0] = fft[T // 2] = 0

    max_harmonic = T // 2
    min_val = 0.000001  # -120 dB
    while np.abs(fft[max_harmonic]) < min_val and max_harmonic != 0:
        max_harmonic -= 1

    top_freq = (2 * R) / (3 * max_harmonic)

    out = []
    while max_harmonic != 0:
        table_fft = np.zeros(fft.shape, dtype=np.complex128)
        table_fft[1:max_harmonic + 1] = fft[1:max_harmonic + 1]

        table_wav = sp.fft.irfft(table_fft)

        out.append((table_wav, top_freq))
        top_freq *= 2
        max_harmonic //= 2

    # Scale all tables by global max
    global_max = np.max(out[0][0])
    out = [(t / global_max, f) for t, f in out]
    return np.array(out)


sawtooth_wavetable = generate_wavetable(sawtooth_tables[0])
sweep = play_wavetable_sweep(sawtooth_wavetable, 10, do_play=False)
# sp.io.wavfile.write('20Hz_20kHz_sweep_sawtooth.wav', R, sweep)
show_spectrogram(sweep)
