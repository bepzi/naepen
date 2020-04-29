<h1 align="center">Naepen</h1>

![](https://github.com/bepzi/naepen/workflows/CI/badge.svg)

A cross-platform polyphonic synthesizer.

### Building

On Arch Linux:

```bash
sudo pacman -S juce fftw
git clone <repo_url.git> <repo> && cd <repo>
cd Builds/LinuxMakefile  # Alternatively, build in CLion
CONFIG=Release make -j
```
