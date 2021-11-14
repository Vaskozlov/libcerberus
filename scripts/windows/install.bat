@echo off

set tempPath=".\tmp"

if not exist "%tempPath%\tmp"      mkdir "%tempPath%\tmp"

cd "%tempPath%"
set extra_path=\vcpkg\scripts\buildsystems\vcpkg.cmake
set CMAKE_TOOLCHAIN_FILE=%cd%%extra_path%

powershell -Command "git clone https://github.com/microsoft/vcpkg"
powershell -Command ".\vcpkg\bootstrap-vcpkg.bat"
powershell -Command ".\vcpkg\vcpkg install fmt:x64-windows"

cd ..

echo "SUCCESS! Installation is completed"

powershell -Command "ls"
