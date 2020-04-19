cmake_minimum_required(VERSION 3.10)

project(vtkprograms)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(VTK 8.2 COMPONENTS
  vtkFiltersModeling
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
  REQUIRED
)

include_directories(${VTK_INCLUDE_DIRS})

add_executable(add_key_points_different_color_planes add_key_points_different_color_planes.cpp )

add_executable(add_key_points_on_geometry add_key_points_on_geometry.cpp )

add_executable(add_key_points_on_pelvic_girdle add_key_points_on_pelvic_girdle.cpp )

add_executable(add_key_points_on_pelvic_girdle_and_define_3_planes add_key_points_on_pelvic_girdle_and_define_3_planes.cpp )

add_executable(add_key_points_on_pelvic_girdle_calculate__ add_key_points_on_pelvic_girdle_calculate__.cpp )

add_executable(align_tool_to_random_vector align_tool_to_random_vector.cpp )

add_executable(arrow_at_all_points_of_sphere arrow_at_all_points_of_sphere.cpp )

add_executable(boundary_tracing_using_blobs_and_polylines boundary_tracing_using_blobs_and_polylines.cpp )

add_executable(BoxDemo BoxDemo.cpp )

add_executable(clipping_sphere clipping_sphere.cpp )

add_executable(clip_sphere_using_keys clip_sphere_using_keys.cpp )

add_executable(clip_sphere_using_slider clip_sphere_using_slider.cpp )

add_executable(cone_nonuniform_scale cone_nonuniform_scale.cpp )

add_executable(coordinate_system coordinate_system.cpp )

add_executable(coordinate_system_draw coordinate_system_draw.cpp )

add_executable(create_arrow_on_snapped_point_with_surface_normal create_arrow_on_snapped_point_with_surface_normal.cpp )

add_executable(cutview_solid_using_3_planes cutview_solid_using_3_planes.cpp )

add_executable(cutview_solid_using_box cutview_solid_using_box.cpp )

add_executable(DICOMImageInteractor DICOMImageInteractor.cpp )

add_executable(DICOM_reader_works_on_vtk5 DICOM_reader_works_on_vtk5.cpp )

add_executable(DICOM_reader_works_on_vtk7 DICOM_reader_works_on_vtk7.cpp )

add_executable(DICOM_read_single_file_works_vtk5 DICOM_read_single_file_works_vtk5.cpp )

add_executable(display_dicom_volume display_dicom_volume.cpp )

add_executable(dragging_inserting dragging_inserting.cpp )

add_executable(dragging_point dragging_point.cpp )

add_executable(drawRectangle drawRectangle.cpp )

add_executable(draw_border draw_border.cpp )

add_executable(draw_selected_triangle_on_geometry draw_selected_triangle_on_geometry.cpp )

add_executable(glyph_3d_sphere_sphere glyph_3d_sphere_sphere.cpp )

add_executable(half_sphere half_sphere.cpp )

add_executable(Hello Hello.cpp )

add_executable(ImageData_to_VolumeData ImageData_to_VolumeData.cpp )

add_executable(ImageData_to_VolumeData_sphere ImageData_to_VolumeData_sphere.cpp )

add_executable(imagedata_volumedata_clipping imagedata_volumedata_clipping.cpp )

add_executable(labeltext labeltext.cpp )

add_executable(Move_glyph_sphere Move_glyph_sphere.cpp )

add_executable(move_objects_by_middle_button move_objects_by_middle_button.cpp )

add_executable(normals_at_Each_point_on_sphere normals_at_Each_point_on_sphere.cpp )

add_executable(normal_at_clicked_point_on_obj_file normal_at_clicked_point_on_obj_file.cpp )

add_executable(orientArrow_to_desired_direction orientArrow_to_desired_direction.cpp )

add_executable(painting painting.cpp )

add_executable(painting_stl_mesh painting_stl_mesh.cpp )

add_executable(painting_stl_mesh1 painting_stl_mesh1.cpp )

add_executable(painting_stl_mesh_good painting_stl_mesh_good.cpp )

add_executable(paint_within_device_radius paint_within_device_radius.cpp )

add_executable(paint_within_radius_bounds paint_within_radius_bounds.cpp )

