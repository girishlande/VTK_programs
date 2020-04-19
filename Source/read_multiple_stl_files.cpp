// #include "stdafx.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkRenderingFreeType)

#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string wchar_t2string(const wchar_t *wchar)
{
	string str = "";
	int index = 0;
	while (wchar[index] != 0)
	{
		str += (char)wchar[index];
		++index;
	}
	return str;
}

wchar_t *string2wchar_t(const string &str)
{
	wchar_t wchar[260];
	int index = 0;
	while (index < str.size())
	{
		wchar[index] = (wchar_t)str[index];
		++index;
	}
	wchar[index] = 0;
	return wchar;
}

vector<string> listFilesInDirectory(string directoryName)
{
	WIN32_FIND_DATA FindFileData;
	wchar_t * FileName = string2wchar_t(directoryName);
	HANDLE hFind = FindFirstFile(FileName, &FindFileData);

	vector<string> listFileNames;
	listFileNames.push_back(wchar_t2string(FindFileData.cFileName));

	while (FindNextFile(hFind, &FindFileData))
		listFileNames.push_back(wchar_t2string(FindFileData.cFileName));

	return listFileNames;
}


int main(int argc, char *argv[])
{
	vtkSmartPointer<vtkRenderer> renderer =
		vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow =
		vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->SetSize(800, 800);
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);


	std::string inputFilename = "D:\\tempstl\\F130058_Hip_Femur_Left_5-13-16.stl";

	vector<string> listFiles;
	listFiles = listFilesInDirectory("D:\\tempstl\\*.stl");
	int cnt = 0;
	for each (string str in listFiles)
	{
		string path("D:\\tempstl\\");
		path.append(str);
		vtkSmartPointer<vtkSTLReader> reader =
			vtkSmartPointer<vtkSTLReader>::New();
		reader->SetFileName(path.c_str());
		reader->Update();

		// Visualize
		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(reader->GetOutputPort());

		vtkSmartPointer<vtkActor> actor =
			vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		switch (cnt%4) {
		case 0: actor->GetProperty()->SetColor(1, 0, 0); break;
		case 1: actor->GetProperty()->SetColor(1, 1, 0); break;
		case 2: actor->GetProperty()->SetColor(0, 1, 0); break;
		case 3: actor->GetProperty()->SetColor(0, 0, 1); break;
		}
		cnt++;
		renderer->AddActor(actor);
	}

	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}