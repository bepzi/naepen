<h1 align="center">Naepen</h1>

![](https://github.com/bepzi/naepen/workflows/CI/badge.svg) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

A polyphonic synthesizer.

### Building

On Arch Linux:

```bash
sudo pacman -S juce fftw
git clone --recurse-submodules <repo_url.git> <repo> && cd <repo>
cd Builds/LinuxMakefile  # Alternatively, build with CMake
CONFIG=Release make -j
```
