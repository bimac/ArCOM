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
#define ALIAS(alias, target) template<typename... Args> inline auto alias(Args&&... args) -> decltype(target(args...)) { return target(args...); }


class ArCOM
{
public:

  // Constructor
  ArCOM(Stream &s);

  // Serial functions
  bool available();
  void flush();

  // Template: Write scalar
  template<typename T> void write(T data) {
    buffer <T>buf;
    buf.data = data;
    for (size_t i = 0; i<sizeof(T); i++)
      ArCOMstream->write(buf.bytes[i]);
  }

  // Template: Write array
  template<typename T> void write(T *data, size_t nValues) {
    for (uint16_t i = 0; i < nValues; i++)
      write<T>(data[i]);
  }

  // Template: Read scalar
  template<typename T> T read() {
    buffer <T>buf;
    for (size_t i = 0; i<sizeof(T); i++) {
      while (ArCOMstream->available() == 0) {}
      buf.bytes[i] = ArCOMstream->read();
    }
    return buf.data;
  }

  // Template: Read array
  template<typename T> void read(T *data, size_t nValues) {
    for (uint16_t i = 0; i < nValues; i++)
      data[i] = read<T>();
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
  template<typename T> union buffer {
    T data;
    uint8_t bytes[sizeof(T)];
  };
  Stream *ArCOMstream;
};

// Template specializations for boards with 4-byte "doubles"
#if __SIZEOF_DOUBLE__ != 8
  template<> double ArCOM::read<double>();
  template<> void ArCOM::write<double>(double data);
#endif
