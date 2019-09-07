add_library(jsox STATIC
    parser.c
)
target_include_directories(jsox PUBLIC ${PROJECT_SOURCE_DIR}/include)
install(TARGETS jsox ARCHIVE DESTINATION lib)

add_executable(demo demo.c)
target_link_libraries(demo PRIVATE jsox)
