#!/usr/bin/bash

# Compile all .vert files within the shaders/ folder into .spv files
# Compile all .frag files within the shaders/ folder into .spv files
# Compile all .comp files within the shaders/ folder into .spv files


count=0
search_dir=SimpleTriangle
for entry in */*.vert
do
    count=$((count+1))
    echo Compile ${entry}
    glslc.exe ${entry} -o ${entry}.spv
done
for entry in */*.frag
do
    count=$((count+1))
    echo Compile ${entry}
    glslc.exe ${entry} -o ${entry}.spv
done
for entry in */*.comp
do
    count=$((count+1))
    echo Compile ${entry}
    glslc.exe ${entry} -o ${entry}.spv
done
echo Total compiled: ${count}

read

# *** Below are .bat script ***
# @echo off
# for /d /r %%i in (*) do (
#     if exist %%i/shader.vert (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.vert -o %%i/vert.spv)
#     if exist %%i/shader.frag (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.frag -o %%i/frag.spv)
#     if exist %%i/shader.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.comp -o %%i/comp.spv)
#     if exist %%i/shaderTranspose.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shaderTranspose.comp -o %%i/compTranspose.spv)
#     if exist %%i/shaderOptimizeMatMul.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shaderOptimizeMatMul.comp -o %%i/compOptimizeMatMul.spv)
# )

