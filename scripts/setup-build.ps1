#! /usr/bin/env pwsh

Param(
    [parameter(Mandatory=$true,Position=0)][String] ${target},
    [parameter(Mandatory=$true,Position=1)][String] ${buildType}
)

$project=(Get-Location)

if (${target} -eq "windows") {
	$toolchain="-DCMAKE_TOOLCHAIN_FILE=cmake/Modules/Mingw.cmake"
} elseif (${target} -eq "gba") {
	$toolchain="-DCMAKE_TOOLCHAIN_FILE=cmake/Modules/GBA.cmake -DNOSTALGIA_BUILD_TYPE=GBA -DOX_USE_STDLIB=OFF"
}

if (${buildType} -eq "asan") {
	$buildTypeArgs="-DUSE_ASAN=ON -DCMAKE_BUILD_TYPE=Debug"
} elseif (${buildType} -eq "debug") {
	$buildTypeArgs="-DCMAKE_BUILD_TYPE=Debug"
} elseif (${buildType} -eq "release") {
	$buildTypeArgs="-DCMAKE_BUILD_TYPE=Release"
}

$buildDir="build/${target}-${buildType}"
$distDir="../../dist/${target}-${buildType}"

New-Item -ItemType Directory -Path $buildDir | Out-Null
Push-Location $buildDir
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON `
      -DCMAKE_INSTALL_PREFIX="$distDir" `
      -DCMAKE_INSTALL_RPATH="$project/dist/${target}-${buildType}/lib/nostalgia" `
      -DNOSTALGIA_IDE_BUILD=OFF `
      $buildTool `
      $buildTypeArgs `
      $toolchain `
      $project
Pop-Location

rm -f build/current dist/current
New-Item -ItemType Directory -Path dist | Out-Null
ln -s ${target}-${buildType} build/current
ln -s ${target}-${buildType} dist/current
