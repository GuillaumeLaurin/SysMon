set(CLIENT_NAME "SysMonClient")

set(SPDLOG_NO_EXCEPTIONS ON)

# Dependencies

add_library(sqlite3 STATIC
    ${CMAKE_SOURCE_DIR}/vendor/sqlite/sqlite3.c
)

add_library(imgui STATIC
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_win32.cpp
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_dx11.cpp
)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/spdlog/ vendor/spdlog)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/nlohmann-json/ vendor/nlohmann-json)

add_executable(${CLIENT_NAME}
    src/Client.cpp
    # --- Core ---
    src/core/DIContainer.cpp
    src/core/DriverConnector.cpp
    src/core/EventProcessor.cpp
    # --- Reporter ---
    src/reporter/ErrorQueue.cpp
    src/reporter/ErrorRecord.cpp
    src/reporter/JsonFormatter.cpp
    src/reporter/ConsoleSink.cpp
    src/reporter/FileSink.cpp
    src/reporter/ErrorDispatcher.cpp
    src/reporter/SeverityFilter.cpp
    src/reporter/CategoryFilter.cpp
    src/reporter/RateLimitFilter.cpp
    src/reporter/DumpProvider.cpp
    # --- Exceptions ---
    src/exceptions/SysMonException.cpp
    src/exceptions/ExceptionHandler.cpp
    # --- Logging ---
    src/logging/Logger.cpp
    # --- Storage ---
    src/storage/Database.cpp
    src/storage/EventRepository.cpp
    src/storage/ConfigRepository.cpp
)

target_compile_features(${CLIENT_NAME} PRIVATE cxx_std_17)

target_compile_options(${CLIENT_NAME} PRIVATE
    /W4
    /WX
    /MP
    /EHsc
    $<$<CONFIG:Debug>:/Od /Zi /RTC1>
    $<$<CONFIG:Release>:/O2 /GL>
)

target_link_options(${CLIENT_NAME} PRIVATE
    $<$<CONFIG:Release>:/LTCG>
)

target_link_libraries(${CLIENT_NAME} PRIVATE
    kernel32
    user32
    sqlite3
    d3d11
    dxgi
    dwmapi
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    imgui
    dbghelp
    psapi
    ole32
)

target_include_directories(${CLIENT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/vendor/sqlite/
    include/
)

target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/imgui/
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends/
)

set_target_properties(${CLIENT_NAME} PROPERTIES
    OUTPUT_NAME "${CLIENT_NAME}"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_compile_definitions(${CLIENT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)