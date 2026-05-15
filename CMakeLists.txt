cmake_minimum_required(VERSION 3.19)
project(xor-app LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(xor-app
    WIN32 MACOSX_BUNDLE
    main.cpp
    application.cpp
    application.h
    application.ui
)

target_link_libraries(xor-app
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS xor-app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET xor-app
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
