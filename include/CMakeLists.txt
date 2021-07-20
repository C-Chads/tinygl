set(zhdrs
	zbuffer.h
	zfeatures.h
	)

install(FILES ${zhdrs} DESTINATION ${INCLUDE_DIR})

install(FILES TTGL/gl.h DESTINATION ${INCLUDE_DIR}/TGL)
