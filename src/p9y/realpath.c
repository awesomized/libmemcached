#include "realpath.hpp"

#if defined _WIN32
char *realpath(const char *path, char real[_MAX_PATH]) {
  return _fullpath(real, path, _MAX_PATH);
}
#endif
