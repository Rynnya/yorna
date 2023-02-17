@echo off

if %1.==. (
    set currentPath=%cd%
) else (
    set currentPath=%1
)
echo Running into %currentPath%...

echo Starting compiling *.vert files...
for %%f in (%currentPath%\*.vert) do (
    echo Compiling %%f...
    C:\VulkanSDK\1.3.211.0\Bin\glslc.exe %%f -o %%f.spv
)

echo *.vert files compiled!
echo Starting compiling *.frag files...
for %%f in (%currentPath%\*.frag) do (
    echo Compiling %%f...
    C:\VulkanSDK\1.3.211.0\Bin\glslc.exe %%f -o %%f.spv
)

echo *.frag files compiled!
echo Compiling is done!