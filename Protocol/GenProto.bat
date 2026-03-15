@echo off
setlocal enabledelayedexpansion

set PROTOC_PATH=..\Tools\protoc.exe

set CS_OUT_PATH=.\Generated_CS
set CPP_OUT_PATH=.\Generated_CPP

if not exist %CS_OUT_PATH% mkdir %CS_OUT_PATH%
if not exist %CPP_OUT_PATH% mkdir %CPP_OUT_PATH%

echo [Protobuf Compilation Start]

for %%f in (*.proto) do (
    echo Processing: %%f
    %PROTOC_PATH% -I=.\ --csharp_out=%CS_OUT_PATH% %%f
	
	%PROTOC_PATH% -I=.\ --cpp_out=%CPP_OUT_PATH% %%f
)

echo [Compilation Finished]

copy .\Generated_CPP ..\Src\Generated_CPP

pause