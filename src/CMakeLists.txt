set(APP_SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_executable(${PROJECT_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/manager/subsystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/manager/power_state_manager.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/manager/modules/canbus.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/manager/modules/ignition.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/manager/modules/lcd.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/manager/modules/lcd_welcome_msg.cpp"
)

target_link_libraries(${PROJECT_NAME} mbed-os VolvoLib)

mbed_set_post_build(${PROJECT_NAME})
