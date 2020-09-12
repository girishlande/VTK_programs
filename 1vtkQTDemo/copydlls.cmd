
if [%1]==[debug] (
  set destdir=build\debug
  set sourceFolder=Debug
) else (
  set destdir=build\release
  set sourceFolder=Release
)
echo Copying dlls for %destdir%

robocopy /NFL /NDL /NJH /NJS /nc /ns /np C:\dev\Packages\hologic.qt.bin.5.12.2\bin\ %destdir% *.dll
robocopy /NFL /NDL /NJH /NJS /nc /ns /np /mir "C:\dev\Packages\hologic.qt.bin.5.12.2\plugins\platforms" %destdir%\platforms
robocopy /NFL /NDL /NJH /NJS /nc /ns /np D:\repos\vtk_release\build_release\bin\%sourceFolder%\ %destdir% *.dll
