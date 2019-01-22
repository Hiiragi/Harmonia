# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(../../../../libs/src/libogg/include)
include_directories(../../../../libs/src/libvorbis/include)
include_directories(../../../../src/c)

add_library( # Sets the name of the library.
             Harmonia

             # Sets the library as a shared library.
             SHARED

             # Ogg
             ../../../../libs/src/libogg/src/bitwise.c
             ../../../../libs/src/libogg/src/framing.c

             # Vorbis
             ../../../../libs/src/libvorbis/lib/analysis.c
             ../../../../libs/src/libvorbis/lib/bitrate.c
             ../../../../libs/src/libvorbis/lib/block.c
             ../../../../libs/src/libvorbis/lib/codebook.c
             ../../../../libs/src/libvorbis/lib/envelope.c
             ../../../../libs/src/libvorbis/lib/floor0.c
             ../../../../libs/src/libvorbis/lib/floor1.c
             ../../../../libs/src/libvorbis/lib/info.c
             ../../../../libs/src/libvorbis/lib/lookup.c
             ../../../../libs/src/libvorbis/lib/lpc.c
             ../../../../libs/src/libvorbis/lib/lsp.c
             ../../../../libs/src/libvorbis/lib/mapping0.c
             ../../../../libs/src/libvorbis/lib/mdct.c
             ../../../../libs/src/libvorbis/lib/psy.c
             ../../../../libs/src/libvorbis/lib/registry.c
             ../../../../libs/src/libvorbis/lib/res0.c
             ../../../../libs/src/libvorbis/lib/sharedbook.c
             ../../../../libs/src/libvorbis/lib/smallft.c
             ../../../../libs/src/libvorbis/lib/synthesis.c
             ../../../../libs/src/libvorbis/lib/window.c

             # VorbisFile
             ../../../../libs/src/libvorbis/lib/vorbisfile.c

             # Provides a relative path to your source file(s).
             ../../../../src/c/HarmoniaInterface.cpp
            ../../../../src/c/Harmonia.cpp
            ../../../../src/c/AbstractEffectCommand.cpp
            ../../../../src/c/AbstractSoundData.cpp
            ../../../../src/c/DuckerCommand.cpp
            ../../../../src/c/HarmoniaErrorData.cpp
            ../../../../src/c/PlayingData.cpp
            ../../../../src/c/RegisteredSoundData.cpp
            ../../../../src/c/SoundEventData.cpp
            ../../../../src/c/SoundGroup.cpp
            src/main/cpp/native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       Harmonia

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                        OpenSLES)