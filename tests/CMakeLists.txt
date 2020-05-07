#define executable
add_executable(tests tests.cpp)
add_executable(UnderStory::tests ALIAS tests)
target_link_libraries(tests PRIVATE
    UnderStory::core
    UnderStory::network
)
list(APPEND TARGETS_TO_TEST 
    tests
)

#determine local tests
if(LOCAL_TESTING)
    #define executable
    add_executable(localTests local_only.cpp)
    add_executable(UnderStory::localTests ALIAS localTests)
    target_link_libraries(localTests PRIVATE
        UnderStory::network
    )
    list(APPEND TARGETS_TO_TEST 
        localTests
    )
endif()

###################
## Deps : Catch2 ##
###################

#fetch
find_package(Catch2 REQUIRED)

#link
foreach(TEST_TARGET IN LISTS TARGETS_TO_TEST)
    target_link_libraries(${TEST_TARGET} PRIVATE
        Catch2::Catch2
        spdlog
    )
endforeach()

####################
## tests handling ##
####################

#declare use of CTest and add Catch2 implementation
include(CTest)
include(Catch)

#for each test targets
foreach(TEST_TARGET IN LISTS TARGETS_TO_TEST)
    catch_discover_tests(${TEST_TARGET})
endforeach()