
add_library(appModels STATIC)
add_library(UnderStory::models ALIAS appModels)

######################
## Deps : capnproto ##
######################

#fetch
find_package(CapnProto REQUIRED)

capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS 
    Asset.capnp
)

target_include_directories(appModels PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#############
## Sources ##
#############

target_sources(appModels PRIVATE ${CAPNP_SRCS})