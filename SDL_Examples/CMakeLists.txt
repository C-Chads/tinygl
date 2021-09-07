find_package(SDL QUIET)
find_package(OpenMP QUIET)

if(SDL_FOUND)

  if(TARGET tinygl)
    set(TINYGL_LIB tinygl)
  elseif(TARGET tinygl-static)
    set(TINYGL_LIB tinygl-static)
  endif()

  if (TINYGL_LIB)

    set(demo_names game gears helloworld menu model texture)
    foreach(DEMO ${demo_names})
      set(DEMO_NAME "sdl_${DEMO}")
      add_executable(${DEMO_NAME} ${DEMO}.c)
      target_link_libraries(${DEMO_NAME} PRIVATE ${TINYGL_LIB} ${M_LIBRARY} ${SDL_LIBRARY})
      target_include_directories(${DEMO_NAME} PUBLIC ${SDL_INCLUDE_DIR})
      if(NOT MSVC)
        target_compile_options(${DEMO_NAME} PRIVATE -O3 -DNDEBUG -pedantic -Wall)
        if(NOT CMAKE_CROSSCOMPILING)
          target_compile_options(tinygl PRIVATE -march=native)
        endif(NOT CMAKE_CROSSCOMPILING)
      endif(NOT MSVC)
      if(OPENMP_C_FOUND)
        target_link_libraries(${DEMO_NAME} PRIVATE OpenMP::OpenMP_C)
      endif(OPENMP_C_FOUND)
    endforeach()

    set(TINYGL_RESOURCES
      Arnepalette.png
      WWGW.mp3
      extrude.obj
      monkey3.obj
      tex.jpg
      tex_hole.png
      tex_old.jpg
      texture.png
      )
    foreach(FILE ${TINYGL_RESOURCES})
      get_filename_component(FILE_NAME ${FILE} NAME)
      configure_file(${FILE} ${FILE_NAME} COPYONLY)
    endforeach()

  endif (TINYGL_LIB)

else(SDL_FOUND)

  message(STATUS "tinygl: SDL not found")

endif(SDL_FOUND)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

