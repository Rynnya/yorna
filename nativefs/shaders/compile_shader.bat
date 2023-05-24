@echo off

if %1.==. (
    echo Filepath not provided!
    exit /b 1
) else (
    set fileToCompile=%1
)

C:\VulkanSDK\1.3.211.0\Bin\glslc.exe %fileToCompile% -o %fileToCompile%.spv

if %errorlevel% equ 0 (
    echo Successfully compiled %fileToCompile%.spv
) else (
    echo Failed to compile %fileToCompile%.spv
    exit /b %errorlevel%
)