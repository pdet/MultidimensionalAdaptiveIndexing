#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] ) {
  // global setup...
  system("mkdir -p test_data");

  int result = Catch::Session().run( argc, argv );

  // global clean-up...
  system("rm -rf test_data");

  return result;
}

