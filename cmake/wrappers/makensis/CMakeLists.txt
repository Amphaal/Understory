project(makensis-wrapper
    DESCRIPTION "wrapper for makensis execution on X-Compilation environement"
    LANGUAGES CXX
)

add_executable(makensis makensis.cpp)

install(TARGETS makensis RUNTIME)