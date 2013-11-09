mkdir -p libs
cd libs
cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=YES ../..
make
