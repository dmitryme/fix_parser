mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_TESTS=YES ..
make
