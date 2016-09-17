//
//  main.cpp
//  OpenGL4Test
//
//  Created by Rafael Radkowski on 5/28/15.
//  Copyright (c) 2015 -. All rights reserved.
//

// stl include
#include <iostream>
#include <string>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// glfw includes
#include <GLFW/glfw3.h>


// include local files
#include "controls.h"
#include "HCI557Common.h"
#include "CoordSystem.h"

// this line tells the compiler to use the namespace std.
// Each object, command without a namespace is assumed to be part of std. 
using namespace std;




static const string vs_string =
"#version 410 core                                                 \n"
"                                                                   \n"
"uniform mat4 projectionMatrix;                                    \n"
"uniform mat4 viewMatrix;                                           \n"
"uniform mat4 modelMatrix;                                          \n"
"in vec3 in_Position;                                               \n"
"                                                                   \n"
"in vec3 in_Color;                                                  \n"
"out vec3 pass_Color;                                               \n"
"                                                                  \n"
"void main(void)                                                   \n"
"{                                                                 \n"
"    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_Position, 1.0);  \n"
"    pass_Color = in_Color;                                         \n"
"}                                                                 \n";

// Fragment shader source code. This determines the colors in the fragment generated in the shader pipeline. In this case, it colors the inside of our triangle specified by our vertex shader.
static const string fs_string =
"#version 410 core                                                 \n"
"                                                                  \n"
"in vec3 pass_Color;                                                 \n"
"out vec4 color;                                                    \n"
"void main(void)                                                   \n"
"{                                                                 \n"
"    color = vec4(pass_Color, 1.0);                               \n"
"}                                                                 \n";




/// Camera control matrices
glm::mat4 projectionMatrix; // Store the projection matrix
glm::mat4 viewMatrix; // Store the view matrix
glm::mat4 modelMatrix; // Store the model matrix




					   // The handle to the window object
GLFWwindow*         window;


// Define some of the global variables we're using for this sample
GLuint program;







///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Fill this functions with your model code.

// USE THESE vertex array objects to define your objects
unsigned int vaoID[2];

unsigned int vboID[4];

