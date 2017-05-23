#include <QCoreApplication>
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)

#include <QGridLayout.h>
#include <QDir.h>
#include <QFileDialog.h>

#include <vtkPlane.h>
#include <vtkClipPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCylinderSource.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkProperty.h>
#include <vtkSphere.h>
#include <vtkSphereSource.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLineSource.h>
#include <vtkMath.h>
#include <vtkTransform.h>

#define vtkNew(type, name)\
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()
#define RADIUS 20.0

// ---------------------------------------------------
// Orient object with vector define by given 2 points
//
//                   o dest
//                  /
//                 /
//                /
//               /
//              o source
//
//
//   alignment : source->dest
//
// ---------------------------------------------------

void OrientObject(double pt1[],double pt2[]) {

    // --------------------------------------------------
    // Create a renderer, render window, and interactor
    // --------------------------------------------------
    vtkNew(vtkRenderer,renderer);
    vtkNew(vtkRenderWindow,renderWindow);
    renderWindow->AddRenderer(renderer);
    vtkNew(vtkRenderWindowInteractor,renderWindowInteractor);
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkNew(vtkInteractorStyleTrackballCamera,style);
    renderWindowInteractor->SetInteractorStyle(style);

    // Calculate Trajectory vector between pt1 and pt2
    double	trajectory[3];
    vtkMath::Subtract(pt2,pt1,trajectory);
    double negative_dir[3];
    vtkMath::Subtract(pt1,pt2,negative_dir);
    vtkMath::Normalize(negative_dir);
    vtkMath::MultiplyScalar(negative_dir,RADIUS);
    double tip_center[3];
    vtkMath::Add(pt2,negative_dir,tip_center);

    // Calculate axis of rotation . Take cross product of (pt1->pt2) and Z axis
    // Output will be perpendicular to (pt1->pt2) vector
    double z_axis[3] = { 0.0, 1.0, 0.0 };
    double rotation_axis[3];
    vtkMath::Cross(z_axis, trajectory, rotation_axis);

    // Calculate rotation angle between Z axis and (pt1 -> pt2) vector
    double dot_product = vtkMath::Dot(z_axis, trajectory);
    double a_length = vtkMath::Norm(z_axis);
    double b_length = vtkMath::Norm(trajectory);
    double cos_angle = dot_product / (a_length*b_length);
    double acos_angle = acos(cos_angle);
    double degree_angle = (acos_angle * 180) / vtkMath::Pi();

    // Calculate transformation matrix. Translate from (0,0,0) to ablation point
    vtkNew(vtkTransform, xform);
    xform->Translate(tip_center);

    // Rotate Against rotation_axis with calculated angle to match z axis (0,0,1) with Trajectory axis
    vtkNew(vtkTransform, rotation);
    rotation->RotateWXYZ(degree_angle, rotation_axis);
    xform->Concatenate(rotation);

    vtkNew(vtkLineSource,lineSource);
    lineSource->SetPoint1(pt1);
    lineSource->SetPoint2(pt2);
    lineSource->Update();

    // Visualize
    vtkNew(vtkPolyDataMapper,mapper);
    mapper->SetInputConnection(lineSource->GetOutputPort());
    vtkNew(vtkActor,actor);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetLineWidth(4);

    double blob_radius = RADIUS * 0.1;

    // Add blobs indicating source and destination points
    bool add_blob=true;
    if (add_blob) {
        vtkNew(vtkSphereSource,sphere1);
        sphere1->SetCenter(pt1);
        sphere1->SetRadius(blob_radius);
        sphere1->SetThetaResolution(50);
        sphere1->SetPhiResolution(50);
        vtkNew(vtkPolyDataMapper,blobsm1);
        blobsm1->SetInputConnection(sphere1->GetOutputPort());
        vtkNew(vtkActor,blob1);
        blob1->SetMapper(blobsm1);
        blob1->GetProperty()->SetColor(0,0,1);  // blue
        renderer->AddActor(blob1);

        vtkNew(vtkSphereSource,sphere2);
        sphere2->SetCenter(pt2);
        sphere2->SetRadius(blob_radius);
        sphere2->SetThetaResolution(50);
        sphere2->SetPhiResolution(50);
        vtkNew(vtkPolyDataMapper,blobsm2);
        blobsm2->SetInputConnection(sphere2->GetOutputPort());
        vtkNew(vtkActor,blob2);
        blob2->SetMapper(blobsm2);
        blob2->GetProperty()->SetColor(0,1,0);  // green
        renderer->AddActor(blob2);
    }

    // ---------------
    // Add the tool
    // ---------------

    //  cylinder 1 : lid to close half sphere
    double lid_radius = RADIUS;
    double lid_height = lid_radius*0.05;
    if (lid_height<1.0) {
        lid_height = 1.0;
    }
    vtkNew(vtkCylinderSource,s1);
    s1->SetHeight(lid_height);
    s1->SetRadius(lid_radius);
    s1->SetResolution(50);
    s1->Update();

    vtkNew(vtkPolyDataMapper,sm1);
    sm1->SetInputConnection(s1->GetOutputPort());
    vtkNew(vtkActor,sa1);
    sa1->SetMapper(sm1);

    // cylinder 2  as a handle to the lid
    double handle_height = lid_radius*1.8;
    double handle_radius = lid_radius*0.4;
    vtkNew(vtkCylinderSource,s2);
    s2->SetHeight(handle_height);
    s2->SetRadius(handle_radius);
    s2->SetResolution(50);
    s2->Update();

    vtkNew(vtkPolyDataMapper,sm2);
    sm2->SetInputConnection(s2->GetOutputPort());
    vtkNew(vtkActor,sa2);
    sa2->SetMapper(sm2);
    double col2[3]={1.0,1,0};
    sa2->GetProperty()->SetColor(col2);

    double red[3]={1.0,0,0};
    sa1->GetProperty()->SetColor(red);

    vtkNew(vtkSphereSource,sphere);
    // Half sphere as a cap of the tool
    sphere->SetRadius(RADIUS);
    sphere->SetThetaResolution(50);
    sphere->SetPhiResolution(50);

    vtkNew(vtkClipPolyData,clip);
    clip->SetValue(0);
    clip->GenerateClippedOutputOn();
    clip->SetInputConnection(sphere->GetOutputPort());
    vtkNew(vtkPlane,plane);
    plane->SetNormal(0.0, 1.0, 0.0);
    clip->SetClipFunction (plane);

    vtkNew(vtkPolyDataMapper,polyDataMapper);
    polyDataMapper->SetInputConnection(clip->GetOutputPort());

    vtkNew(vtkActor,sa3);
    sa3->SetMapper(polyDataMapper);
    double green[3]={105.0/255,105.0/255,1};
    sa3->GetProperty()->SetColor(green);



    //Add the actor to the scene
    double tool_opacity = 1.99;
    sa1->SetUserTransform(xform);
    sa2->SetUserTransform(xform);
    sa3->SetUserTransform(xform);
    sa1->GetProperty()->SetOpacity(tool_opacity);
    sa2->GetProperty()->SetOpacity(tool_opacity);
    sa3->GetProperty()->SetOpacity(tool_opacity);

    renderer->AddActor(sa1);
    renderer->AddActor(sa2);
    renderer->AddActor(sa3);
    renderer->AddActor(actor);

    // Create a nice view
    renderer->ResetCamera();

    //Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();
}

int main(int, char *[])
{
    // --------------------------
    // Create a random line
    // --------------------------
    double pt1[3]={100,100,100};
    double pt2[3]={100,110,100};

    OrientObject(pt1,pt2);

    return EXIT_SUCCESS;
}
