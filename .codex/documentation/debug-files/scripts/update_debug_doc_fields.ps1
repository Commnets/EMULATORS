param(
    [string]$StatusFile = "",
    [string]$ExportDirectory = "",
    [switch]$SkipFieldUpdate
)

$ErrorActionPreference = "Stop"
$root = (Resolve-Path (Join-Path $PSScriptRoot "..\..\..\..")).Path
$files = Get-ChildItem (Join-Path $root "docs") -Recurse -Filter "*_DEBUG_File_Format.docx"
$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
if ($ExportDirectory -ne "") {
    New-Item -ItemType Directory -Force -Path $ExportDirectory | Out-Null
}

try {
    foreach ($file in $files) {
        if ($ExportDirectory -ne "") {
            $pdf = Join-Path $ExportDirectory ($file.BaseName + ".pdf")
            if (Test-Path -LiteralPath $pdf) { continue }
        }
        $document = $word.Documents.Open($file.FullName, $false, $false)
        if (-not $SkipFieldUpdate) {
            foreach ($toc in $document.TablesOfContents) { $toc.Update() }
            foreach ($tof in $document.TablesOfFigures) { $tof.Update() }
            $document.Fields.Update() | Out-Null
            foreach ($section in $document.Sections) {
                foreach ($header in $section.Headers) { $header.Range.Fields.Update() | Out-Null }
                foreach ($footer in $section.Footers) { $footer.Range.Fields.Update() | Out-Null }
            }
            $document.Save()
        }
        if ($ExportDirectory -ne "") {
            $document.ExportAsFixedFormat($pdf, 17)
            try { $document.Close(0) } catch { }
            break
        }
        $document.Close(0)
    }
    if ($StatusFile -ne "") {
        Set-Content -LiteralPath $StatusFile -Value "saved" -Encoding ascii
    }
}
finally {
    try { $word.Quit(0) } catch { }
}
