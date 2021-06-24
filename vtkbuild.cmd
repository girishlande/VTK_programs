:: Download vtk from kitware website. 
:: Folder will be named something like vtk8.2  Copy this folder in some parent folder like D:/VTK 
:: Rename this folder to source
:: copy this script in parent folder. 
:: Start command prompt in admin mode and then execute this script. 

@echo off

setlocal 

set mode=release

set arg=%1
if [%arg%]==[] (
  echo pass argument for which build to create [debug / release]
  exit /b 0
) 

if [%arg%]==[debug] (
  set mode=debug
  goto :main
)

if [%arg%]==[release] (
  set mode=release
) else (
  echo only [debug/release] is valid mode 
  exit /b 0
)


:main

set startime=%date% %time%
echo Started: %date% %time%

mkdir build
pushd build

cmake ../source -DVTK_QT_VERSION:STRING=5 \
     -DVTK_Group_Qt:BOOL=ON \
	 -DModule_vtkGUISupportQtOpenGL:BOOL=ON \
	 -DModule_vtkImagingOpenGL2:BOOL=ON \
	 -DModule_vtkRenderingLICOpenGL2:BOOL=ON \
	 -DQt5_DIR:PATH=C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5 \
     -DQT_QMAKE_EXECUTABLE:PATH=C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/bin/qmake \
     -DCMAKE_PREFIX_PATH:PATH=C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5  \
	 -DQt5OpenGL_DIR:PATH=C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5OpenGL  \
     -DBUILD_SHARED_LIBS:BOOL=ON

cmake --build . --target ALL_BUILD --config %mode% --parallel 4
cmake --build . --target INSTALL --config %mode%
popd


echo Started: %starttime%
echo VTK build Completed: %date% %time%


endlocal