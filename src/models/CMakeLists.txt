
add_library(appModels STATIC)
add_library(UnderStory::models ALIAS appModels)

######################
## Deps : capnproto ##
######################

#fetch
find_package(CapnProto REQUIRED)

#link (for using libs)
target_link_libraries(appModels INTERFACE CapnProto::capnp)

#generate
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS 
    Asset.capnp
    Character.capnp
    Handshake.capnp
    Message.capnp
    SharedDocument.capnp
    User.capnp
    YoutubeVideo.capnp
)

target_include_directories(appModels PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#############
## Sources ##
#############

target_sources(appModels PRIVATE ${CAPNP_SRCS})