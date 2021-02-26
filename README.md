<h1 align="center">Naepen</h1>

![](https://github.com/bepzi/naepen/workflows/CI/badge.svg) [![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)

A toy polyphonic synthesizer. Not ready for production!

This was my final project for Prof. Taube's
[MUS205](http://cmp.music.illinois.edu/courses/taube/mus205/) at the
University of Illinois. If you are a future MUS205 student, you are
welcome to take inspiration from this code, provided you give proper
attribution.

<p align="center">
    <img src="screenshots/naepen-2020-05-10.png?raw=true" alt="Naepen (2020-05-10)"/>
</p>

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
