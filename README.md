# libmemcached-awesome

[![License Badge]](https://opensource.org/licenses/BSD-3-Clause)

[License Badge]:    https://img.shields.io/badge/License-BSD%203--Clause-blue.svg "BSD 3-Clause"

libmemcached-awesome is an open source C/C++ client library and tools
for the memcached server (http://memcached.org/). It has been designed
to be light on memory usage, thread safe, and provide full access to
server side methods.

> **NOTE:**  
> This is a resurrection of the original work from Brian Aker at
> [libmemcached.org](https://libmemcached.org).

## Documentation

[![Docs Actions Badge]](
    https://github.com/awesomized/libmemcached/actions?query=workflow%3Adocs-publish-pages)

[Docs Actions Badge]:
    https://github.com/awesomized/libmemcached/workflows/docs-publish-pages/badge.svg?branch=v1.x
    "Github Docs Action"

See https://awesomized.github.io/libmemcached

### Building and updating docs

See [gh-pages/publish](./docs/gh-pages/publish.sh) script and the
[docs-publish-pages](./.github/workflows/docs-publish-pages.yml) workflow,
which automate pushing updated documentation to github pages.

## Installing

libmemcached-awesome uses `CMake`.
Please see/edit [`CMakeConfig.txt`](./CMakeConfig.txt) or use `ccmake(1)` or
`cmake-gui(1)` to set any preferred options.

### From source

    git clone github.com:awesomized/libmemcached
    mkdir build-libmemcached
    cd $_
    cmake ../libmemcached
    make
    sudo make install

#### Requirements

* CMake 3.9+
* C++11 compiler
* GNU Bison 2.3+ and Flex

##### Optional dependencies

* C++17 compiler (required for: tests)
* Intel's libtbb (optional for: tests; for GCC's stdlib parallelism support)
* pthreads (required for: tests, contrib/bin/memaslap, libmemcachedutil/pool)
* libevent (required for: contrib/bin/memaslap)
* Cyrus' libsasl2 (required for: libmemcached/sasl)

### Binaries

CI and release builds for Linux, a couple BSDs, MacOS and Windows are available at
https://artifacts.m6w6.name/libmemcached/ and rsync://m6w6.name::artifacts/libmemcached/.

## Testing

Enable the `BUILD_TESTING` setting for a build and run `make test`.

    cmake -DBUILD_TESTING=ON ../libmemcached
    make test

### Continuous integration

[![Actions Badge]](https://github.com/awesomized/libmemcached/actions?query=workflow%3Acmake-build-ci)
[![Sourcehut Badge]](https://builds.sr.ht/~m6w6/libmemcached)

[Actions Badge]:
    https://github.com/awesomized/libmemcached/workflows/cmake-build-ci/badge.svg?branch=v1.x
    "Github Actions"
[Sourcehut Badge]:
    https://builds.sr.ht/~m6w6/libmemcached/commits.svg
    "Sourcehut Builds"

CI/Testing is performed on the following system matrix:

| OS               | Compiler                     | Arch                    | Comments                   |
|------------------|------------------------------|-------------------------|----------------------------|
| Linux            | GNU 7/9/10, Clang 9/10/11/12 | amd64                   | sasl, coverage, sanitizers |
| MacOS            | AppleClang 12                | amd64                   | sasl, coverage             |
| FreeBSD          | Clang 11                     | amd64                   | sasl, coverage             |
| OpenBSD          | Clang 10                     | amd64                   | sasl, coverage             |
| Windows          | MSVC 16, MinGW               | amd64                   | no sasl, no tests          |
| Solaris          | SunPro 12.5                  | amd64                   | no sasl, no tests, manually|

libmemcached-awesome has been tested against [memcached](https://github.
com/memcached/memcached) v1.5 and v1.6.

## ChangeLog

Check out the latest [releases](https://github.com/awesomized/libmemcached/releases)
or the bundled [ChangeLog](./ChangeLog-1.1.md) for a comprehensive list of changes.

## License

libmemcached-awesome is licensed under the 3-Clause-BSD license, which 
can be
found in the accompanying [LICENSE](./LICENSE) file.

## Contributing

Please report any issues on the [bug tracker](https://github.com/awesomized/libmemcached/issues).

A list of known permanent issues is maintained in [BUGS](./BUGS.md).

All forms of contribution are welcome! Please see the bundled
[CONTRIBUTING](./CONTRIBUTING.md) note for the general principles followed.

The list of current and past maintainers and contributors is available in [AUTHORS](./AUTHORS).

