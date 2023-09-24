@echo off
IF "%~1" == "" (
	ECHO "Error: provide path to local Unreal Engine installation"
	EXIT /b
)
SET "UEDir=%~1"
SHIFT

ECHO "Generating build files"
CALL "%UEDIR%"\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe -CMakefile -vscode -projectfiles -project="%CD%\UEBox.uproject" -game -rocket -progress
ECHO %UEDir%> .uedir
