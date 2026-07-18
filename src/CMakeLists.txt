set(RIME_SOURCES
    rimestate.cpp
    rimeengine.cpp
    rimecandidate.cpp
    rimesession.cpp
    rimeaction.cpp
    rimefactory.cpp
)

set(RIME_LINK_LIBRARIES
    Fcitx5::Core
    Fcitx5::Config
    ${RIME_TARGET}
    Fcitx5::Module::Notifications
    Pthread::Pthread
)

find_package(Fcitx5ModuleDBus QUIET)
if (Fcitx5ModuleDBus_FOUND)
    list(APPEND RIME_SOURCES rimeservice.cpp)
    list(APPEND RIME_LINK_LIBRARIES Fcitx5::Module::DBus)
else()
    add_definitions(-DFCITX_RIME_NO_DBUS)
endif()

add_fcitx5_addon(rime-taigi ${RIME_SOURCES})
target_link_libraries(rime-taigi ${RIME_LINK_LIBRARIES})
install(TARGETS rime-taigi DESTINATION "${CMAKE_INSTALL_LIBDIR}/fcitx5")
fcitx5_translate_desktop_file(rime-taigi.conf.in rime-taigi.conf)
configure_file(rime-taigi-addon.conf.in.in rime-taigi-addon.conf.in)
fcitx5_translate_desktop_file("${CMAKE_CURRENT_BINARY_DIR}/rime-taigi-addon.conf.in" rime-taigi-addon.conf)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rime-taigi.conf" DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod" COMPONENT config)
# Note the name of this file is used as the addon's unique name, then used as
# part of the name of the entry point function. So it must be a valid C
# identifier.
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/rime-taigi-addon.conf" RENAME rime_taigi.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon" COMPONENT config)
