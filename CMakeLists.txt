cmake_minimum_required(VERSION 3.19)
project(xor-app LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(xor-app
    WIN32 MACOSX_BUNDLE
    main.cpp
    src/application.cpp src/application.h
    src/utils/structures.h
    src/utils/app_state.h src/utils/app_state.cpp
    src/utils/file_processor.h src/utils/file_processor.cpp
    src/services/ifile_search_service.h
    src/services/ifile_processing_service.h
    src/services/file_search_service.h src/services/file_search_service.cpp
    src/services/file_processing_service.h src/services/file_processing_service.cpp
    src/services/orchestrator.h src/services/orchestrator.cpp
    src/controllers/main_window_controller.h src/controllers/main_window_controller.cpp
    resources.qrc
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
