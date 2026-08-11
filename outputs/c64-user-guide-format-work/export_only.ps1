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
    $word.Options.UpdateFieldsAtPrint = $false
    $document = $word.Documents.Open($DocumentPath, $false, $true)
    $document.ExportAsFixedFormat($PdfPath, 17)
    $pages = [int]$document.ComputeStatistics(2)
    $document.Close($false)
    $document = $null
    [pscustomobject]@{ ok = $true; pages = $pages; pdf = $PdfPath } |
        ConvertTo-Json | Set-Content -LiteralPath $StatusPath -Encoding UTF8
}
catch {
    [pscustomobject]@{ ok = $false; error = $_.Exception.Message } |
        ConvertTo-Json | Set-Content -LiteralPath $StatusPath -Encoding UTF8
}
finally {
    if ($null -ne $document) { $document.Close($false) }
    if ($null -ne $word) { $word.Quit() }
}
