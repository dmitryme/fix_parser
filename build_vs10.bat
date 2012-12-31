call "%VS100COMNTOOLS%\vsvars32.bat"

mkdir build
pushd build
cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 10" ../

popd
