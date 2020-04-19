//#include <QCoreApplication>
#include <vtkAutoInit.h>
 VTK_MODULE_INIT(vtkRenderingOpenGL2)

#include <vtkSmartPointer.h>

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkBrownianPoints.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <vtkVersion.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointData.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkXMLPolyDataReader.h>

 bool GetPointNormals(vtkPolyData* polydata)
 {
   std::cout << "In GetPointNormals: " << polydata->GetNumberOfPoints() << std::endl;
   std::cout << "Looking for point normals..." << std::endl;

   // Count points
   vtkIdType numPoints = polydata->GetNumberOfPoints();
   std::cout << "There are " << numPoints << " points." << std::endl;

   // Count triangles
   vtkIdType numPolys = polydata->GetNumberOfPolys();
   std::cout << "There are " << numPolys << " polys." << std::endl;

   ////////////////////////////////////////////////////////////////
   // Double normals in an array
   vtkDoubleArray* normalDataDouble =
     vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("Normals"));

   if(normalDataDouble)
     {
     int nc = normalDataDouble->GetNumberOfTuples();
     std::cout << "There are " << nc
             << " components in normalDataDouble" << std::endl;
     return true;
     }

   ////////////////////////////////////////////////////////////////
   // Double normals in an array
   vtkFloatArray* normalDataFloat =
     vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetArray("Normals"));

   if(normalDataFloat)
     {
     int nc = normalDataFloat->GetNumberOfTuples();
     std::cout << "There are " << nc
             << " components in normalDataFloat" << std::endl;
     return true;
     }

   ////////////////////////////////////////////////////////////////
   // Point normals
   vtkDoubleArray* normalsDouble =
     vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetNormals());

   if(normalsDouble)
     {
     std::cout << "There are " << normalsDouble->GetNumberOfComponents()
               << " components in normalsDouble" << std::endl;
     return true;
     }

   ////////////////////////////////////////////////////////////////
   // Point normals
   vtkFloatArray* normalsFloat =
     vtkFloatArray::SafeDownCast(polydata->GetPointData()->GetNormals());

   if(normalsFloat)
     {
     std::cout << "There are " << normalsFloat->GetNumberOfComponents()
               << " components in normalsFloat" << std::endl;
     return true;
     }

   /////////////////////////////////////////////////////////////////////
   // Generic type point normals
   vtkDataArray* normalsGeneric = polydata->GetPointData()->GetNormals(); //works
   if(normalsGeneric)
     {
     std::cout << "There are " << normalsGeneric->GetNumberOfTuples()
               << " normals in normalsGeneric" << std::endl;

     double testDouble[3];
     normalsGeneric->GetTuple(0, testDouble);

     std::cout << "Double: " << testDouble[0] << " "
               << testDouble[1] << " " << testDouble[2] << std::endl;

     // Can't do this:
     /*
     float testFloat[3];
     normalsGeneric->GetTuple(0, testFloat);

     std::cout << "Float: " << testFloat[0] << " "
               << testFloat[1] << " " << testFloat[2] << std::endl;
     */
     return true;
     }


   // If the function has not yet quit, there were none of these types of normals
   std::cout << "Normals not found!" << std::endl;
   return false;

 }


 void TestPointNormals(vtkPolyData* polydata)
 {
   std::cout << "In TestPointNormals: " << polydata->GetNumberOfPoints() << std::endl;
   // Try to read normals directly
   bool hasPointNormals = GetPointNormals(polydata);

   if(!hasPointNormals)
     {
     std::cout << "No point normals were found. Computing normals..." << std::endl;

     // Generate normals
     vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
 #if VTK_MAJOR_VERSION <= 5
     normalGenerator->SetInput(polydata);
 #else
     normalGenerator->SetInputData(polydata);
 #endif
     normalGenerator->ComputePointNormalsOn();
     normalGenerator->ComputeCellNormalsOff();
     normalGenerator->Update();

     polydata = normalGenerator->GetOutput();

     // Try to read normals again
     hasPointNormals = GetPointNormals(polydata);

     std::cout << "On the second try, has point normals? " << hasPointNormals << std::endl;

     }
   else
     {
     std::cout << "Point normals were found!" << std::endl;
     }
 }

int main(int, char *[])
{
  // Create a sphere
  vtkSmartPointer<vtkSphereSource> sphereSource =
    vtkSmartPointer<vtkSphereSource>::New();
  //sphereSource->SetPhiResolution(50);
  //sphereSource->SetThetaResolution(50);
  vtkPolyData* poly = sphereSource->GetOutput();
  TestPointNormals(poly);

  // Generate random vectors
  vtkMath::RandomSeed(100); // for testing
  vtkSmartPointer<vtkBrownianPoints> brownianPoints =
    vtkSmartPointer<vtkBrownianPoints>::New();
  brownianPoints->SetInputConnection(sphereSource->GetOutputPort());

  vtkSmartPointer<vtkArrowSource> arrowSource =
    vtkSmartPointer<vtkArrowSource>::New();

  vtkSmartPointer<vtkGlyph3D> glyph3D =
    vtkSmartPointer<vtkGlyph3D>::New();
  glyph3D->SetSourceConnection(arrowSource->GetOutputPort());
  glyph3D->SetInputConnection(brownianPoints->GetOutputPort());
  glyph3D->SetScaleFactor(.3);

  // Create a mapper and actor for sphere
  vtkSmartPointer<vtkPolyDataMapper> mapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(sphereSource->GetOutputPort());

  vtkSmartPointer<vtkActor> actor =
    vtkSmartPointer<vtkActor>::New();
  actor->GetProperty()->SetInterpolationToFlat();
  actor->SetMapper(mapper);

  // Create a mapper and actor for glyphs
  vtkSmartPointer<vtkPolyDataMapper> glyphMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  glyphMapper->SetInputConnection(glyph3D->GetOutputPort());

  vtkSmartPointer<vtkActor> glyphActor =
    vtkSmartPointer<vtkActor>::New();
  glyphActor->GetProperty()->SetColor(0.89,0.81,0.34); // banana
  glyphActor->SetMapper(glyphMapper);

  //Create a renderer, render window, and interactor
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =
    vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  renderWindowInteractor->SetRenderWindow(renderWindow);

  //Add the actor to the scene
  renderer->AddActor(actor);
  renderer->AddActor(glyphActor);

  // Create a nice view
  renderer->ResetCamera();
  renderer->GetActiveCamera()->Azimuth(30);
  renderer->GetActiveCamera()->Elevation(30);
  renderer->GetActiveCamera()->Dolly(1.4);
  renderer->ResetCameraClippingRange();

  renderer->SetBackground(.3, .4, .5);

  //Render and interact
  renderWindow->Render();
  renderWindowInteractor->Start();

  return EXIT_SUCCESS;
}
