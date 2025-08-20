set(APP_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_executable(${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/subsystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/power_state_manager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/modules/canbus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/modules/ignition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/modules/lcd.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/modules/lcd_welcome_msg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/console/console.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/console/command_registry.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/opencfe/console/lcd_command.cpp"
)

target_link_libraries(${PROJECT_NAME} mbed-os VolvoLib)

mbed_set_post_build(${PROJECT_NAME})
