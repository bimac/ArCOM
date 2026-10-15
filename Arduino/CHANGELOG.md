# Changelog

Changes to the Arduino library of ArCOM. The format follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions follow
the `version` field in `library.json`.

## [2.1.0] - 2026-10-15

### Changed

- `available()` returns the number of bytes waiting again, as in 1.0.0. In
  2.0.0 it returned `bool`, so code comparing it with `1` behaves differently.
- Array writes are sent in blocks instead of one `write()` per element.
- Reads use the `readBytes()` of the serial class the object was constructed
  with. Construct ArCOM from the serial object itself (`ArCOM COM(Serial);`)
  to benefit; constructing it from a `Stream &` works, but uses the slower
  generic implementation.
- On boards with 4-byte doubles (AVR), using `double` with ArCOM is now a
  compile error: use `float` instead. The conversion used in 2.0.0 gave wrong
  results for 0, infinity, NaN and out-of-range values, and the dependency on
  IEEE754tools is gone.
- On Teensy 3.x, LC and 4.x, reads from USB serial (including `SerialUSB1`
  and `SerialUSB2` of the dual and triple serial USB types) use the core's
  `usb_serial*_read()` directly. On Teensy 4, values read one at a time (e.g.
  `readUint32()`) are about 2.5 times faster.
- Measured on Teensy 4.1: array reads 5 -> 33 MB/s, byte-array writes
  5 -> 41 MB/s. On SAMD21: array reads 0.1 -> 1.1 MB/s.

### Added

- `stream()` returns the wrapped `Stream`, e.g. for `print()` or `peek()`.
- Array writes accept `const` data, so constant tables and string literals
  can be sent directly.
- Sending or receiving pointers is a compile error, e.g. `write("hello")`
  without a number of values; it used to send the address.

### Fixed

- Arrays with more than 65535 elements were truncated.
- Writes of more than 16384 bytes sent nothing on SAMD boards.
- A read following `peek()` could hang (SAMD).
- Reads no longer set the stream's read-error flag when the host pauses for
  longer than the stream timeout (Teensy).
- Reads call `yield()` while waiting for data, also on boards whose
  `available()` does not (e.g. SAMD, Due), so that e.g. the Scheduler library
  keeps running.
- A stream whose `readBytes()` returns an invalid count (e.g. an error code)
  can no longer make a read write past the end of the buffer.
- The previous function names (`writeUint32()`, ...) take their arguments by
  value again, so bit-fields and members of packed structs can be passed, as
  in 1.0.0.
- Array reads and writes accept `volatile` arrays again, as in 1.0.0 (e.g.
  `readByteArray()` on a `volatile byte[]` in the Bpod state machine
  firmware); 2.0.0 failed to compile them.
- Including `ArCOM.h` no longer redefines and removes a macro called `ALIAS`.
- Stream classes without an accessible `readBytes(char *, size_t)` failed to
  compile; they now use the generic implementation.
- `ArCOMexample` did not compile for AVR boards.

### Removed

- `write_now()`, which was declared but never implemented.

## [2.0.0] - 2022-11-17

### Changed

- Rewritten around the templates `read<T>()` and `write<T>()`. The previous
  function names (`readUint16()`, `writeByteArray()`, ...) remain available.

### Added

- 64-bit integers, `float` and `double`.
- Structs (and arrays of them) can be sent and received in one call, e.g.
  `write(sample)` and `read<Sample>()`. Declare them
  `__attribute__((packed))` so that no padding is transferred.
- On boards with 4-byte doubles (AVR), doubles are converted to and from the
  8-byte format on the wire; using them there produces a deprecation warning.
- PlatformIO support (`platformio.ini`, `library.json`).

## [1.0.0]

- Original release by Sanworks.

[2.1.0]: https://github.com/bimac/ArCOM/compare/v2.0.0...v2.1.0
[2.0.0]: https://github.com/bimac/ArCOM/releases/tag/v2.0.0
