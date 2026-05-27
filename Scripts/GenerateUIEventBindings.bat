@echo off
setlocal

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0GenerateUIEventBindings.ps1" %*
exit /b %ERRORLEVEL%
