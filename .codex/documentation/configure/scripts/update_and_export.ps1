param(
    [Parameter(Mandatory = $true)][string]$DocumentPath,
    [Parameter(Mandatory = $true)][string]$PdfPath
)

$ErrorActionPreference = 'Stop'
$documentAbsolute = (Resolve-Path -LiteralPath $DocumentPath).Path
$pdfAbsolute = [System.IO.Path]::GetFullPath($PdfPath)
$wordApplication = $null
$wordDocument = $null
$wordProcessId = [uint32]0
$existingWordIds = @(Get-Process WINWORD -ErrorAction SilentlyContinue | Select-Object -ExpandProperty Id)
if (-not ('ConfigureWordWindow' -as [type])) {
    Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public static class ConfigureWordWindow { [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr window, out uint process); }'
}
try {
    $wordApplication = New-Object -ComObject Word.Application
    $wordApplication.Visible = $false
    $wordApplication.DisplayAlerts = 0
    $wordApplication.AutomationSecurity = 3
    $wordDocument = $wordApplication.Documents.Open($documentAbsolute, $false, $false)
    $null = [ConfigureWordWindow]::GetWindowThreadProcessId([IntPtr]$wordDocument.ActiveWindow.Hwnd, [ref]$wordProcessId)
    foreach ($contents in $wordDocument.TablesOfContents) { $contents.Update() }
    foreach ($figures in $wordDocument.TablesOfFigures) { $figures.Update() }
    $null = $wordDocument.Fields.Update()
    $wordDocument.Repaginate()
    foreach ($contents in $wordDocument.TablesOfContents) { $contents.Update() }
    foreach ($story in $wordDocument.StoryRanges) {
        $currentStory = $story
        while ($null -ne $currentStory) {
            $null = $currentStory.Fields.Update()
            $currentStory = $currentStory.NextStoryRange
        }
    }
    $wordDocument.Repaginate()
    foreach ($documentField in $wordDocument.Fields) {
        if ($documentField.Code.Text -match '\\c "Illustration"') {
            $documentField.Result.Text = 'No illustrations are used in this guide.'
        }
        if ($documentField.Code.Text -match '\\c "Equation"') {
            $documentField.Result.Text = 'No equations are used in this guide.'
        }
    }
    $wordDocument.Save()
    $wordDocument.ExportAsFixedFormat($pdfAbsolute, 17)
    Write-Output $pdfAbsolute
}
finally {
    if ($null -ne $wordDocument) { try { $wordDocument.Close(0) } catch { } }
    if ($null -ne $wordApplication) { try { $wordApplication.Quit() } catch { } }
    if ($null -ne $wordDocument) {
        $null = [System.Runtime.InteropServices.Marshal]::FinalReleaseComObject($wordDocument)
    }
    if ($null -ne $wordApplication) {
        $null = [System.Runtime.InteropServices.Marshal]::FinalReleaseComObject($wordApplication)
    }
    [GC]::Collect()
    [GC]::WaitForPendingFinalizers()
    # Only stop the background instance created by this export operation.
    if ($wordProcessId -ne 0 -and $wordProcessId -notin $existingWordIds) {
        Stop-Process -Id $wordProcessId -Force -ErrorAction SilentlyContinue
    }
}
& 'C:/Users/ignac/.cache/codex-runtimes/codex-primary-runtime/dependencies/python/python.exe' (Join-Path $PSScriptRoot 'qa_configure.py') $pdfAbsolute
if ($LASTEXITCODE -ne 0) { throw 'PDF rendering failed' }
