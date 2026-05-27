param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string]$EditorCmd = "C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
)

$ProjectFile = Join-Path $ProjectRoot "UISample.uproject"

if (!(Test-Path $ProjectFile)) {
    throw "Project file not found: $ProjectFile"
}

if (!(Test-Path $EditorCmd)) {
    throw "UnrealEditor-Cmd.exe not found: $EditorCmd"
}

Write-Host "GenerateUIEventBindings is a compatibility wrapper."
Write-Host "Running RefreshUIEventContracts..."

& $EditorCmd $ProjectFile -run=RefreshUIEventContracts -unattended -nop4 -nosplash -NoShaderCompile -NullRHI
exit $LASTEXITCODE
