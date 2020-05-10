<h1 align="center">Naepen</h1>

![](https://github.com/bepzi/naepen/workflows/CI/badge.svg) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

A polyphonic synthesizer.

### Building

**Arch Linux**

```bash
sudo pacman -S fftw
git clone --recurse-submodules <repo_url.git> <repo> && cd <repo>
cd Builds/LinuxMakefile  # Alternatively, build with CMake
CONFIG=Release make -j
```

**Ubuntu 18.04**

```bash
sudo apt install libfftw3-dev libxinerama-dev libasound2-dev libfreetype6-dev \
                 libwebkit2gtk-4.0-dev libcurl4-openssl-dev libflac-dev \
                 libvorbis-dev libogg-dev libjack-dev
git clone --recurse-submodules <repo_url.git> <repo> && cd <repo>
cd Builds/LinuxMakefile  # Alternatively, build with CMake
CONFIG=Release make -j
```
