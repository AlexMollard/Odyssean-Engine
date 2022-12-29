rem This is a batch file to compile the shaders from glsl to spir-v.
rem It requires the glslangValidator.exe to be in the same folder as this batch file.
rem It will compile all the shaders in the folder and subfolders.

rem Set the path to the glslangValidator.exe
set GLSLANGVALIDATOR=glslangValidator.exe

rem Set the path to the folder containing the shaders
set SHADERPATH=.

GOTO :START

:START
rem Compile all the shaders in the folder and subfolders
for /r %SHADERPATH% %%f in (*.vert) do %GLSLANGVALIDATOR% -V %%f
for /r %SHADERPATH% %%f in (*.frag) do %GLSLANGVALIDATOR% -V %%f

rem Hold the window open
pause