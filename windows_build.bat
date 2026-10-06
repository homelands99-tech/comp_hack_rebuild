@echo off
rem Build the COMP_hack servers on Windows (64-bit).
rem
rem Requirements:
rem   - Visual Studio 2022 with "Desktop development with C++" and the
rem     "MSVC v141 - VS 2017 C++ x64/x86 build tools" component
rem   - Qt 5 for MSVC 2015/2017 64-bit (needed by the configure step; set
rem     QT_DIR below or in the environment)
rem
rem Usage:
rem   windows_build.bat configure
rem   windows_build.bat build [target ...]   (default: the three servers)
rem   windows_build.bat all                  (configure + build servers)
rem
rem Output: build\bin\comp_lobby.exe, comp_world.exe, comp_channel.exe
setlocal

if "%QT_DIR%"=="" set "QT_DIR=C:\Qt\Qt5.10.1\5.10.1\msvc2015_64"
set "SRC=%~dp0"
set "SRC=%SRC:~0,-1%"
set "BLD=%~dp0build"

rem Find Visual Studio 2022.
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo Visual Studio Installer ^(vswhere.exe^) was not found.
    exit /b 1
)
for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -version [17^,18^) -products * -property installationPath`) do set "VSDIR=%%i"
if "%VSDIR%"=="" (
    echo Visual Studio 2022 was not found.
    exit /b 1
)

rem The release was built with MSVC v141 (14.16); use the same compiler.
call "%VSDIR%\VC\Auxiliary\Build\vcvarsall.bat" x64 -vcvars_ver=14.16 >nul || (
    echo The MSVC v141 ^(14.16^) x64 build tools are not installed.
    exit /b 1
)

rem Keep other toolchains (MinGW/Strawberry gcc, a system zlib) out of the
rem build so CMake only finds MSVC and the bundled dependencies.
for /f "delims=" %%p in ('powershell -NoProfile -Command "($env:PATH -split ';' | Where-Object { $_ -and $_ -notmatch 'Strawberry|mingw|msys|cygwin|zlib' }) -join ';'"') do set "PATH=%%p"
set ZLIB_ROOT=

rem The dependency snapshots need CMake 3.x (CMake 4 dropped support for
rem cmake_minimum_required < 3.5); use the copy bundled with Visual Studio.
set "PATH=%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;%VSDIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"

rem Single-config (Ninja) builds do not pass the build type to the external
rem dependency projects; CMake 3.22+ picks it up from this variable instead.
set CMAKE_BUILD_TYPE=RelWithDebInfo

if /i "%~1"=="configure" goto configure
if /i "%~1"=="build" goto build
if /i "%~1"=="all" (
    call :configure_step || exit /b 1
    goto build
)
echo Usage: windows_build.bat configure ^| build [target ...] ^| all
exit /b 1

:configure
call :configure_step
exit /b %errorlevel%

:configure_step
cmake -S "%SRC%" -B "%BLD%" -G Ninja ^
    -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl ^
    -DCMAKE_BUILD_TYPE=RelWithDebInfo ^
    "-DCMAKE_PREFIX_PATH=%QT_DIR%" ^
    -DDISABLE_TESTING=ON -DGENERATE_DOCUMENTATION=OFF -DBUILD_DREAM=OFF ^
    -DCOVERALLS=OFF
exit /b %errorlevel%

:build
shift
set T=
:targets
if "%~1"=="" goto run
set T=%T% %1
shift
goto targets
:run
if "%T%"=="" set T=comp_lobby comp_world comp_channel
cmake --build "%BLD%" --target %T% -- -k 0
exit /b %errorlevel%
