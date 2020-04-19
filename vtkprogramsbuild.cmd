@echo off

setlocal 
set mode=release
setlocal ENABLEEXTENSIONS
Setlocal EnableDelayedExpansion

set arg=%1
if [%arg%]==[] (
  echo pass argument for which build to create [debug / release]
  exit /b 0
) 

if [%arg%]==[debug] (
  set mode=debug
  set destdir=build\debug
  goto :main
)

if [%arg%]==[release] (
  set mode=release
  set destdir=build\release
) else (
  echo only [debug/release] is valid mode 
  exit /b 0
)


:main
mkdir build

pushd build
cmake ../Source
cmake --build . --config %mode
popd
