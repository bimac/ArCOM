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

bool ArCOM::available() {
  return ArCOMstream->available();
}

void ArCOM::flush() {
  ArCOMstream->flush();
}

// Template specializations for boards with 4-byte "doubles"
#if __SIZEOF_DOUBLE__ == 4
  #include <IEEE754tools.h>

  template<> double ArCOM::read<double>() {
    uint8_t tmp[8] = {};
    readUint8Array(tmp,8);
    float data = doublePacked2Float(tmp, LSBFIRST);
    return data;
  }

  template<> void ArCOM::write<double>(double data) {
    uint8_t tmp[8] = {};
    float2DoublePacked((float) data, tmp, LSBFIRST);
    writeUint8Array(tmp,8);
  }
#endif
