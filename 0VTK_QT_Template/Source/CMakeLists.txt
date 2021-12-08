cmake_minimum_required (VERSION 3.1)
project(vtkQTDemo)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Change below path to point to correct installation 
set(VTK_DIR D:/setups/VTK/install/lib/cmake/vtk-8.2)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# find VTK
# Minimal set of modules: They point to their corrresponding cmake files in installed directory
find_package(VTK 8.2 COMPONENTS
  vtkFiltersModeling
  vtkGUISupportQt
  vtkIOImage
  vtkIOLegacy
  vtkImagingColor
  vtkImagingStatistics
  vtkImagingStencil
  vtkInteractionImage
  vtkInteractionStyle
  vtkInteractionWidgets
  vtkRenderingAnnotation
  vtkRenderingVolumeOpenGL2
  vtkIOGeometry
  vtkRenderingImage
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2
  REQUIRED
)

# find QT packages
find_package (Qt5
  COMPONENTS
    Core
    Gui
    Widgets
  REQUIRED
)

add_executable(vtkQTDemo
    mainwindow.ui
    mainwindow.cpp
    main.cpp
    resource.qrc
)

include_directories(${VTK_INCLUDE_DIRS})

target_link_libraries(vtkQTDemo
  PUBLIC
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets  
  PRIVATE
	${VTK_LIBRARIES}  
  )


