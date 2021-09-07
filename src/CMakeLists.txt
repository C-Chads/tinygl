include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(tinygl_srcs
  accum.c
  api.c
  arrays.c
  clear.c
  clip.c
  get.c
  image_util.c
  init.c
  light.c
  list.c
  matrix.c
  memory.c
  misc.c
  msghandling.c
  select.c
  specbuf.c
  texture.c
  vertex.c
  zbuffer.c
  zline.c
  zmath.c
  zpostprocess.c
  zraster.c
  ztext.c
  ztriangle.c
  )

find_package(OpenMP)

if(TINYGL_BUILD_SHARED)
  add_library(tinygl SHARED ${tinygl_srcs})
  target_include_directories(tinygl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  install(TARGETS tinygl
    RUNTIME DESTINATION ${BIN_DIR}
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR})
  if(NOT MSVC)
    target_compile_options(tinygl PRIVATE -O3 -DNDEBUG -pedantic -Wall -Wno-unused-function)
    if(NOT CMAKE_CROSSCOMPILING)
      target_compile_options(tinygl PRIVATE -march=native)
    endif(NOT CMAKE_CROSSCOMPILING)
  endif(NOT MSVC)
  if(OPENMP_C_FOUND)
    target_link_libraries(tinygl PUBLIC OpenMP::OpenMP_C)
  endif(OPENMP_C_FOUND)
endif(TINYGL_BUILD_SHARED)

if(TINYGL_BUILD_STATIC)
  add_library(tinygl-static STATIC ${tinygl_srcs})
  target_include_directories(tinygl-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  install(TARGETS tinygl-static
    RUNTIME DESTINATION ${BIN_DIR}
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR})
  if(NOT MSVC)
    target_compile_options(tinygl-static PRIVATE -O3 -DNDEBUG -pedantic -Wall -Wno-unused-function)
    if(NOT CMAKE_CROSSCOMPILING)
      target_compile_options(tinygl-static PRIVATE -march=native)
    endif(NOT CMAKE_CROSSCOMPILING)
  endif(NOT MSVC)
  if(OPENMP_C_FOUND)
    target_link_libraries(tinygl-static PUBLIC OpenMP::OpenMP_C)
  endif(OPENMP_C_FOUND)
endif(TINYGL_BUILD_STATIC)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

