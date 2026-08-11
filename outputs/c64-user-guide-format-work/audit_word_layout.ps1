param(
    [Parameter(Mandatory = $true)]
    [string]$DocumentPath
)

$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
$violations = @()
$headings = 0
$heading2Spacing = @()

try {
    $document = $word.Documents.Open($DocumentPath, $false, $true)
    $paragraphCount = $document.Paragraphs.Count

    for ($index = 1; $index -le $paragraphCount; $index++) {
        $paragraph = $document.Paragraphs.Item($index)
        $styleName = [string]$paragraph.Range.Style.NameLocal
        if ($styleName -notmatch '^(Title|Título)( [123])?$') {
            continue
        }

        $headings++
        $headingPage = [int]$paragraph.Range.Information(3)
        $keepWithNext = [int]$paragraph.Range.ParagraphFormat.KeepWithNext
        if ($styleName -match '(Heading 2|Título 2)$') {
            $heading2Spacing += [double]$paragraph.Range.ParagraphFormat.SpaceBefore
        }

        $nextIndex = $index + 1
        while ($nextIndex -le $paragraphCount) {
            $nextParagraph = $document.Paragraphs.Item($nextIndex)
            $nextText = ([string]$nextParagraph.Range.Text).Trim([char]13, [char]7, [char]32, [char]9)
            if ($nextText.Length -gt 0) {
                break
            }
            $nextIndex++
        }

        if ($nextIndex -gt $paragraphCount) {
            $violations += [pscustomobject]@{
                heading = ([string]$paragraph.Range.Text).Trim()
                reason = 'No following paragraph'
                page = $headingPage
            }
            continue
        }

        $nextPage = [int]$nextParagraph.Range.Information(3)
        if (($keepWithNext -eq 0) -or ($headingPage -ne $nextPage)) {
            $violations += [pscustomobject]@{
                heading = ([string]$paragraph.Range.Text).Trim()
                reason = if ($keepWithNext -eq 0) { 'KeepWithNext disabled' } else { 'Following paragraph begins on another page' }
                page = $headingPage
                followingPage = $nextPage
            }
        }
    }

    $pages = [int]$document.ComputeStatistics(2)
    $document.Close($false)
    [pscustomobject]@{
        pages = $pages
        headings = $headings
        violations = $violations
        heading2MinimumSpaceBefore = if ($heading2Spacing.Count) { ($heading2Spacing | Measure-Object -Minimum).Minimum } else { $null }
        heading2MaximumSpaceBefore = if ($heading2Spacing.Count) { ($heading2Spacing | Measure-Object -Maximum).Maximum } else { $null }
    } | ConvertTo-Json -Depth 5
}
finally {
    $word.Quit()
    [System.Runtime.InteropServices.Marshal]::FinalReleaseComObject($word) | Out-Null
    [gc]::Collect()
    [gc]::WaitForPendingFinalizers()
}
