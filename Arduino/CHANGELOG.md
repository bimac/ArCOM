# Changelog

Changes to the Arduino library of ArCOM. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions follow
the `version` field in `library.json`.

## [2.1.0] - 2026-10-14

### Changed

- `available()` returns the number of bytes waiting again, as in 1.0.0. In
  2.0.0 it returned `bool`, so code comparing it with `1` behaves differently.
- Array writes are sent in blocks instead of one `write()` per element.
- Reads use the `readBytes()` of the serial class the object was constructed
  with. Construct ArCOM from the serial object itself (`ArCOM COM(Serial);`)
  to benefit; constructing it from a `Stream &` works, but uses the slower
  generic implementation.
- Measured on Teensy 4.1: array reads 5 -> 33 MB/s, byte-array writes
  5 -> 41 MB/s. On SAMD21: array reads 0.1 -> 1.1 MB/s.

### Added

- `stream()` returns the wrapped `Stream`, e.g. for `print()` or `peek()`.
- Array writes accept `const` data, so constant tables and string literals
  can be sent directly.
- Array reads and writes of `double` on boards with 4-byte doubles.

### Fixed

- Arrays with more than 65535 elements were truncated.
- Writes of more than 16384 bytes sent nothing on SAMD boards.
- A read following `peek()` could hang (SAMD).
- Reads no longer set the stream's read-error flag when the host pauses for
  longer than the stream timeout, and no longer stop `yield()` from running
  while waiting (Teensy).
- Removed `write_now()`, which was declared but never implemented.
- `ArCOMexample` did not compile for AVR boards.
- `library.json`: example paths, and the IEEE754tools dependency for AVR.

## [2.0.0] - 2022-11-17

### Changed

- Rewritten around the templates `read<T>()` and `write<T>()`. The previous
  function names (`readUint16()`, `writeByteArray()`, ...) remain available.
- Examples moved to `Arduino/examples`.

### Added

- 64-bit integers, `float` and `double`.
- On boards with 4-byte doubles (AVR), doubles are converted to and from the
  8-byte format on the wire; using them there produces a deprecation warning.
- PlatformIO support (`platformio.ini`, `library.json`).
- `FloatsAndDoubles` example.

## [1.0.0]

- Original release by Sanworks.

[2.1.0]: https://github.com/bimac/ArCOM/compare/v2.0.0...v2.1.0
[2.0.0]: https://github.com/bimac/ArCOM/releases/tag/v2.0.0
