set(TARGET glad)

set(SOURCES
    "src/glad.c"
)

set(HEADERS
    "include/glad/glad.h"
    "include/KHR/khrplatform.h"
)

add_library(${TARGET} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${TARGET} PUBLIC ./include)
