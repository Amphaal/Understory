find_path(ZMQ_INCLUDE_DIR zmq.h)
find_library(ZMQ_LIBRARY NAMES zmq)

include (FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set ZMQ_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args (
    ZeroMQ DEFAULT_MSG 
    ZMQ_LIBRARY 
    ZMQ_INCLUDE_DIR
)

add_library(libzmq STATIC IMPORTED)
set_target_properties(libzmq 
  PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${ZMQ_INCLUDE_DIR}"
    IMPORTED_LOCATION "${ZMQ_LIBRARY}"
)