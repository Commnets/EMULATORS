param(
    [Parameter(Mandatory = $true)][string]$DocumentPath,
    [Parameter(Mandatory = $true)][string]$PdfPath,
    [Parameter(Mandatory = $true)][string]$StatusPath
)

$word = $null
$document = $null
try {
    $word = New-Object -ComObject Word.Application
    $word.Visible = $false
    $word.DisplayAlerts = 0
    $document = $word.Documents.Open($DocumentPath, $false, $false)
    foreach ($toc in $document.TablesOfContents) { $toc.Update() }
    $null = $document.Fields.Update()
    foreach ($section in $document.Sections) {
        foreach ($header in $section.Headers) {
            if ($header.Exists) { $null = $header.Range.Fields.Update() }
        }
        foreach ($footer in $section.Footers) {
            if ($footer.Exists) { $null = $footer.Range.Fields.Update() }
        }
    }
    $document.Save()
    $document.ExportAsFixedFormat($PdfPath, 17)
    [pscustomobject]@{ ok = $true; pdf = $PdfPath } |
        ConvertTo-Json | Set-Content -LiteralPath $StatusPath -Encoding UTF8
}
catch {
    [pscustomobject]@{ ok = $false; error = $_.Exception.Message } |
        ConvertTo-Json | Set-Content -LiteralPath $StatusPath -Encoding UTF8
}
finally {
    if ($null -ne $document) { try { $document.Close($false) } catch { } }
    if ($null -ne $word) { try { $word.Quit() } catch { } }
}
