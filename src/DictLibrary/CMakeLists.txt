INCLUDE_DIRECTORIES(../)
AUX_SOURCE_DIRECTORY(. DICTLIBRARY_SRC_LIST)
ADD_LIBRARY(lib_dictlibrary ${DICTLIBRARY_SRC_LIST})
