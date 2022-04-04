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

#include <Arduino.h>
#include "ArCOM.h"

ArCOM::ArCOM(Stream &s) {
  ArCOMstream = &s;  // Sets the interface (Serial, Serial1, SerialUSB, etc.)
}

unsigned int ArCOM::available() {
  return ArCOMstream->available();
}

void ArCOM::flush() {
  ArCOMstream->flush();
}

void ArCOM::writeByteArray(byte numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    ArCOMstream->write(numArray[i]);
  }
}
void ArCOM::writeUint8Array(byte numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    ArCOMstream->write(numArray[i]);
  }
}
void ArCOM::writeCharArray(char charArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    ArCOMstream->write(charArray[i]);
  }
}
void ArCOM::writeInt8Array(int8_t numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    typeBuffer.int8 = numArray[i];
    ArCOMstream->write(typeBuffer.byteArray[0]);
  }
}
void ArCOM::writeUint16Array(unsigned short numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    typeBuffer.uint16 = numArray[i];
    ArCOMstream->write(typeBuffer.byteArray[0]);
    ArCOMstream->write(typeBuffer.byteArray[1]);
  }
}
void ArCOM::writeInt16Array(int16_t numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    typeBuffer.int16 = numArray[i];
    ArCOMstream->write(typeBuffer.byteArray[0]);
    ArCOMstream->write(typeBuffer.byteArray[1]);
  }
}
void ArCOM::writeUint32Array(unsigned long numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    typeBuffer.uint32 = numArray[i];
    ArCOMstream->write(typeBuffer.byteArray[0]);
    ArCOMstream->write(typeBuffer.byteArray[1]);
    ArCOMstream->write(typeBuffer.byteArray[2]);
    ArCOMstream->write(typeBuffer.byteArray[3]);
  }
}
void ArCOM::writeInt32Array(long numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    typeBuffer.int32 = numArray[i];
    ArCOMstream->write(typeBuffer.byteArray[0]);
    ArCOMstream->write(typeBuffer.byteArray[1]);
    ArCOMstream->write(typeBuffer.byteArray[2]);
    ArCOMstream->write(typeBuffer.byteArray[3]);
  }
}
void ArCOM::readByteArray(byte numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    numArray[i] = ArCOMstream->read();
  }
}
void ArCOM::readUint8Array(byte numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    numArray[i] = ArCOMstream->read();
  }
}
void ArCOM::readCharArray(char charArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    charArray[i] = ArCOMstream->read();
  }
}
void ArCOM::readInt8Array(int8_t numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[0] = ArCOMstream->read();
    numArray[i] = typeBuffer.int8;
  }
}
void ArCOM::readUint16Array(unsigned short numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[0] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[1] = ArCOMstream->read();
    numArray[i] = typeBuffer.uint16;
  }
}
void ArCOM::readInt16Array(short numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[0] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[1] = ArCOMstream->read();
    numArray[i] = typeBuffer.int16;
  }
}
void ArCOM::readUint32Array(unsigned long numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[0] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[1] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[2] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[3] = ArCOMstream->read();
    numArray[i] = typeBuffer.uint32;
  }
}
void ArCOM::readInt32Array(long numArray[], unsigned int nValues) {
  for (unsigned int i = 0; i < nValues; i++) {
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[0] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[1] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[2] = ArCOMstream->read();
    while (ArCOMstream->available() == 0) {}
    typeBuffer.byteArray[3] = ArCOMstream->read();
    numArray[i] = typeBuffer.int32;
  }
}
