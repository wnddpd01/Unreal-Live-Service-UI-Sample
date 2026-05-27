param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
)

$ManifestPath = Join-Path $ProjectRoot "Source/UISample/UIEvents/UIEvents.json"
$OutputDir = Join-Path $ProjectRoot "Source/UISample/UIEvents"
$OutputPath = Join-Path $OutputDir "GeneratedUIEventIds.h"

if (!(Test-Path $ManifestPath)) {
    throw "UI events manifest not found: $ManifestPath"
}

$Manifest = Get-Content -Raw -Path $ManifestPath | ConvertFrom-Json
$SeenIds = @{}

function ConvertTo-Identifier([string]$Value) {
    $Identifier = [regex]::Replace($Value, "[^A-Za-z0-9_]", "_")
    if ($Identifier.Length -eq 0) {
        return "_"
    }

    if ($Identifier[0] -match "[0-9]") {
        return "_$Identifier"
    }

    return $Identifier
}

$Lines = New-Object System.Collections.Generic.List[string]
$Lines.Add("#pragma once")
$Lines.Add("")
$Lines.Add("#include `"CoreMinimal.h`"")
$Lines.Add("")
$Lines.Add("// Generated UI event identifiers from Source/UISample/UIEvents/UIEvents.json. Do not edit by hand.")
$Lines.Add("")

foreach ($Event in ($Manifest.events | Sort-Object id)) {
    if ([string]::IsNullOrWhiteSpace($Event.id)) {
        throw "UI event entry is missing id."
    }

    if ($SeenIds.ContainsKey($Event.id)) {
        throw "Duplicate UI event id: $($Event.id)"
    }

    $SeenIds[$Event.id] = $true
    $Parts = $Event.id.Split(".")
    if ($Parts.Length -lt 2) {
        throw "UI event id must have at least one namespace and one event name: $($Event.id)"
    }

    $NamespaceParts = $Parts[0..($Parts.Length - 2)] | ForEach-Object { ConvertTo-Identifier $_ }
    $EventName = ConvertTo-Identifier $Parts[$Parts.Length - 1]

    $Lines.Add("namespace UIEvents")
    $Lines.Add("{")
    foreach ($Part in $NamespaceParts) {
        $Lines.Add("namespace $Part")
        $Lines.Add("{")
    }

    $EscapedId = $Event.id.Replace("\", "\\").Replace('"', '\"')
    $Lines.Add("inline const FName $EventName = TEXT(`"$EscapedId`");")

    for ($Index = 0; $Index -lt $NamespaceParts.Length; ++$Index) {
        $Lines.Add("}")
    }
    $Lines.Add("}")
    $Lines.Add("")
}

New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null
Set-Content -Path $OutputPath -Value ($Lines -join "`r`n") -NoNewline
Write-Host "Generated $OutputPath"