add_executable(pelvic_girdle_round_planes pelvic_girdle_round_planes.cpp )

add_executable(pickable_off pickable_off.cpp )

add_executable(Pick_point_and_closest_point Pick_point_and_closest_point.cpp )

add_executable(qt_drag_point qt_drag_point.cpp )

add_executable(qt_pick_point_and_show qt_pick_point_and_show.cpp )

add_executable(read_multiple_stl_files read_multiple_stl_files.cpp )

add_executable(Reamer2D Reamer2D.cpp )

add_executable(reamer_2D_rotating reamer_2D_rotating.cpp )

add_executable(reamer_tool reamer_tool.cpp )

add_executable(reamer_tool_2d reamer_tool_2d.cpp )

add_executable(reamer_tool_2d_along_random_plane reamer_tool_2d_along_random_plane.cpp )

add_executable(reamer_tool_2d_v1 reamer_tool_2d_v1.cpp )

add_executable(reslice reslice.cpp )

add_executable(Rotate_points Rotate_points.cpp )

add_executable(rotate_points_with_random_axis rotate_points_with_random_axis.cpp )

add_executable(segmentation_automatic segmentation_automatic.cpp )

add_executable(select_triangles_on_geometry select_triangles_on_geometry.cpp )

add_executable(set set.cpp )

add_executable(show_normal_direction_on_hover show_normal_direction_on_hover.cpp )

add_executable(SixPlanesBox SixPlanesBox.cpp )

add_executable(Slider2D Slider2D.cpp )

add_executable(STL_read_stl_file STL_read_stl_file.cpp )

add_executable(surface_normals_using_glyph surface_normals_using_glyph.cpp )

add_executable(test_program test_program.cpp )

add_executable(verticalText verticalText.cpp )


target_link_libraries(add_key_points_different_color_planes  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(add_key_points_on_geometry  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(add_key_points_on_pelvic_girdle  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(add_key_points_on_pelvic_girdle_and_define_3_planes  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(add_key_points_on_pelvic_girdle_calculate__  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(align_tool_to_random_vector  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(arrow_at_all_points_of_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(boundary_tracing_using_blobs_and_polylines  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(BoxDemo  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(clipping_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(clip_sphere_using_keys  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(clip_sphere_using_slider  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(cone_nonuniform_scale  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(coordinate_system  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(coordinate_system_draw  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(create_arrow_on_snapped_point_with_surface_normal  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(cutview_solid_using_3_planes  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(cutview_solid_using_box  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(DICOMImageInteractor  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(DICOM_reader_works_on_vtk5  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(DICOM_reader_works_on_vtk7  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(DICOM_read_single_file_works_vtk5  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(display_dicom_volume  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(dragging_inserting  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(dragging_point  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(drawRectangle  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(draw_border  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(draw_selected_triangle_on_geometry  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(glyph_3d_sphere_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(half_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(Hello  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(ImageData_to_VolumeData  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(ImageData_to_VolumeData_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(imagedata_volumedata_clipping  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(labeltext  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(Move_glyph_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(move_objects_by_middle_button  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(normals_at_Each_point_on_sphere  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(normal_at_clicked_point_on_obj_file  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(orientArrow_to_desired_direction  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(painting  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(painting_stl_mesh  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(painting_stl_mesh1  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(painting_stl_mesh_good  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(paint_within_device_radius  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(paint_within_radius_bounds  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(pelvic_girdle_round_planes  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(pickable_off  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(Pick_point_and_closest_point  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(qt_drag_point  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(qt_pick_point_and_show  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(read_multiple_stl_files  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(Reamer2D  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(reamer_2D_rotating  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(reamer_tool  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(reamer_tool_2d  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(reamer_tool_2d_along_random_plane  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(reamer_tool_2d_v1  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(reslice  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(Rotate_points  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(rotate_points_with_random_axis  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(segmentation_automatic  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(select_triangles_on_geometry  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(set  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(show_normal_direction_on_hover  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(SixPlanesBox  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(Slider2D  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(STL_read_stl_file  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(surface_normals_using_glyph  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(test_program  PUBLIC ${VTK_LIBRARIES}  )

target_link_libraries(verticalText  PUBLIC ${VTK_LIBRARIES}  )

