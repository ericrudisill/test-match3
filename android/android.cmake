# Android-specific CMake configuration

# Set Android-specific properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_NAME "main"
)

# Link Android libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    log
    android
    EGL
    GLESv2
)

# Set Android manifest and resources
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        ANDROID_MANIFEST ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml
    )
endif()
