# libmemcached

[![Build Status](https://travis-ci.org/m6w6/libmemcached.svg?branch=v1.0)](https://travis-ci.org/m6w6/libmemcached) [![Join the chat at https://gitter.im/m6w6/libmemcached](https://badges.gitter.im/m6w6/libmemcached.svg)](https://gitter.im/m6w6/libmemcached?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

libmemcached is an open source C/C++ client library and tools for the
memcached server (http://memcached.org/). It has been designed to be
light on memory usage, thread safe, and provide full access to server
side methods.

> **NOTE:**  
> This is a resurrection of the original work from Brian Aker at
> [libmemcached.org](https://libmemcached.org) and the only publicly maintained
> version of libmemcached currently known to me.

## Documentation

See https://m6w6.github.io/libmemcached

## Installing

libmemcached uses `CMake`. Please see/edit `CMakeConfig.txt` or use
`ccmake(1)` to set any preferred options.

### From source

	git clone github.com:m6w6/libmemcached
	mkdir build-libmemcached
	cd $_
	cmake ../libmemcached
	make
	sudo make install

## Testing

Enable the `BUILD_TESTING` setting for a build and run `make test`.

    cmake -DBUILD_TESTING=ON ../libmemcached
    make test

### Continuous integration

CI/Test results are available at [Travis CI](https://travis-ci.org/m6w6/libmemcached).
 
## ChangeLog

Check out the latest [releases](./releases) or the bundled
[ChangeLog](./ChangeLog) for a comprehensive list of changes.

## License

libmemcached is licensed under the 3-Clause-BSD license, which can be
found in the accompanying [LICENSE](./LICENSE) file.

## Contributing

All forms of contribution are welcome! Please see the bundled
[CONTRIBUTING](./CONTRIBUTING.md) note for the general principles followed.

The list of past and current contributors is maintained in [THANKS](./THANKS).

The list of current maintainers is available in [AUTHORS](./AUTHORS).

### Building and updating docs

See [gh-pages/publish](./docs/gh-pages/publish.sh) script, which automates 
pushing updated documentation to github pages.