/*!
ADD YOUR CODE TO CREATE THE TRIANGLE STRIP MODEL TO THIS FUNCTION
*/
unsigned int createTriangleStripModel(void)
{
	// use the vertex array object vaoID[0] for this model representation

	float *vertices= new float[129];
	float *colors= new float[129];
	for (int i = 0; i <= 126; i=i+3) {
		colors[i] = 0.101;
		colors[i+1] = 0.180;
		colors[i+2] = 0.858;
	}
		

	vertices[0] = -1; vertices[1] = 0; vertices[2] = 2;
	vertices[3] = -1; vertices[4] = 1; vertices[5] = 2;
	vertices[6] = 0; vertices[7] = 0; vertices[8] = 2;
	vertices[9] = 0; vertices[10] = 1; vertices[11] = 2;
	vertices[12] = 0; vertices[13] = 0; vertices[14] = 0;
	vertices[15] = 0; vertices[16] = 1; vertices[17] = 0;
	vertices[18] = -1; vertices[19] = 0; vertices[20] = 0;
	vertices[21] = -1; vertices[22] = 1; vertices[23] = 0;
	vertices[24] = -1; vertices[25] = 0; vertices[26] = 2;
	vertices[27] = -1; vertices[28] = 1; vertices[29] = 2;
	vertices[30] = 0; vertices[31] = 1; vertices[32] = 2;
	vertices[33] = -1; vertices[34] = 1; vertices[35] = 0;
	vertices[36] = 0; vertices[37] = 1; vertices[38] = 0;
	vertices[39] = -1; vertices[40] = 0; vertices[41] = 0;
	vertices[42] = 0; vertices[43] = 0; vertices[44] = 0;
	vertices[45] = -1; vertices[46] = 0; vertices[47] = 2;
	vertices[48] = 0; vertices[49] = 0; vertices[50] = 2;

	vertices[51] = 0; vertices[52] = 1; vertices[53] = 2;
	vertices[54] = 0; vertices[55] = 0; vertices[56] = 0;
	vertices[57] = -1; vertices[58] = 0; vertices[59] = -1;
	vertices[60] = 2; vertices[61] = 0; vertices[62] = -1;
	vertices[63] = 2; vertices[64] = 0; vertices[65] = 0;
	vertices[66] = -1; vertices[67] = 0; vertices[68] = 0;
	vertices[69] = 2; vertices[70] = 1; vertices[71] = 0;
	vertices[72] = -1; vertices[73] = 1; vertices[74] = 0;
	vertices[75] = 0; vertices[76] = 2; vertices[77] = 0;
	vertices[78] = -1; vertices[79] = 2; vertices[80] = 0;
	vertices[81] = 0; vertices[82] = 2; vertices[83] = -1;
	vertices[84] = -1; vertices[85] = 2; vertices[86] = -1;
	vertices[87] = 2; vertices[88] = 1; vertices[89] = -1;
	vertices[90] = -1; vertices[91] = 1; vertices[92] = -1;
	vertices[93] = 2; vertices[94] = 0; vertices[95] = -1;
	vertices[96] = -1; vertices[97] = 0; vertices[98] = -1;
	vertices[99] = -1; vertices[100] = 2; vertices[101] = -1;
	vertices[102] = -1; vertices[103] = 0; vertices[104] = 0;
	vertices[105] = -1; vertices[106] = 2; vertices[107] = 0;
	vertices[108] = -1; vertices[109] = 2; vertices[110] = -1;
	vertices[111] = 0; vertices[112] = 2; vertices[113] = 0;
	vertices[114] = 0; vertices[115] = 2; vertices[116] = -1;
	vertices[117] = 2; vertices[118] = 1; vertices[119] = 0;
	vertices[120] = 2; vertices[121] = 1; vertices[122] = -1;
	vertices[123] = 2; vertices[124] = 0; vertices[125] = 0;
	vertices[126] = 2; vertices[127] = 0; vertices[128] = -1;




	glGenVertexArrays(2, &vaoID[0]); // Create our Vertex Array Object
	glBindVertexArray(vaoID[0]);
	glGenBuffers(4, vboID); // Generate our Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboID[0]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, 129 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(0); // Disable our Vertex Array Object
								  //////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboID[1]); // Bind our second Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, 129 * sizeof(GLfloat), colors, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(1); // Enable the second vertex attribute array
	delete[] vertices; // Delete our vertices from memory
	delete[] colors;



	return 1;
}

