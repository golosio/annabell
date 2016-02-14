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

//////////////////////////////////////////////////////////////////////
// Class of the model's free parameters
//////////////////////////////////////////////////////////////////////

#ifndef ANNABELLPARAMSH
#define ANNABELLPARAMSH

#include <string>

class AnnabellParams {
 public:
  float seed;
  float CW_W;
  float InPhB_W;
  float WkPhB_W;
  float WkEqWG_W;
  float WkEqGoalWG_W;
  float GoalPhEqGoalWG_W;
  float OutPhB_W;
  int MaxDynamicBias_W;

  AnnabellParams();
  int LoadFromFile(std::string filename);
  int Display();
};

#endif // ANNABELLPARAMSH
