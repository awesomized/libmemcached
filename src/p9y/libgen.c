#include "libgen.hpp"

#if defined _WIN32
char *basename(const char *filename) {
  static char base[_MAX_PATH * 2], ext[_MAX_PATH], *ptr;
  (void) _splitpath_s(filename, NULL, 0, NULL, 0, base, _MAX_PATH, ext, _MAX_PATH);
  strcat_s(base, _MAX_PATH * 2 - 1, ext);
  return base;
}
#endif
