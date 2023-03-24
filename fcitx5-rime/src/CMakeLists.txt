set({{{uppername}}}_SOURCES
    {{{lowername}}}state.cpp
    {{{lowername}}}engine.cpp
    {{{lowername}}}candidate.cpp
    {{{lowername}}}service.cpp
)
add_library(fcitx5_{{{lowername}}} MODULE ${{{{uppername}}}_SOURCES})
target_link_libraries(fcitx5_{{{lowername}}}
  Fcitx5::Core
  Fcitx5::Config
  {{{lowername}}}
  Fcitx5::Module::Notifications
  Fcitx5::Module::DBus)
set_target_properties(fcitx5_{{{lowername}}} PROPERTIES PREFIX "")
install(TARGETS fcitx5_{{{lowername}}} DESTINATION "${CMAKE_INSTALL_LIBDIR}/fcitx5")
fcitx5_translate_desktop_file({{{lowername}}}.conf.in {{{lowername}}}.conf)
configure_file({{{lowername}}}-addon.conf.in.in {{{lowername}}}-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/{{{lowername}}}-addon.conf.in" {{{lowername}}}-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/{{{lowername}}}.conf" DESTINATION "${CMAKE_INSTALL_DATADIR}/fcitx5/inputmethod")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/{{{lowername}}}-addon.conf" RENAME {{{lowername}}}.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

