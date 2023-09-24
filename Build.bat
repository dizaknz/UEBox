ECHO ON
for /f "delims=|" %%x in (.uedir) do set UEDir=%%x
IF "%UEDir%" == "" (
	ECHO "Error: no directory found for Unreal Engine"
	EXIT /b
)
SET BuildType=Development
IF "%1" == "" (
	ECHO "Warn: default to Development"
) ELSE (
	SET BuildType=%1
)
SHIFT

ECHO "Building UEBox editor"
CALL "%UEDir%"\Engine\Build\BatchFiles\RunUAT.bat BuildEditor -project="%CD%\UEBox.uproject" -platform=Win64 -notools
ECHO "Building UEBox game"
CALL "%UEDir%"\Engine\Build\BatchFiles\RunUAT.bat BuildGame -project="%CD%\UEBox.uproject" -platform=Win64 -notools -configuration=%BuildType%
