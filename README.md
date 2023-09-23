# ThreePM
## Fixed-point MP3 decoding library in C

Based on the helix mp3 library created by RealNetworks in 2003    
Hard-fork of https://github.com/ikjordan/picomp3lib    
Primary purpose of this fork is to make it integrate better into Rust

### Changes since forking from picomp3lib to ThreePM
- Add const qualifiers to pointers in api that never mutated anything
- Remove any pointers from MP3DecInfo so that Rust FFI isn't so painful
- Add #define's for removing static buffers, so that the caller can own all context memory
- ESP32 optimised math functions from https://github.com/ultraembedded/libhelix-mp3/pull/1
- RISCV optimised math functions from lib-helix https://github.com/ultraembedded/libhelix-mp3

# Usage
This library is intended to be consumed exclusely by a Rust wrapper.
All high-level functionality should be implemented there.

## Licence
This library is under the RPSL license.
See [LICENSE.txt](src/LICENSE.txt), [RPSL.txt](src/RPSL.txt), and [RCSL.txt](src/RCSL.txt) for more info.