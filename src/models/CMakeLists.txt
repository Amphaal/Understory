
add_library(appModels STATIC)
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

target_include_directories(appModels PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#################
## Deps : gRPC ##
#################

find_package(Threads REQUIRED)
find_package(gRPC REQUIRED)

target_link_libraries(appModels PRIVATE 
    gRPC::grpc
)

list(APPEND GRPC_PROTO
    AssetFetcher.proto
)
add_custom_command(TARGET appModels
    COMMAND $<TARGET_FILE:protobuf::protoc>
    ARGS 
        --grpc_out ${CMAKE_CURRENT_BINARY_DIR}
        -I ${CMAKE_CURRENT_SOURCE_DIR}
        --plugin=protoc-gen-grpc=$<TARGET_FILE:gRPC::grpc_cpp_plugin>
        ${GRPC_PROTO}
)

#############
## Sources ##
#############

target_sources(appModels PRIVATE ${PROTO_SRCS} ${PROTO_HDRS})