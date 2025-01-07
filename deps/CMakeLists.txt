include(FetchContent)

set(LIBLCF_VERSION 0.8)

find_package(liblcf)

if (NOT liblcf_FOUND)
    FetchContent_Declare(
        liblcf
        URL "https://github.com/EasyRPG/liblcf/archive/${LIBLCF_VERSION}.zip"
        FIND_PACKAGE_ARGS NAMES lcf
    )

    FetchContent_MakeAvailable(liblcf)
endif ()
