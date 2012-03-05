/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2012 Data Differential, http://datadifferential.com/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <libtest/common.h>

#include <unistd.h>

namespace libtest {

size_t number_of_cpus()
{
  size_t number_of_cpu= 1;
#ifdef TARGET_OS_LINUX
  number_of_cpu= sysconf(_SC_NPROCESSORS_ONLN);
#elif TARGET_OS_OSX || TARGET_OS_FREEBSD
  int mib[4];
  size_t len= sizeof(number_of_cpu); 

  /* set the mib for hw.ncpu */
  mib[0] = CTL_HW;
  mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

  /* get the number of CPUs from the system */
  sysctl(mib, 2, &number_of_cpu, &len, NULL, 0);

  if (number_of_cpu < 1) 
  {
    mib[1]= HW_NCPU;
    sysctl(mib, 2, &number_of_cpu, &len, NULL, 0 );

    if (number_of_cpu < 1 )
    {
      number_of_cpu = 1;
    }
  }
#endif

  return number_of_cpu;
}

} // namespace libtest
