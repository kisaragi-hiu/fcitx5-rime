# Author: Marguerite Su <i@marguerite.su>
# License: GPL
# Description: find {{{name}}} schema collection package.
# {{{uppername}}}_DATA_FOUND - System has {{{lowername}}}-data package
# {{{uppername}}}_DATA_DIR - {{{lowername}}}-data absolute path

set({{{uppername}}}_DATA_FIND_DIR "${CMAKE_INSTALL_PREFIX}/share/{{{lowername}}}-data"
                       "${CMAKE_INSTALL_PREFIX}/share/{{{lowername}}}/data"
                       "/usr/share/{{{lowername}}}-data"
                       "/usr/share/{{{lowername}}}/data")

set({{{uppername}}}_DATA_FOUND FALSE)

foreach(_{{{uppername}}}_DATA_DIR ${{{{uppername}}}_DATA_FIND_DIR})
    if (IS_DIRECTORY ${_{{{uppername}}}_DATA_DIR})
        set({{{uppername}}}_DATA_FOUND True)
        set({{{uppername}}}_DATA_DIR ${_{{{uppername}}}_DATA_DIR})
    endif (IS_DIRECTORY ${_{{{uppername}}}_DATA_DIR})
endforeach(_{{{uppername}}}_DATA_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args({{{name}}}Data DEFAULT_MSG {{{uppername}}}_DATA_DIR)
mark_as_advanced({{{uppername}}}_DATA_DIR)
