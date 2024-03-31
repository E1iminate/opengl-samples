set(TARGET opengl-sample)

set(SOURCES
    main.cxx
)

set(HEADERS
)


add_executable(${TARGET} ${SOURCES} ${HEADERS})

target_link_libraries(${TARGET} glfw glad)
