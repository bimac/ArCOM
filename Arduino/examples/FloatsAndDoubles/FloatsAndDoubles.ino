/*
ArCOM
Copyright (C) 2016 Sanworks LLC, Sound Beach, New York, USA
Copyright (C) 2022 Florian Rau

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, version 3.

This program is distributed  WITHOUT ANY WARRANTY and without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ArCOM.h"
ArCOM myUSB(SerialUSB); // Sets ArCOM to wrap SerialUSB (could also be Serial, Serial1, etc.)

char charBuffer[18];

void setup()
{
  SerialUSB.begin(9600);
}

void loop()
{
  if (myUSB.available())
  {
    switch (myUSB.readUint8())
    {
    case 0:
      myUSB.writeFloat(3.14159265358979311600L);
      break;
    case 1:
      myUSB.writeDouble(3.14159265358979311600L);
      break;
    case 2:
      sprintf(charBuffer, "%1.15Lf", 3.14159265358979311600L);
      myUSB.writeCharArray(charBuffer, 18);
      break;
    case 3:
      myUSB.writeDouble(myUSB.readDouble());
      break;
    case 4:
      sprintf(charBuffer, "%1.15f", myUSB.readDouble());
      myUSB.writeCharArray(charBuffer, 18);
      break;
    }
  }
}
