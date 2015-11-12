/*
Copyright (C) 2015 Bruno Golosio
Copyright (C) 2015 Alfio E. Fresta

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

#include <sys/time.h>
#include "gettime.h"

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

void GetRealTime(struct timespec* clk)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  // src. http://stackoverflow.com/a/6725161 
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  clk->tv_sec = mts.tv_sec;
  clk->tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_REALTIME, &clk);
#endif
}

void GetMonotonicTime(struct timespec* clk)
{
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
  // src. http://stackoverflow.com/a/6725161 
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  clk->tv_sec = mts.tv_sec;
  clk->tv_nsec = mts.tv_nsec;
#else
  clock_gettime(CLOCK_MONOTONIC, &clk);
#endif
}
