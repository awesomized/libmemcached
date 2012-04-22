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



#pragma once

/**
  Framework is the structure which is passed to the test implementation to be filled.
  This must be implemented in order for the test framework to load the tests. We call
  get_world() in order to fill this structure.
*/

class Framework {
public:
  collection_st *collections;

  /* These methods are called outside of any collection call. */
  test_callback_create_fn *_create;
  test_callback_destroy_fn *_destroy;

public:
  void* create(test_return_t& arg);

  /**
    If an error occurs during the test, this is called.
  */
  test_callback_error_fn *_on_error;

  void set_on_error(test_callback_error_fn *arg)
  {
    _on_error= arg;
  }

  test_return_t on_error(const enum test_return_t, void *);

  void set_socket()
  {
    _servers.set_socket();
  }

  void set_sasl(const std::string& username_arg, const std::string& password_arg)
  {
    _servers.set_sasl(username_arg, password_arg);
  }

  libtest::server_startup_st& servers()
  {
    return _servers;
  }
  
  /**
    Runner represents the callers for the tests. If not implemented we will use
    a set of default implementations.
  */
  libtest::Runner *_runner;

  void set_runner(libtest::Runner *arg)
  {
    _runner= arg;
  }

  libtest::Runner *runner();


  Framework();

  virtual ~Framework();

  Framework(const Framework&);

private:
  Framework& operator=(const Framework&);
  libtest::server_startup_st _servers;
  bool _socket;
  void *_creators_ptr;
  unsigned long int _servers_to_run;
};
