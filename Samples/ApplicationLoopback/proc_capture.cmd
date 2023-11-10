set app=explorer.exe
cd c:\appac

REM Look for first executable file that match the filename and get its PID
for /f "tokens=2" %%a in ('tasklist /nh /fi "imagename eq %app%"') do if not [%%a]==[No] (set process_pid=%%a & goto capture)

:capture
ApplicationLoopback_EnterToStopCFG %process_pid% includetree Captured.wav 48000 32 2 3

pause