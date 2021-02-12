
#####################
## Deps : protobuf ##
#####################

#fetch
find_package(Protobuf REQUIRED)

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
        Heartbeat.proto
)

#############
## Sources ##
#############

# define
add_library(appModels STATIC 
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
)
add_library(UnderStory::models ALIAS appModels)

# include
target_include_directories(appModels 
    INTERFACE 
        ${CMAKE_BINARY_DIR}
)

# link
target_link_libraries(appModels INTERFACE protobuf::libprotobuf)