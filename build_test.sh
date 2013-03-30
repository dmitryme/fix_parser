mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=YES ..
make
