/*
ArCOM
Copyright (C) 2016 Sanworks LLC, Sound Beach, New York, USA
Copyright (C) 2022 Florian Rau

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

This program is distributed  WITHOUT ANY WARRANTY and without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once
#include <Arduino.h>

// Macro for generating wrapper functions
// cf. https://www.fluentcpp.com/2017/10/27/function-aliases-cpp/
#define ALIAS(alias, target)                      \
template<typename... Args>                        \
inline auto alias(Args&&... args) ->              \
decltype(target(static_cast<Args&&>(args)...)) {  \
  return target(static_cast<Args&&>(args)...);    \
}

class ArCOM
{
  // Used for restricting the templated constructor to classes derived from
  // Stream (needs to be declared ahead of the constructor)
  static void isStream(Stream *);

public:

  // Constructor: sets the interface (Serial, Serial1, SerialUSB, etc.)
  ArCOM(Stream &s) : ArCOMstream(&s), readBytesFn(&readBytesImpl<Stream>) {}

  // Constructor for classes derived from Stream: binds to the readBytes() of
  // that class, which may be faster than the one of Stream
  template<typename S, typename = decltype(isStream((S *) nullptr))>
  ArCOM(S &s) : ArCOMstream(&s), readBytesFn(&readBytesImpl<S>) {}

  // Serial functions
  unsigned int available();
  void flush();
  Stream &stream() { return *ArCOMstream; }

  // Template: Write scalar
  template<typename T> void write(T data) {
    ArCOMstream->write(reinterpret_cast<const uint8_t *>(&data), sizeof(T));
  }

  // Template: Write array (raw bytes, does not make use of write<T>())
  template<typename T> void write(const T *data, size_t nValues) {
    writeChunked(reinterpret_cast<const uint8_t *>(data), nValues * sizeof(T));
  }

  // Template: Read scalar
  template<typename T> T read() {
    T data;
    readBlocking(reinterpret_cast<uint8_t *>(&data), sizeof(T));
    return data;
  }

  // Template: Read array (raw bytes, does not make use of read<T>())
  template<typename T> void read(T *data, size_t nValues) {
    readBlocking(reinterpret_cast<uint8_t *>(data), nValues * sizeof(T));
  }

  // Wrapper functions for backwards-compatibility (read scalar)
  ALIAS(readByte,         read<uint8_t>)
  ALIAS(readUint8,        read<uint8_t>)
  ALIAS(readUint16,       read<uint16_t>)
  ALIAS(readUint32,       read<uint32_t>)
  ALIAS(readUint64,       read<uint64_t>)
  ALIAS(readChar,         read<char>)
  ALIAS(readInt8,         read<int8_t>)
  ALIAS(readInt16,        read<int16_t>)
  ALIAS(readInt32,        read<int32_t>)
  ALIAS(readInt64,        read<int64_t>)
  ALIAS(readFloat,        read<float>)
  ALIAS(readDouble,       read<double>)

  // Wrapper functions for backwards-compatibility (read array)
  ALIAS(readByteArray,    read<uint8_t>)
  ALIAS(readUint8Array,   read<uint8_t>)
  ALIAS(readUint16Array,  read<uint16_t>)
  ALIAS(readUint32Array,  read<uint32_t>)
  ALIAS(readUint64Array,  read<uint64_t>)
  ALIAS(readCharArray,    read<char>)
  ALIAS(readInt8Array,    read<int8_t>)
  ALIAS(readInt16Array,   read<int16_t>)
  ALIAS(readInt32Array,   read<int32_t>)
  ALIAS(readInt64Array,   read<int64_t>)
  ALIAS(readFloatArray,   read<float>)
  ALIAS(readDoubleArray,  read<double>)

  // Wrapper functions for backwards-compatibility (write scalar)
  ALIAS(writeByte,        write<uint8_t>)
  ALIAS(writeUint8,       write<uint8_t>)
  ALIAS(writeUint16,      write<uint16_t>)
  ALIAS(writeUint32,      write<uint32_t>)
  ALIAS(writeUint64,      write<uint64_t>)
  ALIAS(writeChar,        write<char>)
  ALIAS(writeInt8,        write<int8_t>)
  ALIAS(writeInt16,       write<int16_t>)
  ALIAS(writeInt32,       write<int32_t>)
  ALIAS(writeInt64,       write<int64_t>)
  ALIAS(writeFloat,       write<float>)
  ALIAS(writeDouble,      write<double>)

  // Wrapper functions for backwards-compatibility (write array)
  ALIAS(writeByteArray,   write<uint8_t>)
  ALIAS(writeUint8Array,  write<uint8_t>)
  ALIAS(writeUint16Array, write<uint16_t>)
  ALIAS(writeUint32Array, write<uint32_t>)
  ALIAS(writeUint64Array, write<uint64_t>)
  ALIAS(writeCharArray,   write<char>)
  ALIAS(writeInt8Array,   write<int8_t>)
  ALIAS(writeInt16Array,  write<int16_t>)
  ALIAS(writeInt32Array,  write<int32_t>)
  ALIAS(writeInt64Array,  write<int64_t>)
  ALIAS(writeFloatArray,  write<float>)
  ALIAS(writeDoubleArray, write<double>)

private:
  Stream *ArCOMstream;
  size_t (*readBytesFn)(Stream *, uint8_t *, size_t);

  template<typename S> static size_t readBytesImpl(Stream *s, uint8_t *data, size_t nBytes) {
    return static_cast<S *>(s)->readBytes((char *) data, nBytes);
  }

  // Writes in chunks, as some cores cannot send large buffers in a single
  // call of write() (e.g., SAMD: 16384 bytes). The chunk size is kept well
  // below that; it has no measurable effect on throughput.
  void writeChunked(const uint8_t *data, size_t nBytes) {
    const size_t chunkSize = 1024;
    while (nBytes > 0) {
      size_t nWrite = min(nBytes, chunkSize);
      ArCOMstream->write(data, nWrite);
      data += nWrite;
      nBytes -= nWrite;
    }
  }

  // Blocks until all bytes have been read. The first byte is obtained by
  // means of read(), which also returns a byte held back by peek() (SAMD).
  // The rest is read in bulk, but only as far as available: readBytes() does
  // not have to wait and never runs into its timeout.
  void readBlocking(uint8_t *data, size_t nBytes) {
    while (nBytes > 0) {
      int c = ArCOMstream->read();
      if (c < 0) {
        ArCOMstream->available();  // some cores yield / service USB in here
        continue;
      }
      *data++ = c;
      if (--nBytes == 0)
        return;
      int nAvailable = ArCOMstream->available();
      if (nAvailable > 0) {
        size_t nRead = readBytesFn(ArCOMstream, data, min((size_t) nAvailable, nBytes));
        data += nRead;
        nBytes -= nRead;
      }
    }
  }
};

#undef ALIAS

// Template specializations for boards with 4-byte "doubles"
#if __SIZEOF_DOUBLE__ != 8
  #define ARCOM_NO_DOUBLE __attribute__((deprecated("Target does not support 8-byte doubles. Use float instead.")))
  template<> double ArCOM::read<double>() ARCOM_NO_DOUBLE;
  template<> void ArCOM::read<double>(double *data, size_t nValues) ARCOM_NO_DOUBLE;
  template<> void ArCOM::write<double>(double data) ARCOM_NO_DOUBLE;
  template<> void ArCOM::write<double>(const double *data, size_t nValues) ARCOM_NO_DOUBLE;
  #undef ARCOM_NO_DOUBLE
#endif
