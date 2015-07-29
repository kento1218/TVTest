function Update-Revision($rev)
{
    $filename = '.\src\TVTest.h'
    $content = Get-Content $filename
    Set-Content $filename $($content -replace 'VERSION_REVISION\s[0-9]*', "VERSION_REVISION`t$rev")
}

function Fetch-BaseClasses($url)
{
    (New-Object Net.WebClient).DownloadFile($url, $(Join-Path $(pwd) 'BaseClasses.zip'))
    &'7z' e -y -osrc/BaseClasses BaseClasses.zip
}

Fetch-BaseClasses $env:BASE_CLASSES_URL
Update-Revision $env:APPVEYOR_BUILD_NUMBER
