@REM <-- Set all the variables

@SET WORKSPACE_NAME=mgcas.workspace


@REM <-- Create shell & Run background
@echo off
@if not "%1" == "h" mshta vbscript:createobject("wscript.shell").run("%~nx0 h",0)(window.close)&&exit
@REM -->


@REM <-- Load project
codeblocks.exe  %WORKSPACE_NAME%
@REM -->
