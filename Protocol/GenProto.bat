@echo off
setlocal enabledelayedexpansion

set PROTOC_PATH=..\vcpkg_installed\x64-windows\tools\protobuf\protoc.exe
set CS_OUT_PATH=.\Generated_CS
set CPP_OUT_PATH=.\Generated_CPP
set SRC_DEST_PATH=..\Src

if not exist %CS_OUT_PATH% mkdir %CS_OUT_PATH%
if not exist %CPP_OUT_PATH% mkdir %CPP_OUT_PATH%

echo [Protobuf Compilation Start]

for %%f in (*.proto) do (
    echo Processing: %%f
    %PROTOC_PATH% -I=.\ --csharp_out=%CS_OUT_PATH% %%f
	
	%PROTOC_PATH% -I=.\ --cpp_out=%CPP_OUT_PATH% %%f
)

echo [Compilation Finished]

echo [2/3] Copying generated files to Src folder...

xcopy "%CPP_OUT_PATH%\*" "%SRC_DEST_PATH%\" /Y /I /D

echo [3/3] All processes finished successfully.

pause