/*!
ADD YOUR CODE TO CREATE A MODEL USING PRIMITIVES OF YOUR CHOISE TO THIS FUNCTION
*/
unsigned int createPolygonModel(void)
{
	// use the vertex array object vaoID[1] for this model representation
	float *vertices=new float[243];
	float *colors=new float[243];
	//	float vertices_polygen[15] = {0,0,2,-1,0,2,0,1,2,5,5,5,0,0,2};
	//	float colors_polygen[15] = {0.1,0.5,0.5,0.1,0.5,0.1, 0.5,0.1,0.5,0.5,0.1,0.5, 0.1,0.5,0.5};
	

	vertices[0] = 0; vertices[1] = 0; vertices[2] = 2;
	vertices[3] = 0; vertices[4] = 1; vertices[5] = 2;
	vertices[6] = 0; vertices[7] = 0; vertices[8] = 0;

	vertices[9] = 0; vertices[10] = 1; vertices[11] = 2;
	vertices[12] = 0; vertices[13] = 0; vertices[14] = 0;
	vertices[15] = 0; vertices[16] = 1; vertices[17] = 0;
	
	vertices[18] = 0; vertices[19] = 1; vertices[20] = 2;
	vertices[21] = 0; vertices[22] = 1; vertices[23] = 0;
	vertices[24] = -1; vertices[25] = 1; vertices[26] = 0;
	
	vertices[27] = -1; vertices[28] = 1; vertices[29] = 0;
	vertices[30] = 0; vertices[31] = 1; vertices[32] = 2;
	vertices[33] = -1; vertices[34] = 1; vertices[35] = 2;
	
	vertices[36] = 0; vertices[37] = 1; vertices[38] = 2;
	vertices[39] = -1; vertices[40] = 1; vertices[41] = 2;
	vertices[42] = 0; vertices[43] = 0; vertices[44] = 2;
	
	vertices[45] = 0; vertices[46] = 0; vertices[47] = 2;
	vertices[48] = -1; vertices[49] = 1; vertices[50] = 2;
	vertices[51] = -1; vertices[52] = 0; vertices[53] = 2;
	
	vertices[54] = -1; vertices[55] = 0; vertices[56] = 0;
	vertices[57] = -1; vertices[58] = 1; vertices[59] = 0;
	vertices[60] = -1; vertices[61] = 1; vertices[62] = 2;
	
	vertices[63] = -1; vertices[64] = 1; vertices[65] = 2;
	vertices[66] = -1; vertices[67] = 0; vertices[68] = 2;
	vertices[69] = -1; vertices[70] = 0; vertices[71] = 0;
	
	vertices[72] = 0; vertices[73] = 0; vertices[74] = 2;
	vertices[75] = -1; vertices[76] = 0; vertices[77] = 2;
	vertices[78] = -1; vertices[79] = 0; vertices[80] = 0;

	vertices[81] = 0; vertices[82] = 0; vertices[83] = 2;
	vertices[84] = -1; vertices[85] = 0; vertices[86] = 2;
	vertices[87] =-1; vertices[88] = 0; vertices[89] = 0;

	vertices[90] = 0; vertices[91] = 0; vertices[92] = 2;
	vertices[93] = -1; vertices[94] = 0; vertices[95] = 0;
	vertices[96] = 0; vertices[97] = 0; vertices[98] = 0;


	vertices[99] = 0; vertices[100] = 2; vertices[101] = 0;
	vertices[102] = 2; vertices[103] = 1; vertices[104] = 0;
	vertices[105] = 2; vertices[106] = 0; vertices[107] = 0;

	vertices[108] = 0; vertices[109] = 2; vertices[110] = 0;
	vertices[111] = 2; vertices[112] = 0; vertices[113] = 0;
	vertices[114] = -1; vertices[115] = 0; vertices[116] = 0;

	vertices[117] = 0; vertices[118] = 2; vertices[119] = 0;
	vertices[120] = -1; vertices[121] = 0; vertices[122] = 0;
	vertices[123] = -1; vertices[124] = 2; vertices[125] = 0;
	///
	vertices[126] = 0; vertices[127] = 2; vertices[128] = -1;
	vertices[129] = 2; vertices[130] = 1; vertices[131] = -1;
	vertices[132] = 2; vertices[133] = 0; vertices[134] = -1;

	vertices[135] = 0; vertices[136] = 2; vertices[137] = -1;
	vertices[138] = 2; vertices[139] = 0; vertices[140] = -1;
	vertices[141] = -1; vertices[142] = 0; vertices[143] =-1;
	
	vertices[144] = 0; vertices[145] = 2; vertices[146] = -1;
	vertices[147] = -1; vertices[148] = 0; vertices[149] = -1;
	vertices[150] = -1; vertices[151] = 2; vertices[152] = -1;
/////////////////////
	vertices[153] = 2; vertices[154] = 1; vertices[155] = -1;
	vertices[156] = 2; vertices[157] = 1; vertices[158] = 0;
	vertices[159] = 2; vertices[160] = 0; vertices[161] = 0;

	vertices[162] = 2; vertices[163] = 1; vertices[164] = -1;
	vertices[165] = 2; vertices[166] = 0; vertices[167] = -1;
	vertices[168] = 2; vertices[169] = 0; vertices[170] = 0;

	vertices[171] = 2; vertices[172] = 1; vertices[173] = -1;
	vertices[174] = 2; vertices[175] = 1; vertices[176] = 0;
	vertices[177] = 0; vertices[178] = 2; vertices[179] = -1;

	vertices[180] = 2; vertices[181] = 1; vertices[182] = 0;
	vertices[183] = 0; vertices[184] = 2; vertices[185] = -1;
	vertices[186] = 0; vertices[187] = 2; vertices[188] = 0;

	//
	vertices[189] = 0; vertices[190] = 2; vertices[191] = -1;
	vertices[192] = 0; vertices[193] = 2; vertices[194] = 0;
	vertices[195] = -1; vertices[196] = 2; vertices[197] = 0;

	vertices[198] = 0; vertices[199] = 2; vertices[200] = -1;
	vertices[201] = -1; vertices[202] = 2; vertices[203] = 0;
	vertices[204] = -1; vertices[205] = 2; vertices[206] = -1;

	vertices[207] = -1; vertices[208] = 2; vertices[209] = -1;
	vertices[210] = -1; vertices[211] = 2; vertices[212] = 0;
	vertices[213] = -1; vertices[214] = 0; vertices[215] = 0;

	vertices[216] = -1; vertices[217] = 2; vertices[218] = -1;
	vertices[219] = -1; vertices[220] = 0; vertices[221] = -1;
	vertices[222] = -1; vertices[223] = 0; vertices[224] = 0;

	vertices[225] = 2; vertices[226] = 0; vertices[227] = 0;
	vertices[228] = -1; vertices[229] = 0; vertices[230] = -1;
	vertices[231] = -1; vertices[232] = 0; vertices[233] = 0;

	vertices[234] = 2; vertices[235] = 0; vertices[236] = 0;
	vertices[237] = -1; vertices[238] = 0; vertices[239] = -1;
	vertices[240] = 2; vertices[241] = 0; vertices[242] = -1;

	for (int i = 0; i <= 240; i = i + 3) {
		colors[i] = 0.201;
		colors[i + 1] = 0.280;
		colors[i + 2] = 0.258;
	}
	//	vertices = &vertices_polygen[0];
	//	colors = &colors_polygen[0];

	//	glGenVertexArrays(2, &vaoID[1]); // Create our Vertex Array Object
	glBindVertexArray(vaoID[1]);
	//	glGenBuffers(2, vboID); // Generate our Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, vboID[2]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, 243 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(0); // Disable our Vertex Array Object
								  //////////////////
	glBindBuffer(GL_ARRAY_BUFFER, vboID[3]); // Bind our second Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, 243 * sizeof(GLfloat), colors, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW

	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
	glEnableVertexAttribArray(1); // Enable the second vertex attribute array
	delete[] vertices; // Delete our vertices from memory
	delete[] colors;
	return 1;
}



