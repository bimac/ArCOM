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
#define ALIAS(alias, target) \
template<typename... Args> \
inline auto alias(Args&&... args) -> decltype(target(std::forward<Args>(args)...)) \
{ \
  return target(std::forward<Args>(args)...); \
}


class ArCOM
{
protected:


public:


  // Constructor
  ArCOM(Stream &s);
  // Serial functions
  unsigned int available();
  void flush();


  // Unsigned integers
  void writeByteArray(byte numArray[], unsigned int size);
  void writeUint8Array(byte numArray[], unsigned int size);
  void writeCharArray(char charArray[], unsigned int size);
  void writeUint16Array(unsigned short numArray[], unsigned int size);
  void writeUint32Array(unsigned long numArray[], unsigned int size);
  void readByteArray(byte numArray[], unsigned int size);
  void readUint8Array(byte numArray[], unsigned int size);
  void readCharArray(char charArray[], unsigned int size);
  void readUint16Array(unsigned short numArray[], unsigned int size);
  void readUint32Array(unsigned long numArray[], unsigned int size);

  // Signed integers
  void writeInt8Array(int8_t numArray[], unsigned int size);
  void writeInt16Array(int16_t numArray[], unsigned int size);
  void writeInt32Array(int32_t numArray[], unsigned int size);
  void readInt8Array(int8_t numArray[], unsigned int size);
  void readInt16Array(int16_t numArray[], unsigned int size);
  void readInt32Array(int32_t numArray[], unsigned int size);

  template<typename T>
  void write(T data) {
    buffer <T>buf;
    buf.data = data;
    for (size_t i = 0; i<sizeof(T); i++)
      Serial.write(buf.bytes[i]);
  }

  // template<typename T>
  // void write(T data, uint8_t n) {
  //   buffer <T>buf;
  //   buf.data = data;
  //   for (size_t i = 0; i<sizeof(T); i++)
  //     Serial.write(buf.bytes[i]);
  // }

  template<typename T>
  T read() {
    buffer <T>buf;
    for (size_t i = 0; i<sizeof(T); i++) {
      while (ArCOMstream->available() == 0) {}
      buf.bytes[i] = ArCOMstream->read();
    }
    return buf.data;
  }

  // Generate wrapper functions for backwards-compatibility
  ALIAS(readByte,    read<uint8_t>)
  ALIAS(readUint8,   read<uint8_t>)
  ALIAS(readUint16,  read<uint16_t>)
  ALIAS(readUint32,  read<uint32_t>)
  ALIAS(readUint64,  read<uint64_t>)
  ALIAS(readChar,    read<signed char>)
  ALIAS(readInt8,    read<int8_t>)
  ALIAS(readInt16,   read<int16_t>)
  ALIAS(readInt32,   read<int32_t>)
  ALIAS(readInt64,   read<int64_t>)
  ALIAS(readFloat,   read<float>)
  ALIAS(readDouble,  read<double>)
  ALIAS(writeByte,   write<uint8_t>)
  ALIAS(writeUint8,  write<uint8_t>)
  ALIAS(writeUint16, write<uint16_t>)
  ALIAS(writeUint32, write<uint32_t>)
  ALIAS(writeUint64, write<uint64_t>)
  ALIAS(writeChar,   write<signed char>)
  ALIAS(writeInt8,   write<int8_t>)
  ALIAS(writeInt16,  write<int16_t>)
  ALIAS(writeInt32,  write<int32_t>)
  ALIAS(writeInt64,  write<int64_t>)
  ALIAS(writeFloat,  write<float>)
  ALIAS(writeDouble, write<double>)

private:
  template<typename T>
  union buffer {
    T data;
    uint8_t bytes[sizeof(T)];
  };

  Stream *ArCOMstream; // Stores the interface (Serial, Serial1, SerialUSB, etc.)
  union {
    byte byteArray[4];
    uint16_t uint16;
    uint32_t uint32;
    int8_t int8;
    int16_t int16;
    int32_t int32;
} typeBuffer;

};
