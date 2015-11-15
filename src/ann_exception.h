/*
Copyright (C) 2015 Bruno Golosio

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
///////////////////////////////////
//     ann_exception.h           //
//        15/11/2015             //
//   author : Bruno Golosio      //
///////////////////////////////////
// ann_exception class definition
// This class handles runtime errors
/////////////////////////////////////

#ifndef ANNEXCEPTIONH
#define ANNEXCEPTIONH
#include <string>
#include <exception>
using namespace std;

///////////////////////////////////
// ann_exception class definition
// in case of errors displays a message and stop the execution
//////////////////////////////////
class ann_exception: public exception
{
  const char *Message; // error message
  
 public:
  // constructors
  ann_exception(const char *ch)  {Message=ch;}
  ann_exception(string s)  {Message=s.c_str();}
  // throw method
  virtual const char* what() const throw()
  {
    return Message;
  }
};

#endif
