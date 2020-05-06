
add_library(appModels STATIC)
add_library(UnderStory::models ALIAS appModels)

######################
## Deps : capnproto ##
######################

#fetch
find_package(Protobuf REQUIRED)

#link (for using libs)
target_link_libraries(appModels INTERFACE protobuf::libprotobuf)

#generate
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    File.proto
    Asset.proto
    # Character.proto
    # Handshake.proto
    # Message.proto
    # SharedDocument.proto
    # User.proto
    # YoutubeVideo.proto
)

target_include_directories(appModels PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#############
## Sources ##
#############

target_sources(appModels PRIVATE ${PROTO_SRCS} ${PROTO_HDRS})