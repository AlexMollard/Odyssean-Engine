@ECHO OFF
REM This script will compile all shaders in the current directory.
REM This script requires glslangValidator.exe to be in the same directory.
REM It will compile all .vert and .frag files in the current directory.

REM Set the path to the glslangValidator.exe
SET GLSLANGVALIDATOR=glslangValidator.exe

REM Set the path to the current directory
SET CURRENTDIR=%~dp0

REM Set the path to the output directory
SET OUTPUTDIR=%CURRENTDIR%compiled

REM Set a variable to false to indicate that the script failed
SET FAILED=FALSE

REM Create the output directory if it doesn't exist
IF NOT EXIST %OUTPUTDIR% MKDIR %OUTPUTDIR%

REM Compile all .vert files
FOR %%f IN (%CURRENTDIR%*.vert) DO (
    ECHO Compiling %%~nf.vert
    %GLSLANGVALIDATOR% -V %%f -o %OUTPUTDIR%\%%~nf_vert.spv
    IF EXIST %OUTPUTDIR%\%%~nf_vert.spv (
        ECHO %%~nf.vert compiled successfully.
    ) ELSE (
        ECHO ******* %%~nf.vert failed to compile. *******
        SET FAILED=TRUE
    )
    ECHO.
)

REM Compile all .frag files
FOR %%f IN (%CURRENTDIR%*.frag) DO (
    ECHO Compiling %%~nf.frag
    %GLSLANGVALIDATOR% -V %%f -o %OUTPUTDIR%\%%~nf_frag.spv
    IF EXIST %OUTPUTDIR%\%%~nf_frag.spv (
        ECHO %%~nf.frag compiled successfully.
    ) ELSE (
        ECHO ******* %%~nf.frag failed to compile. *******
        SET FAILED=TRUE
    )
    ECHO.
)

REM Pause the script if it failed
if (%FAILED%) == (TRUE) PAUSE

REM Sleep for 25 seconds
TIMEOUT /T 15 /NOBREAK