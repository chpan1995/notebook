cmake_minimum_required(VERSION 3.16)
project(TestGrpc C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

get_filename_component(hw_proto "./helloworld.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/helloworld.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")
# # Include generated *.pb.h files
# include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(hw_grpc_proto ${hw_proto_srcs} ${hw_proto_hdrs} ${hw_grpc_srcs} ${hw_grpc_hdrs})

target_link_libraries(hw_grpc_proto
 PUBLIC  
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
)

add_executable(${PROJECT_NAME} main.cpp)
add_executable(${PROJECT_NAME}_client client.cpp)

target_include_directories(
 ${PROJECT_NAME}
 PUBLIC
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_BINARY_DIR}
)

target_include_directories(
 ${PROJECT_NAME}_client
 PUBLIC
${CMAKE_CURRENT_BINARY_DIR}
${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
)

target_link_libraries(${PROJECT_NAME}_client
    PUBLIC
    hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
)


