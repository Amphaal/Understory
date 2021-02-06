
add_library(appModels INTERFACE)
add_library(UnderStory::models ALIAS appModels)

#####################
## Deps : protobuf ##
#####################

#fetch
find_package(Protobuf REQUIRED)

#link (for using libs)
target_link_libraries(appModels INTERFACE protobuf::libprotobuf)

#generate
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    PROTOS
        File.proto
        Asset.proto
        Character.proto
        Message.proto
        User.proto
        YoutubeVideo.proto
        Base.proto
        Map.proto
)

#############
## Sources ##
#############

target_include_directories(appModels INTERFACE ${CMAKE_BINARY_DIR})

target_sources(appModels 
    INTERFACE ${PROTO_SRCS} ${PROTO_HDRS}
)