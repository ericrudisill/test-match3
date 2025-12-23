# iOS-specific CMake configuration

# Set iOS-specific properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Match3Game"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.match3game.app"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"  # iPhone and iPad
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
)

# Link iOS frameworks
target_link_libraries(${PROJECT_NAME} PRIVATE
    "-framework Foundation"
    "-framework UIKit"
    "-framework CoreGraphics"
    "-framework QuartzCore"
    "-framework OpenGLES"
    "-framework Metal"
    "-framework AVFoundation"
    "-framework AudioToolbox"
    "-framework CoreMotion"
    "-framework GameController"
)

# Info.plist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist
    )
endif()
