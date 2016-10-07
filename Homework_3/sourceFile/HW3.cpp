//
//  main_spotlight.cpp
//  HCI 557 Spotlight example
//
//  Created by Rafael Radkowski on 5/28/15.
//  Copyright (c) 2015 -. All rights reserved.
//
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")
#define _CRT_SECURE_NO_DEPRECATE
// stl include
#include <iostream>
#include <string>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


// glfw includes
#include <GLFW/glfw3.h>


// include local files
#include "controls.h"
#include "HCI557Common.h"
#include "CoordSystem.h"
#include "Sphere3D.h"
#include "GLAppearance.h"




using namespace std;


// The handle to the window object
GLFWwindow*         window;

// Define some of the global variables we're using for this sample
GLuint program;

/* A trackball to move and rotate the camera view */
extern Trackball trackball;



int main(int argc, const char * argv[])
{


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Init glfw, create a window, and init glew

	// Init the GLFW Window
	window = initWindow();


	// Init the glew api
	initGlew();



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Create some models

	// coordinate system
	CoordSystem* cs = new CoordSystem(40.0);


	// create an apperance object.
	GLAppearance* apperance_1 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* apperance_2 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* apperance_3 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* apperance_4 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");




	// The spotlight object
	GLSpotLightSource  light_source1;
	light_source1._lightPos = glm::vec4(100.0, 30.0, -5.0, 1.0);
	light_source1._ambient_intensity = 0.01;
	light_source1._specular_intensity = 1.0;
	light_source1._diffuse_intensity = 1.0;
	light_source1._attenuation_coeff = 0.0;
	light_source1._inner_cone_angle = 4.0;
	light_source1._outer_cone_angle = 6.8;
	light_source1._cone_direction = glm::vec3(-100.0, -23.0, 9); // this must be aligned with the object and light position.


	GLDirectLightSource  light_source2;
	light_source2._lightPos = glm::vec4(5000.0, -3000.0, 4000.0, 0.0);
	light_source2._ambient_intensity = 0.003;
	light_source2._specular_intensity = 0.5;
	light_source2._diffuse_intensity = 0.9;
	light_source2._attenuation_coeff = 0.1;
	// light_source2._cone_angle = 12.0; // in degree
	// light_source2._cone_direction = glm::vec3(-1.0, 0.0, 0.0); // this must be aligned with the object and light position.


	GLDirectLightSource  light_source3;
	light_source3._lightPos = glm::vec4(100.0, 20.0, -80.0, 0.0);
	light_source3._ambient_intensity = 0.01;
	light_source3._specular_intensity = 1.5;
	light_source3._diffuse_intensity = 1;
	light_source3._attenuation_coeff = 0.08;
//	light_source3._cone_angle = 180.0; // in degree
//	light_source3._cone_direction = glm::vec3(-100.0, -20.0, 120.0); // this must be aligned with the object and light position.

	GLDirectLightSource  light_source4;
	light_source4._lightPos = glm::vec4(100.0, 0.0, -60.0, 0.0);
	light_source4._ambient_intensity = 0.01;
	light_source4._specular_intensity = 0;
	light_source4._diffuse_intensity = 0.8;
	light_source4._attenuation_coeff = 0.08;

	GLSpotLightSource  light_source5;
	light_source5._lightPos = glm::vec4(80.0, 25.0, -5.0, 1.0);
	light_source5._ambient_intensity = 0.0;
	light_source5._specular_intensity = 10.0;
	light_source5._diffuse_intensity = 1.15;
	light_source5._attenuation_coeff = 0.0;
//	light_source5._inner_cone_angle = 5;
	light_source5._outer_cone_angle = 10; // in degree
	light_source5._cone_direction = glm::vec3(-160.0, -26.0, 85); // this must be aligned with the object and light position.

	// add the spot light to this apperance object
	apperance_1->addLightSource(light_source3);
	apperance_2->addLightSource(light_source4);
	apperance_3->addLightSource(light_source5);
//	apperance_4->addLightSource(light_source0);
	apperance_4->addLightSource(light_source1);
	apperance_4->addLightSource(light_source2);

	// Create a material object
	GLMaterial material_1,material_2,material_3,material_4;

	material_1._diffuse_material = glm::vec3(0.949, 0.043, 0.007);
	material_1._ambient_material = glm::vec3(0.949, 0.043, 0.007);
	material_1._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_1._shininess = 100;

	material_2._diffuse_material = glm::vec3(0.117, 0.196, 0.964);
	material_2._ambient_material = glm::vec3(0.117, 0.196, 0.964);
	material_2._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_2._shininess = 0.1;

	material_3._diffuse_material = glm::vec3(0.349, 0.968, 0.133);
	material_3._ambient_material = glm::vec3(0.349, 0.968, 0.133);
	material_3._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_3._shininess = 300;

	material_4._diffuse_material = glm::vec3(0.933, 0.898, 0.105);
	material_4._ambient_material = glm::vec3(0.933, 0.898, 0.105);
	material_4._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_4._shininess =100.0;

	// Add the material to the apperance object
	apperance_1->setMaterial(material_1);
	apperance_2->setMaterial(material_2);
	apperance_3->setMaterial(material_3);
	apperance_4->setMaterial(material_4);
	apperance_1->finalize();
	apperance_2->finalize();
	apperance_3->finalize();
	apperance_4->finalize();

	// create the sphere geometry
	GLSphere3D* sphere_1 = new GLSphere3D(0.0, 0.0, 90.0, 10.0, 100, 100);
	GLSphere3D* sphere_2 = new GLSphere3D(0.0, 0.0, 60.0, 10.0, 100, 100);
	GLSphere3D* sphere_3 = new GLSphere3D(0.0, 0.0, 30.0, 10.0, 100, 100);
	GLSphere3D* sphere_4 = new GLSphere3D(0.0, 0.0, 0.0, 10.0, 100, 100);
	sphere_1->setApperance(*apperance_1);
	sphere_1->init();
	sphere_2->setApperance(*apperance_2);
	sphere_2->init();
	sphere_3->setApperance(*apperance_3);
	sphere_3->init();
	sphere_4->setApperance(*apperance_4);
	sphere_4->init();



//	material_4._diffuse_material = glm::vec3(0.933, 0.898, 0.105);
//	material_4._ambient_material = glm::vec3(0.933, 0.898, 0.105);
//	apperance_4->updateMaterial();
//	light_source1._diffuse_intensity = 1.0;
//	apperance_4->updateLightSources();


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Main render loop

	// Set up our green background color
	static const GLfloat clear_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// This sets the camera to a new location
	// the first parameter is the eye position, the second the center location, and the third the up vector. 
	SetViewAsLookAt(glm::vec3(90.0f, 0.0f, 45.0f), glm::vec3(0.0f, 0.0f, 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));


	// Enable depth test
	// ignore this line, it allows us to keep the distance value after we proejct each object to a 2d canvas.
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	// sphere->enableNormalVectorRenderer();

	// This is our render loop. As long as our window remains open (ESC is not pressed), we'll continue to render things.
	while (!glfwWindowShouldClose(window))
	{

		// Clear the entire buffer with our green color (sets the background to be green).
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, clear_depth);


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// This renders the objects

		// Set the trackball locatiom
		SetTrackballLocation(trackball.getRotationMatrix());

		// draw the objects
		cs->draw();


		sphere_1->draw();
		sphere_2->draw();
		sphere_3->draw();
		sphere_4->draw();

		//// This renders the objects
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		// Swap the buffers so that what we drew will appear on the screen.
		glfwSwapBuffers(window);
		glfwPollEvents();

	}


	delete cs;


}

