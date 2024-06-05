add_executable(test_pthread test_pthread.c)
target_link_libraries(test_pthread controloop cmocka)
target_include_directories(test_pthread PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_test(NAME test_pthread COMMAND test_pthread)
