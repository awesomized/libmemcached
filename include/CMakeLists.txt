
add_subdirectory(libhashkit-1.0)
set(libhashkit_includes "${libhashkit_includes}" PARENT_SCOPE)
add_subdirectory(libmemcached-1.0)
set(libmemcached_includes "${libmemcached_includes}" PARENT_SCOPE)
add_subdirectory(libmemcachedutil-1.0)
set(libmemcachedutil_includes "${libmemcachedutil_includes}" PARENT_SCOPE)
add_subdirectory(libmemcachedprotocol-0.0)
set(libmemcachedprotocol_includes "${libmemcachedprotocol_includes}" PARENT_SCOPE)
