add_library(${PROJECT_NAME} 
    utils/logging.c
    utils/time_funcs.c
    lib.c
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Didn't make libcap work sadly :/
# find_library(CAP_LIBRARY NAMES cap libcap capability REQUIRED)
