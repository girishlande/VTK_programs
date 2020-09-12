@echo off

setlocal 

set VTK build startime=%date% %time%
echo Started: %date% %time%

pushd build_%mode%

cmake ../source -DVTK_QT_VERSION:STRING=5 \
     -DQT_QMAKE_EXECUTABLE:PATH=C:/dev/Packages/hologic.qt.bin.5.12.2/bin/qmake \
     -DVTK_Group_Qt:BOOL=ON \
     -DCMAKE_PREFIX_PATH:PATH=C:/dev/Packages/hologic.qt.bin.5.12.2/lib/cmake/Qt5  \
     -DBUILD_SHARED_LIBS:BOOL=ON

cmake --build . --target ALL_BUILD --config %mode% -- -j 4

popd


echo Started: %starttime%
echo VTK build Completed: %date% %time%


endlocal