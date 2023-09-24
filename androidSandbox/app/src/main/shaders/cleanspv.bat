@echo off
for /d /r %%i in (*) do (
    if exist %%i\vert.spv (del %%i\vert.spv)
    if exist %%i\frag.spv (del %%i\frag.spv)
    if exist %%i\comp.spv (del %%i\comp.spv)
)

