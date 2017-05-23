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

int main(int, char *[])
{
    // ---------------------------------------------------
    // DO VTK business
    // ---------------------------------------------------
    vtkSmartPointer<vtkCylinderSource> s1 =
            vtkSmartPointer<vtkCylinderSource>::New();
    s1->SetHeight(5);
    s1->SetRadius(100);
    s1->SetResolution(50);
    s1->Update();

    vtkSmartPointer<vtkPolyDataMapper> sm1 =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    sm1->SetInputConnection(s1->GetOutputPort());
    vtkSmartPointer<vtkActor> sa1 =
            vtkSmartPointer<vtkActor>::New();
    sa1->SetMapper(sm1);

    // cylinder 2
    vtkSmartPointer<vtkCylinderSource> s2 =
            vtkSmartPointer<vtkCylinderSource>::New();
    s2->SetHeight(180);
    s2->SetRadius(40);
    s2->SetResolution(50);
    s2->Update();

    vtkSmartPointer<vtkPolyDataMapper> sm2 =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    sm2->SetInputConnection(s2->GetOutputPort());
    vtkSmartPointer<vtkActor> sa2 =
            vtkSmartPointer<vtkActor>::New();
    sa2->SetMapper(sm2);
    double col2[3]={1.0,1,0};
    sa2->GetProperty()->SetColor(col2);

    double red[3]={1.0,0,0};
    sa1->GetProperty()->SetColor(red);

    vtkSmartPointer<vtkRenderer> renderer =
            vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(sa1);

    renderer->AddActor(sa1);
    renderer->AddActor(sa2);

    //====================================
    vtkSmartPointer<vtkSphereSource> sphere =
            vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetRadius(100);
    sphere->SetThetaResolution(50);
    sphere->SetPhiResolution(50);

    vtkSmartPointer<vtkClipPolyData> clip =
            vtkSmartPointer<vtkClipPolyData>::New();
    clip->SetValue(0);
    clip->GenerateClippedOutputOn();
    clip->SetInputConnection(sphere->GetOutputPort());
    vtkSmartPointer<vtkPlane> plane =
            vtkSmartPointer<vtkPlane>::New();
    plane->SetNormal(0.0, 1.0, 0.0);
    clip->SetClipFunction (plane);

    vtkSmartPointer<vtkPolyDataMapper> polyDataMapper =
            vtkSmartPointer<vtkPolyDataMapper>::New();
    polyDataMapper->SetInputConnection(clip->GetOutputPort());
    vtkSmartPointer<vtkActor> actor =
            vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(polyDataMapper);
    double green[3]={105.0/255,105.0/255,1};
    actor->GetProperty()->SetColor(green);

    //Create a renderer, render window, and interactor

    vtkSmartPointer<vtkRenderWindow> renderWindow =
            vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    //Add the actor to the scene
    renderer->AddActor(actor);

    // Create a nice view
    renderer->ResetCamera();

    //Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
