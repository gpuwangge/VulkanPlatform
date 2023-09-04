for /d /r %%i in (*) do (
    if exist %%i/shader.vert (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.vert -o %%i/vert.spv)
    if exist %%i/shader.frag (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.frag -o %%i/frag.spv)
    if exist %%i/shader.comp (%VULKAN_SDK%/Bin/glslc.exe %%i/shader.comp -o %%i/comp.spv)
)

