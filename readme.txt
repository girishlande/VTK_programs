# VTK_programs
Precondition: To Run these programs you need visual studio an You will have to build VTK libraries using 
script vtkbuild.cmd It makes use of cmake . Once built you make sure that you build "Install" subproject of the 
vtk. THis will create binaries and includes in once place.  


These are some of my VTK programs that I tried while playing around with VTK and learning basics

You should use vtkprogramsbuild.cmd script to build all programs in one go.
You can use copydlls.cmd to copy vtk dlls in release folder so that programs can be run

If you use visual studio to run your programs then you don't have to do anything
If you run executable by double clicking on exe files then you should copy data folder inside build folder.

Note : Some of the programs do not compile as they are legacy programs 


// General concepts of VTK 

1. VTK program needs render window
2. Render window can be enquired from VTK widget 
3. Each Render window can have multiple renderers
4. Each renderer can draw in particular viewport of the window
5. You can have Interactor object which will decide how user interacts with the scene
6. You can have multiple actors in scene

// --------------------------------------------------------------------
// Possible to use different interaction styles across viewports?
// --------------------------------------------------------------------
// Assuming you’ve already created two different interactor styles and associated
// each with a different default renderer, 
// I would suggest you handle the MouseMove event, to detect when your active viewport changes with

vtkRenderWindowInteractor.GetInteractorStyle().GetDefaultRenderer()
and then switch the interactor style with
vtkRenderWindowInteractor.SetInteractorStyle()

