/*
Copyright (C) 2016 Bruno Golosio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SENSORYMOTORH
#define SENSORYMOTORH

#include <sstream>
#include <vector>
#include <string>

#include "display.h"

int SensoryMotor(std::vector <std::string> phrase_token, std::stringstream &ss,
		 display* Display);
int YarpNotSupported(display* Display);
int YarpInterface(std::vector <std::string> phrase_token, std::stringstream &ss,
		 display* Display);
int YarpInput(display* Display);
int YarpOutput(std::string out_phrase, display* Display);
bool YarpOutputInit(display* Display);
int YarpOutputClose(display* Display);
bool CheckYarp(display* Display);

#endif
