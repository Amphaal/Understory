# --------------------------------- FindMiniupnpc Start ---------------------------------
# Locate miniupnp library
# This module defines
#  MINIUPNP_FOUND, if false, do not try to link to miniupnp
#  MINIUPNP_LIBRARY, the miniupnp variant
#  MINIUPNP_INCLUDE_DIR, where to find miniupnpc.h and family)
#  MINIUPNPC_VERSION_1_7_OR_HIGHER, set if we detect the version of miniupnpc is 1.7 or higher
#
# Note that the expected include convention is
#  #include "miniupnpc.h"
# and not
#  #include <miniupnpc/miniupnpc.h>
# This is because, the miniupnpc location is not standardized and may exist
# in locations other than miniupnpc/

if (MINIUPNP_INCLUDE_DIR AND MINIUPNP_LIBRARY)
	# Already in cache, be silent
	set(MINIUPNP_FIND_QUIETLY TRUE)
endif ()

find_path(MINIUPNP_INCLUDE_DIR miniupnpc.h PATH_SUFFIXES miniupnpc)
find_library(MINIUPNP_LIBRARY miniupnpc)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
  miniupnpc DEFAULT_MSG
  MINIUPNP_LIBRARY
  MINIUPNP_INCLUDE_DIR
)

add_library(miniupnpc STATIC IMPORTED)
set_target_properties(miniupnpc 
  PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${MINIUPNP_INCLUDE_DIR}"
    IMPORTED_LOCATION "${MINIUPNP_LIBRARY}"
)

mark_as_advanced(MINIUPNP_INCLUDE_DIR MINIUPNP_LIBRARY)
# --------------------------------- FindMiniupnpc End ---------------------------------