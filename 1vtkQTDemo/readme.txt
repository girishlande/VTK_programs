This project shows how to use vtk along with QT widgets to create visualization application. 

PRECONDITION
===================
It makes use of cmake. You should have that installed. 
Using cmake you can build vtk and qt so their dlls are compatible with common generator(compiler). 

[Note: to build vtk you can use vtkbuild.cmd script. 
This makes use of cmake command line utility to build vtk in debug/release
mode. you can fire command set mode=debug or set mode=release before firing 
vtkbuild.cmd command.]
Though I have noticed that there is limitation of this script.
It doesn't compile or build QT related libraries and hence it cant be used as it is. 
Hence you should use cmake-gui for building this binaries

It assumes that you already have vtk and qt installed in your system. 
To build vtk, download stable release source code. create install folder in source folder. 
Use cmake to configure and build this project. Make sure you check QT option and provide correct QT path. 
You can download and build qt using your compiler(generator).
NOTE: Please see vtkQTCmakeSettings.png in this folder for settings related to QT in cmake s

Now try to build this project.

1. Open cmakeLists.txt and make sure that 
VTK_DIR is set to correct path 
open copydll.cmd and make sure that source path is correctly pointing to vtk and qt
directories to copy their dlls

2.run buildProject.cmd
In ideal env this script should do everything. that is 
> create build folder
> use cmake to populate build folder with correct project file. 
> build project file based on the debug / release setting inside script. 

3. run.cmd will launch executable. 
   open dicom files and stl files from models folder. 

This project has capability to open dicom file for viewing
also it can render stl/obj files 
