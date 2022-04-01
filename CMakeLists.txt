cmake_minimum_required(VERSION 3.2)

set(PATCH_VERSION "1" CACHE INTERNAL "Patch version")
set(PROJECT_VESRION 0.0.${PATCH_VERSION})
set(MY_PROJECT_NAME bayan)
project(${MY_PROJECT_NAME} VERSION ${PROJECT_VESRION})

option(WITH_GOOGLE_TEST "Whether to build Google tets" ON)


add_executable(${MY_PROJECT_NAME} main.cpp )

set_target_properties(${MY_PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)



if(WITH_GOOGLE_TEST)
#git submodule add https://github.com/google/googletest.git
#git submodule update --init --recursive
    add_subdirectory(./googletest EXCLUDE_FROM_ALL)
    add_executable(google_test_version tests.cpp)

    set_target_properties(google_test_version PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    target_link_libraries(google_test_version 
        gtest_main 
        gmock
    )
endif()

if (MSVC)
    target_compile_options(${MY_PROJECT_NAME} PRIVATE
        /W4
    )
    if(WITH_GOOGLE_TEST)
        target_compile_options(google_test_version PRIVATE
            /W4
        )
    endif()
else ()
    target_compile_options(${MY_PROJECT_NAME} PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
    if(WITH_GOOGLE_TEST)
        target_compile_options(google_test_version PRIVATE
        -Wall -Wextra -pedantic -Werror
        )
    endif()
endif()

install(TARGETS ${MY_PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT r.zhigalenko@mail.ru)
include(CPack)

if(WITH_GOOGLE_TEST)
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests(google_test_version)
endif()