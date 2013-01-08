call "%VS100COMNTOOLS%\vsvars32.bat"

mkdir build
pushd build
cmake -G "Visual Studio 10" ../

popd
