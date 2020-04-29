<h1 align="center">Naepen</h1>

![CI Status](https://github.com/bepzi/naepen/workflows/.github/workflows/ci.yml/badge.svg)

A cross-platform polyphonic synthesizer.

### Building

On Arch Linux:

```bash
sudo pacman -S juce fftw
git clone <repo_url.git> <repo> && cd <repo>
cd Builds/LinuxMakefile  # Alternatively, build in CLion
CXX=$(which clang++) CONFIG=Release make -j
```
