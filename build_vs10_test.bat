call "%VS100COMNTOOLS%\vsvars32.bat"

mkdir build
pushd build
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_TESTS=YES -G "Visual Studio 10" ../

popd
