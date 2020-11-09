# libmemcached

[![Gitter Chat](https://badges.gitter.im/m6w6/libmemcached.svg)](https://gitter.im/m6w6/libmemcached?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![BSD 3-Clause License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](https://opensource.org/licenses/BSD-3-Clause)

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

CI/Test results are available at:

| Provider       | Status                | OS               | Compiler   | Arch                  |
|----------------|-----------------------|------------------|------------|-----------------------|
| [Travis CI]    | ![Travis CI Badge]    | Linux            | GNU        | arm64, ppc64le, s390x |
| [Builds.sr.ht] | ![Builds.sr.ht Badge] | FreeBSD, OpenBSD | Clang      | amd64                 |
| [GH Actions]   | ![GH Actions Badge]   | Linux, MacOS     | GNU, Clang | amd64                 |

Additional project metrics:

| Provider  | Type          | Status           |
|-----------|---------------|------------------|
| [Codecov] | Code coverage | ![Codecov Badge] |


## ChangeLog

Check out the latest [releases](https://github.com/m6w6/libmemcached/releases)
or the bundled [ChangeLog](./ChangeLog.md) for a comprehensive list of changes.

## License

libmemcached is licensed under the 3-Clause-BSD license, which can be
found in the accompanying [LICENSE](./LICENSE) file.

## Contributing

Please report any issues on the [bug tracker](https://github.com/m6w6/libmemcached/issues).

A list of known permanent issues is maintained in [BUGS](./BUGS.md).

All forms of contribution are welcome! Please see the bundled
[CONTRIBUTING](./CONTRIBUTING.md) note for the general principles followed.

The list of current and past maintainers and contributors is available in [AUTHORS](./AUTHORS).

### Building and updating docs

See [gh-pages/publish](./docs/gh-pages/publish.sh) script and the 
[docs-publish-pages](./.github/workflows/docs-publish-pages.yml) workflow, 
which automate pushing updated documentation to github pages.

[Travis CI]:            https://travis-ci.org/github/m6w6/libmemcached
[Travis CI Badge]:      https://api.travis-ci.org/m6w6/libmemcached.svg?branch=v1.x
[Cirrus CI]:            https://cirrus-ci.com/github/m6w6/libmemcached
[Cirrus CI Badge]:      https://api.cirrus-ci.com/github/m6w6/libmemcached.svg?branch=v1.x
[GH Actions]:           https://github.com/m6w6/libmemcached/actions?query=workflow%3Acmake-build-ci
[GH Actions Badge]:     https://github.com/m6w6/libmemcached/workflows/cmake-build-ci/badge.svg?branch=v1.x
[Builds.sr.ht]:         https://builds.sr.ht/~m6w6/libmemcached
[Builds.sr.ht Badge]:   https://builds.sr.ht/~m6w6/libmemcached/commits.svg

[Codecov]:          https://codecov.io/gh/m6w6/libmemcached
[Codecov Badge]:    https://codecov.io/gh/m6w6/libmemcached/branch/v1.x/graph/badge.svg
