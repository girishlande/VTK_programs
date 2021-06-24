This project shows how to use vtk along with QT widgets to create visualization application. 
This has only one VTK view and can be used to quickly test some VTK api 

PRECONDITION
===================
It makes use of cmake. You should have that installed. 
Using cmake you can build vtk and qt so their dlls are compatible with common generator(compiler). 

It assumes that you already have vtk and qt installed in your system. 
To build vtk, download stable release source code. it will have name something like vtk8.2

Copy this folder in D:\setups\VTK\
Rename folder to Source. 
Create folder named "build" at the same level of source. 
Create folder named "Install" at the same level of source. 

So tree should look something like
D:\setups\VTK\Source
D:\setups\VTK\Build
D:\setups\VTK\Install

Now, use cmake to configure and build this project. Make sure you check QT option and provide correct QT path. 
You can download and build qt using your compiler(generator).
NOTE: Please see vtkQTCmakeSettings.png in this folder for settings related to QT in cmake s

Once VTK is build and installed, you can build this template project. 


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
