
#define executable
add_executable(tests tests.cpp)
add_executable(UnderStory::tests ALIAS tests)
list(APPEND TARGETS_TO_TEST 
    tests
)

#determine local tests
if(LOCAL_TESTING)
    #define executable
    add_executable(localTests local_only.cpp)
    add_executable(UnderStory::localTests ALIAS localTests)

    list(APPEND TARGETS_TO_TEST 
        localTests
    )
endif()

###################
## Deps : Catch2 ##
###################

#fetch
FetchContent_Declare(Catch2
    GIT_REPOSITORY  "https://github.com/catchorg/Catch2.git"
)
FetchContent_MakeAvailable(Catch2)

#link
foreach(TEST_TARGET IN LISTS TARGETS_TO_TEST)
    target_include_directories(${TEST_TARGET} PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    )
    target_link_libraries(${TEST_TARGET} PRIVATE
        UnderStory::network
        Catch2::Catch2
    )
endforeach()

####################
## tests handling ##
####################

#declare use of CTest and add Catch2 implementation
include(CTest)
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/contrib)

include(Catch)
foreach(TEST_TARGET IN LISTS TARGETS_TO_TEST)
    catch_discover_tests(${TEST_TARGET})
endforeach()