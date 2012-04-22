/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  libtest
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
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


#include <config.h>
#include <libtest/common.h>
#include <iostream>

using namespace libtest;

Framework::Framework() :
  collections(NULL),
  _create(NULL),
  _destroy(NULL),
  _runner(NULL),
  _socket(false),
  _creators_ptr(NULL)
{
}

Framework::~Framework()
{
  if (_destroy and _destroy(_creators_ptr))
  {
    Error << "Failure in _destroy(), some resources may not have been cleaned up.";
  }

  _servers.shutdown();

  delete _runner;
}

libtest::Runner *Framework::runner()
{
  if (_runner == NULL)
  {
    _runner= new Runner;
  }
  _runner->set_servers(_servers);

  return _runner;
}

void* Framework::create(test_return_t& arg)
{
  arg= TEST_SUCCESS;
  if (_create)
  {
    return _creators_ptr= _create(_servers, arg);
  }

  return NULL;
}
