call "%VS100COMNTOOLS%vsvars32.bat"

mkdir libs
pushd libs
cmake -G "Visual Studio 10" ../

popd
