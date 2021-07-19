if(TARGET tinygl)
  set(TINYGL_LIB tinygl)
elseif(TARGET tinygl-static)
  set(TINYGL_LIB tinygl-static)
endif(TARGET tinygl)

if(TINYGL_LIB)

  set(raw_names gears t2i bigfont)
  foreach(DEMO ${demo_names})
    set(DEMO_NAME "raw_${DEMO}")
    add_executable(${DEMO_NAME} ${DEMO}.c)
    target_link_libraries(${DEMO_NAME} ${TINYGL_LIB} ${M_LIBRARY})
    target_include_directories(${DEMO_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  endforeach()

  # Copy utility files to build dir
  configure_file(asciifractal.sh ${CMAKE_CURRENT_BINARY_DIR}/asciifractal.sh)
  configure_file(char.txt ${CMAKE_CURRENT_BINARY_DIR}/char.txt)

endif(TINYGL_LIB)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

