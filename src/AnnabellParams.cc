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
// Methods of the class AnnabellParams.h
// which contains the model's free parameters
//////////////////////////////////////////////////////////////////////

#include "AnnabellParams.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

bool string_to_int(string str, int &int_val);
bool string_to_float(string str, float &f_val);

AnnabellParams::AnnabellParams()
{
  // default values for free parameters
  seed = 12345;
  CW_W = 1;
  InPhB_W = 3;
  WkPhB_W = 1;
  WkEqWG_W = 3;
  WkEqGoalWG_W = 5;
  GoalPhEqGoalWG_W = 1;
  OutPhB_W = 1;
  MaxDynamicBias_W = 500;

}

int AnnabellParams::LoadFromFile(std::string filename)
{
  //load free parameters from file
  ifstream fs(filename.c_str());
  if (!fs) {
    cerr << "Parameters file not found.\n";
    return 1;
  }

  vector<string> input_token;
  string input_line, buf;
  while (getline(fs, input_line)) {
    stringstream ss(input_line); // Insert the line into a stream
    input_token.clear();
    while (ss >> buf) { // split line in string tokens
      input_token.push_back(buf);
      //cout << buf << " ";
    }
    //cout << endl;
    if (input_token.size()==0) continue; // empty line
    buf=input_token[0];
    if (buf[0]=='#') continue; // comment
    else if (input_token.size()==2) { // line with two tokens
      // set values for free parameters
      if (buf=="seed") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	seed = f_val;
      }
      else if (buf=="CW") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	CW_W = f_val;
      }
      else if (buf=="InPhB") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	InPhB_W = f_val;
      }
      else if (buf=="WkPhB") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	WkPhB_W = f_val;
      }
      else if (buf=="WkEqWG") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	WkEqWG_W = f_val;
      }
      else if (buf=="WkEqGoalWG") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	WkEqGoalWG_W = f_val;
      }
      else if (buf=="GoalPhEqGoalWG") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	GoalPhEqGoalWG_W = f_val;
      }
      else if (buf=="OutPhB") {
	float f_val; // convert second token to float
	if (!string_to_float(input_token[1], f_val)) return 1;
	OutPhB_W = f_val;
      }
      else if (buf=="MaxDynamicBias") {
	int int_val; // convert second token to int
	if (!string_to_int(input_token[1], int_val)) return 1;
	MaxDynamicBias_W = int_val;
      }
      else {
	cerr << "Unrecognized parameter in parameter file.\n";
	return 1;
      }
    }
    else {
      cerr << "Syntax error in parameter file.\n";
      return 1;
    }
  }

  return 0;
}

int AnnabellParams::Display()
{
  // display values of free parameters
  cout << "Free parameters:\n";
  cout << "seed: " << seed << endl;
  cout << "CW_W: " << CW_W << endl;
  cout << "InPhB_W: " << InPhB_W << endl;
  cout << "WkPhB_W: " << WkPhB_W << endl;
  cout << "WkEqWG_W: " << WkEqWG_W << endl;
  cout << "WkEqGoalWG_W: " << WkEqGoalWG_W << endl;
  cout << "GoalPhEqGoalWG_W: " << GoalPhEqGoalWG_W << endl;
  cout << "OutPhB_W: " << OutPhB_W << endl;
  cout << "MaxDynamicBias_W: " << MaxDynamicBias_W << endl;

  return 0;
}

bool string_to_int(string str, int &int_val)
{
  stringstream ss(str);
  
  ss >> int_val;
  if (!ss) {
    cerr << "Cannot convert token to integer in parameter file.\n";
    return false;
  }
  return true;
}

bool string_to_float(string str, float &f_val)
{
  stringstream ss(str);
  
  ss >> f_val;
  if (!ss) {
    cerr << "Cannot convert token to float in parameter file.\n";
    return false;
  }
  return true;
}
