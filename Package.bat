ECHO ON
for /f "delims=|" %%x in (.uedir) do set UEDir=%%x
IF "%UEDir%" == "" (
	ECHO "Error: no directory found for Unreal Engine"
	EXIT /b
)
SET ClientConfig=Development
IF "%1" == "" (
	ECHO "Warn: default to Development"
) ELSE (
	SET ClientConfig=%1
)
SHIFT

ECHO "Packaging UEBox"
"%UEDir%"\Engine\Build\BatchFiles\RunUAT.bat BuildCookRun -unattended -utf8output -nocompile -cook -stage -package -prereqs -pak -nop4 -unrealexe=UnrealEditor-Cmd.exe -clientconfig=%ClientConfig% -platform=Win64 -project="%CD%\UEBox.uproject" -archive -archivedirectory="%CD%\Packaged" -createreleaseversion=0.0.1 -map=/Game/Maps/RoomLevel

