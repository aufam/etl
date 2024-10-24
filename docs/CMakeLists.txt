find_package(Doxygen REQUIRED)

configure_file(Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

add_custom_target(docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

install(DIRECTORY ${CMAKE_BINARY_DIR}/docs/html
    DESTINATION /usr/share/doc/etl
    OPTIONAL
)
