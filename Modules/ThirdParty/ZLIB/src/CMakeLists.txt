set(INC_INSTALL_DIR "${ITKZLIB_INSTALL_INCLUDE_DIR}/itkzlib-ng")
set(BIN_INSTALL_DIR "${ITKZLIB_INSTALL_RUNTIME_DIR}")
set(LIB_INSTALL_DIR "${ITKZLIB_INSTALL_LIBRARY_DIR}")
set(PKGCONFIG_INSTALL_DIR "${ITKZLIB_INSTALL_ARCHIVE_DIR}")
set(ZLIB_SYMBOL_PREFIX "itkzlib_")
set(ZLIB_COMPAT ON)
set(ZLIB_DUAL_LINK OFF)
set(ZLIB_ENABLE_TESTS OFF)
add_subdirectory(itkzlib-ng)
mark_as_advanced(FORCE
  BIN_INSTALL_DIR
  INC_INSTALL_DIR
  LIB_INSTALL_DIR
  PKGCONFIG_INSTALL_DIR
  WITH_CODE_COVERAGE
  WITH_FUZZERS
  WITH_GZFILEOP
  WITH_MAINTAINER_WARNINGS
  WITH_NATIVE_INSTRUCTIONS
  WITH_NEW_STRATEGIES
  WITH_OPTIM
  WITH_NATIVE_INSTRUCTIONS
  WITH_SANITIZER
  )
