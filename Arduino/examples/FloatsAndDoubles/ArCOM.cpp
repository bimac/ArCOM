/*
ArCOM
Copyright (C) 2016 Sanworks LLC, Sound Beach, New York, USA
Copyright (C) 2022 Florian Rau
Copyright (C) 2026 International Brain Laboratory

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

unsigned int ArCOM::available() {
  return ArCOMstream->available();
}

void ArCOM::flush() {
  ArCOMstream->flush();
}

// Template specializations for boards with 4-byte "doubles". The platforms
// concerned need to be listed with the IEEE754tools dependency in library.json
#if __SIZEOF_DOUBLE__ != 8
  #include <IEEE754tools.h>

  static double readPackedDouble(ArCOM &com) {
    uint8_t tmp[8];
    com.readUint8Array(tmp,8);
    return doublePacked2Float(tmp, LSBFIRST);
  }

  static void writePackedDouble(ArCOM &com, double data) {
    uint8_t tmp[8] = {};
    float2DoublePacked((float) data, tmp, LSBFIRST);
    com.writeUint8Array(tmp,8);
  }

  template<> double ArCOM::read<double>() {
    return readPackedDouble(*this);
  }

  template<> void ArCOM::read<double>(double *data, size_t nValues) {
    for (size_t i=0; i<nValues; i++)
      data[i] = readPackedDouble(*this);
  }

  template<> void ArCOM::write<double>(double data) {
    writePackedDouble(*this, data);
  }

  template<> void ArCOM::write<double>(const double *data, size_t nValues) {
    for (size_t i=0; i<nValues; i++)
      writePackedDouble(*this, data[i]);
  }
#endif
