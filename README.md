# mp3fp
## Fixed-point MP3 decoding library in C

Based on the helix mp3 library created by RealNetworks in 2003    
Hard-fork of https://github.com/ikjordan/picomp3lib    
Primary purpose of this fork is to make it integrate better into Rust

### Changes since forking
- Add const qualifiers to pointers in api that never mutated anything
- Remove any pointers from MP3DecInfo so that Rust FFI isn't so painful
- Add #define's for removing static buffers, so that the caller can own all context memory

### Changes from lib-helix https://github.com/ultraembedded/libhelix-mp3
- RISCV optimised math functions

### Changes from picomp3lib
- Size of frame returned by `MP3FrameInfo`
- Improved `MULSHIFT32` for ARM Cortex M0+
- CMake files, so can be used as a library for Pico

### Changes from AdaFruit_MP3 (from picomp3lib) https://github.com/adafruit/Adafruit_MP3  
- ARM optimised math functions

# Directory Structure
## `src` directory
Base source code for library. The public interface consists 6 functions, however typically the higher level public interface
defined in [music_file.h](test/music_file.h) in the `interface` directory is used.
### `MP3InitDecoder`
Initialises the decoder. This is clearing datastructures and (potentially) allocating.
### `MP3FreeDecoder`
Clears buffers
### `MP3Decode`
Decodes one frame of MP3 data
### `MP3GetLastFrameInfo`
Returns info about last MP3 frame decoded (number of samples decoded, sample rate, bitrate, etc). Typically called directly
after a call to MP3Decode
### `MP3GetNextFrameInfo`
Parses the next MP3 frame header
### `MP3FindSyncWord`
Locates the next byte-aligned sync word in the raw mp3 stream

# Usage
This library is intended to be consumed exclusely by a Rust wrapper.
All high-level functionality should be implemented there.
