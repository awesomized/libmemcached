#define CATCH_CONFIG_RUNNER
#include "lib/catch.hpp"
#include "setup.hpp"

int main(int argc, char *argv[]) {
  setup(argc, &argv);
  return Catch::Session().run(argc, argv);
}
