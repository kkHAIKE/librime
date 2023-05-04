#!/bin/bash
set -euo pipefail

cd $(dirname $0)

# install lua plugin
if [ ! -d plugins/lua ]; then
    bash install-plugins.sh kkHAIKE/librime-lua
fi

# install lua
if [ ! -d plugins/lua/thirdparty ]; then
    $(cd plugins/lua && bash action-install.sh)
fi

mkdir -p build_ios
cd build_ios

cmake .. -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_TEST=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -G Xcode -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake \
    -DPLATFORM=OS64COMBINED \
    -DDEPLOYMENT_TARGET=14.0 \
    -DCMAKE_MACOSX_BUNDLE=OFF \
    -DENABLE_STRICT_TRY_COMPILE=ON

# build combine static library
rm -f librime.a librime_iphone.a

cmake --build . --config Release --target combine
mv librime.a librime_iphone.a
cmake --build . --config Release --target combine -- -sdk iphonesimulator

# create xcframework
rm -rf ../dist_ios/librime.xcframework

mkdir -p public_header
cp ../src/*.h public_header
xcodebuild -create-xcframework \
    -library librime_iphone.a \
    -headers public_header \
    -library librime.a \
    -headers public_header \
    -output ../dist_ios/librime.xcframework
