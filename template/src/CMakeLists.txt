set(RIME_SOURCES
    rimestate.cpp
    rimeengine.cpp
    rimecandidate.cpp
    rimeservice.cpp
)
add_library(rime MODULE ${RIME_SOURCES})
target_link_libraries(rime Fcitx5::Core Fcitx5::Config PkgConfig::Rime Fcitx5::Module::Notifications Fcitx5::Module::DBus)
set_target_properties(rime PROPERTIES PREFIX "")
install(TARGETS rime DESTINATION "${CMAKE_INSTALL_LIBDIR}/fcitx5")
fcitx5_translate_desktop_file({{{lowername}}}.conf.in {{{lowername}}}.conf)
configure_file({{{lowername}}}-addon.conf.in.in {{{lowername}}}-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/{{{lowername}}}-addon.conf.in" {{{lowername}}}-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/{{{lowername}}}.conf" DESTINATION "${CMAKE_INSTALL_DATADIR}/fcitx5/inputmethod")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/{{{lowername}}}-addon.conf" RENAME {{{lowername}}}.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

