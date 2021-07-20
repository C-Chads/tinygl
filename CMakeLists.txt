cmake_minimum_required(VERSION 3.12)
project(tinygl
  DESCRIPTION "tinygl: The ultimate portable graphics library"
  HOMEPAGE_URL "https://github.com/C-Chads/tinygl"
  LANGUAGES C
  )

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Set relative output directory paths, if not already defined
if (NOT BIN_DIR)
  set(BIN_DIR bin)
endif (NOT BIN_DIR)
if (NOT INCLUDE_DIR)
  set(INCLUDE_DIR include)
endif (NOT INCLUDE_DIR)
if (NOT DEFINED LIB_DIR)
  set(LIB_DIR lib)
endif (NOT DEFINED LIB_DIR)

# Check for a math library
include(CheckLibraryExists)
check_library_exists(m cos "" HAVE_M_LIBRARY)
if (HAVE_M_LIBRARY)
  set(M_LIBRARY m)
endif (HAVE_M_LIBRARY)

# Options
option(TINYGL_BUILD_EXAMPLES "Build Examples" OFF)
option(TINYGL_BUILD_SHARED "Build Shared Library" ON)
option(TINYGL_BUILD_STATIC "Build Static Library" ON)

# Build main library
add_subdirectory(src)

# Install logic for headers
add_subdirectory(include)

# Examples
if(TINYGL_BUILD_EXAMPLES)

  enable_testing()

  # These deliberately do not depend on SDL.  TODO - these could be used to
  # drive a "make test" system to check the correct functioning of the library
  # after compilation...
  add_subdirectory(Raw_Demos)

  # Examples that use SDL for interactive display
  add_subdirectory(SDL_Examples)

endif()

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

