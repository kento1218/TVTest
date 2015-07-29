function Make-Package($outdir, $name)
{
    mkdir -Force pkg
    $pkgpath = Join-Path $(pwd) "pkg\${name}.zip"
    
    pushd data
    &'7z' a $pkgpath . -r
    popd

    pushd doc
    &'7z' a $pkgpath License.txt TVTest.txt
    popd

    pushd $outdir
    &'7z' a $pkgpath TVTest.exe TVTest_Image.dll
    popd
}

Make-Package "src\Win32\${env:CONFIGURATION}" "tvtest-${env:APPVEYOR_BUILD_VERSION}-${env:CONFIGURATION}"
