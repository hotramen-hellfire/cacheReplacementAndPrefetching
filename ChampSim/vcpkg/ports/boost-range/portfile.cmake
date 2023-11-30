# Automatically generated by scripts/boost/generate-ports.ps1

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO boostorg/range
    REF boost-1.81.0
    SHA512 7cdba4d90b181e1d9d787c04903bb970569a6797eef77e6ad400709d7734568a8a7e547aea4be9fcf0ae698dac202d1cbf868287c19c7db6c2c27490ee698152
    HEAD_REF master
)

include(${CURRENT_INSTALLED_DIR}/share/boost-vcpkg-helpers/boost-modular-headers.cmake)
boost_modular_headers(SOURCE_PATH ${SOURCE_PATH})