/*!
ADD YOUR CODE TO RENDER THE TRIANGLE STRIP MODEL TO THIS FUNCTION
*/
void renderTriangleStripModel(void)
{

	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(vaoID[0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0 ,129 );

	glBindVertexArray(0);// Unbind our Vertex Array Object
	


}



/*!
ADD YOUR CODE TO RENDER THE TRIANGLE STRIP MODEL TO THIS FUNCTION
*/
void renderPolygonModel(void)
{

	// Bind the buffer and switch it to an active buffer
	glBindVertexArray(vaoID[1]);

	glDrawArrays(GL_TRIANGLES, 0, 243);
	
	glBindVertexArray(0);// Unbind our Vertex Array Object

}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*!
This function creates the two models
*/
void setupScene(void) {

	createTriangleStripModel();
//	renderTriangleStripModel();
	createPolygonModel();
//	renderPolygonModel();
}




int main(int argc, const char * argv[])
{

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// Init glfw, create a window, and init glew

	// Init the GLFW Window
	window = initWindow();


	// Init the glew api
	initGlew();

	// Prepares some defaults
	CoordSystemRenderer* coordinate_system_renderer = new CoordSystemRenderer(10.0);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//// The Shader Program starts here

	// Vertex shader source code. This draws the vertices in our window. We have 3 vertices since we're drawing an triangle.
	// Each vertex is represented by a vector of size 4 (x, y, z, w) coordinates.
	static const string vertex_code = vs_string;
	static const char * vs_source = vertex_code.c_str();

	// Fragment shader source code. This determines the colors in the fragment generated in the shader pipeline. In this case, it colors the inside of our triangle specified by our vertex shader.
	static const string fragment_code = fs_string;
	static const char * fs_source = fragment_code.c_str();

	// This next section we'll generate the OpenGL program and attach the shaders to it so that we can render our triangle.
	program = glCreateProgram();

	// We create a shader with our fragment shader source code and compile it.
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);

	// We create a shader with our vertex shader source code and compile it.
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);

	// We'll attach our two compiled shaders to the OpenGL program.
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	// We'll specify that we want to use this program that we've attached the shaders to.
	glUseProgram(program);

	//// The Shader Program ends here
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/// IGNORE THE NEXT PART OF THIS CODE
	/// IGNORE THE NEXT PART OF THIS CODE
	/// IGNORE THE NEXT PART OF THIS CODE
	// It controls the virtual camera

	// Set up our green background color
	static const GLfloat clear_color[] = { 0.6f, 0.7f, 1.0f, 1.0f };
	static const GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };


	projectionMatrix = glm::perspective(1.1f, (float)800 / (float)600, 0.1f, 100.f);
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Create our model matrix which will halve the size of our model
	viewMatrix = glm::lookAt(glm::vec3(1.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	int projectionMatrixLocation = glGetUniformLocation(program, "projectionMatrix"); // Get the location of our projection matrix in the shader
	int viewMatrixLocation = glGetUniformLocation(program, "viewMatrix"); // Get the location of our view matrix in the shader
	int modelMatrixLocation = glGetUniformLocation(program, "modelMatrix"); // Get the location of our model matrix in the shader


	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]); // Send our projection matrix to the shader
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]); // Send our view matrix to the shader
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader


																			  //// The Shader Program ends here
																			  //// START TO READ AGAIN
																			  //// START TO READ AGAIN
																			  //// START TO READ AGAIN
																			  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	glBindAttribLocation(program, 0, "in_Position");
	glBindAttribLocation(program, 1, "in_Color");



	// this creates the scene
	setupScene();

	int i = 0;

	// Enable depth test
	// ignore this line, it allows us to keep the distance value after we proejct each object to a 2d canvas.
	glEnable(GL_DEPTH_TEST);

	// This is our render loop. As long as our window remains open (ESC is not pressed), we'll continue to render things.
	while (!glfwWindowShouldClose(window))
	{

		// Clear the entire buffer with our green color (sets the background to be green).
		glClearBufferfv(GL_COLOR, 0, clear_color);
		glClearBufferfv(GL_DEPTH, 0, clear_depth);

		// this draws the coordinate system
		coordinate_system_renderer->draw();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//// This generate the object
		// Enable the shader program
		glUseProgram(program);

		// this changes the camera location
		glm::mat4 rotated_view = viewMatrix * GetRotationMatrix();
		glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader


																				  // This moves the model to the right
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader


																				  // This line renders your triangle strip model
		renderTriangleStripModel();


		// This moves the model to the left
		modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -0.0f, 0.0f));
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]); // Send our model matrix to the shader

																				  // This line renders your Ppolygon model
		renderPolygonModel();


		// disable the shader program
		glUseProgram(0);


		//// This generate the object
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		// Swap the buffers so that what we drew will appear on the screen.
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	// delete the coordinate system object
	delete coordinate_system_renderer;

	// Program clean up when the window gets closed.
	glDeleteVertexArrays(2, vaoID);
	glDeleteProgram(program);
}

