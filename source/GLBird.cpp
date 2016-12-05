//
//  GLBird.cpp
//  
//
//  Created by Pei Zhang on 11/15/16.
//	Refer from Dr. Rafael Radkowski GLObject.cpp, GLObjectObj.cpp
//


#include "GLBird.h"
#include <algorithm>


#pragma warning (disable : 4996)//disable ffopen warning

// a pointer for the texture data. 


GLBird::GLBird(string filename, string texturename):
_file_and_path(filename), _texture_name(texturename), str1("../../data/textures/deer_texture.bmp"), str2("")
{
    
    _file_ok = false;
    _file_ok =load_obj(filename.c_str(), _vertices, _normals,_textures, _elements);
   
}

GLBird::GLBird()
{

}


GLBird::~GLBird()
{

}



/*!
 Init the geometry object
 */
void GLBird::init(void)
{
    if(_file_ok)
    {
        initShader();
        initVBO();
    }
}

bool GLBird::extractNextFace1(string& in, string& out, int& pointIdx0, int& pointIdx1, int& pointIdx2 )
{
    GLuint a,b,c;
    
    
    // remove the first space
    int idv1 = in.find_first_of(" ");
    string v1(in.substr(idv1+1, in.length()-idv1)); // remove it
    
    sscanf(v1.c_str(), " %d %d %d", &a, &b, &c);
    
    return true;
}




bool GLBird::extractNextFace3(string& in, string& out, int& pointIdx, int& texIdx, int& normalIdx )
{
    GLuint a,b,c;
    
    // first
    int idv1 = in.find_first_of(" ");
    string v1(in.substr(idv1+1, in.length()-idv1));
    
    sscanf(v1.c_str(), " %d/%d/%d", &a, &b, &c);
    
    int idv2 = v1.find_first_of(" ");
    if(idv2 != -1)
    {
        out = v1.substr(idv2, v1.length()-idv2); // with blank
    }
    else
    {
        out = "";
    }
    
    /*
    int idx = v1.find_first_of("/");
    istringstream c1(v1.substr(0,idx));
    
    
    c1 >> a;
    
    // second
    string v2 = v1.substr(idx+1, v1.length()-idx);
    
    int idv2 = v2.find_first_of("/");
    istringstream c2(v2.substr(0,idv2));
    c2 >> b;
    
    
    // third
    string v3 = v2.substr(idv2+1, v2.length()-idv2);
    
    int idv3 = v3.find_first_of(" ");
    istringstream c3(v3.substr(0,idv3));
    
    c3 >> c;
    
    if(idv3 != -1)
        out = v3.substr(idv3, in.length()-idv3); // with blank
    else
        out = "";
     */

    pointIdx = a;
    texIdx = b;
    normalIdx = c;
    
    return true;

}

/*!
Verifies wheterh a file [name] exits
@param name - the path and the name of the file.
*/
bool GLBird::GLSearchFile(const std::string& name, string& new_name)
{
	new_name = "";

	bool exits = false;

	ifstream f(name.c_str());
	if (f.good()) {
		f.close();
		new_name = name;
		exits = true;
		return exits;
	}
	else {
		f.close();
		exits = false;
	}



	int idx = name.find_first_of("/");
	string newstring = name.substr(idx + 1, name.length() - 3);


	ifstream f2(newstring.c_str());
	if (f2.good()) {
		f2.close();
		exits = true;
	}
	else {
		f2.close();
		exits = false;
	}


	if (exits)
	{
		new_name = newstring;
	}


	return exits;
}

/*!
Verifies wheterh a file [name] exits
@param name - the path and the name of the file.
*/
bool GLSearchTexture(const std::string& name, string& new_name)
{
	new_name = "";

	bool exits = false;

	ifstream f(name.c_str());
	if (f.good()) {
		f.close();
		new_name = name;
		exits = true;
		return exits;
	}
	else {
		f.close();
		exits = false;
	}



	int idx = name.find_first_of("/");
	string newstring = name.substr(idx + 1, name.length() - 3);


	ifstream f2(newstring.c_str());
	if (f2.good()) {
		f2.close();
		exits = true;
	}
	else {
		f2.close();
		exits = false;
	}


	if (exits)
	{
		new_name = newstring;
	}


	return exits;
}

/*!
Load a bitmap image from a file
@param path_and_file - the path and filename
@param data - a pointer to return the data. Note, the data object should be deleted once the data is not required anymore.
@param channels - reference to an integer to keep the channels.
@param width, height, - reference to integers to return the dimensions in pixels.
*/
unsigned char* GLloadBitmapFile(string path_and_file, unsigned int& channels, unsigned int& width, unsigned int& height)
{

	// Check whether we load a bitmap file

	int idx = path_and_file.find_last_of(".");
	string suffix = path_and_file.substr(idx + 1, 3);

	if (suffix.compare("bmp") != 0) {
		cerr << "[ERROR] Filetype " << suffix << " is currently not supported. This example only support bitmap files. " << endl;
		return NULL;
	}

	string new_path_and_file;
	GLSearchTexture(path_and_file, new_path_and_file);


	//**********************************************************************************************
	// Loads the file content

	channels = 3;
	unsigned char* data;
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	//unsigned int width, height;
	unsigned int imageSize;

	// This opens a file
	FILE * file;
	file = fopen(new_path_and_file.c_str(), "rb");

	if (file == NULL) return 0;

	// This reads the header of the file and checks the length.
	if (fread(header, 1, 54, file) != 54)
	{
		// If not 54 bytes read, this is not a bmp.
		// Only a bmp has a header of length 54
		printf("Not a correct BMP file\n");
		return NULL;
	}

	// Read the start position of the data, the size, the width, and height.
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);
	channels = imageSize / (width * height);

	// Create memory for this texture
	data = (unsigned char *)malloc(width * height * channels);

	// Read the data from a file.
	fread(data, width * height * channels, 1, file);

	// Release the file.
	fclose(file);


	return data;

}


bool GLBird::load_obj(const char* filename, vector<glm::vec3> &vertices, vector<glm::vec3> &normals, vector<glm::vec3> &textures,vector<GLuint> &elements)
{
    ifstream in(filename, ios::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << endl;
        return false;
    }
    

    
    vector<glm::vec3> temp_normals;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec3> temp_index_normal2point;
    vector<glm::vec3> temp_index_triangle;
    vector<glm::vec3> temp_index_textures;

	vector<glm::vec3> temp_textures;
    
    string line;
    while (getline(in, line))
    {
        if (line.substr(0,2) == "v ")
        {
            istringstream s(line.substr(2));
            glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;// v.w = 1.0f;
            temp_vertices.push_back(v);
            _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
            
        }
		else if (line.substr(0,3) == "vt ")//Pei add this line to substract texture info
		{
			istringstream s(line.substr(3));
			glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;// v.w = 1.0f;
			temp_textures.push_back(v);
		}
		
        else if (line.substr(0,2) == "f ")
        {
            int idx = line.find_first_of("/");
            if(idx == -1)
            {
                istringstream s(line.substr(2));
                GLushort a,b,c;
                s >> a; s >> b; s >> c;
                cout << a << " : " << b << " : " << c << endl;;
                a--; b--; c--;
            
                elements.push_back(a); elements.push_back(b); elements.push_back(c);
            }
            else
            {
                
                size_t n = std::count(line.begin(), line.end(), '/');
                if(n == 0)
                {
                    string new_line;
                    int p0, p1, p2;
                    extractNextFace1(line, new_line, p0, p1, p2 );
                    
                    temp_index_triangle.push_back(glm::vec3(p0, p1, p2));
                    temp_index_normal2point.push_back(glm::vec3(p0, p1, p2)); // same normal vectors.
                    
                }
                else if(n == 3){
                    string new_line;
                    int p0, p1, p2;
                    extractNextFace1(line, new_line, p0, p1, p2 );
                    
                    temp_index_triangle.push_back(glm::vec3(p0, p1, p2));
                    temp_index_normal2point.push_back(glm::vec3(p0, p1, p2)); // same normal vectors.               
                }
                else if(n==6||n==8){
                    string new_line;
                    int p0, t0,  n0;
                    int p1, t1,  n1;
                    int p2, t2,  n2;
                    int p3, t3,  n3;
                    
                    // first
                    extractNextFace3(line, new_line, p0, t0, n0 );
                    
                    // second
                    extractNextFace3(new_line, new_line, p1, t1, n1 );
                    
                    // third
                    extractNextFace3(new_line, new_line, p2, t2, n2 );
                    
                    // Quads and not triangles
                    if(new_line.length() > 2)
                    {
                        // fourth
                        extractNextFace3(new_line, new_line, p3, t3, n3 );
                    }

                    // remember the normal vectors
                    temp_index_triangle.push_back(glm::vec3(p0, p1, p2));
                    temp_index_normal2point.push_back(glm::vec3(n0, n1, n2));
                    temp_index_textures.push_back(glm::vec3(t0, t1, t2));
                    
                    elements.push_back(p0);
                    elements.push_back(p1);
                    elements.push_back(p2);
                    
                }
                
               
            }
        }
        else if (line.substr(0,3) == "vn ")
        {
            istringstream s(line.substr(3));
            glm::vec3 n; s >> n.x; s >> n.y; s >> n.z;
            temp_normals.push_back(n);
        }
        else if (line.substr(0,7) == "mtllib ")
        {
            istringstream s(line.substr(7));
            s >> _material_file;
            /* ignoring this line */
        }
        else if (line.substr(0,2) == "g ")
        {
            istringstream s(line.substr(2));
            s >> _model_name;
            /* ignoring this line */
        }
        else if (line[0] == '#')
        {
            /* ignoring this line */
        }
        else
        {
            /* ignoring this line */
        }
    }
    
    
    /////////////////////////////////////////////
    // assign normals to points and points to triangles.
    
    vertices.clear();
    normals.clear();
	textures.clear();

    for(int i=0; i<temp_index_triangle.size(); i++)
    {
        glm::vec3 pointIdx = temp_index_triangle[i];
        glm::vec3 normalIdx = temp_index_normal2point[i];
        
        // the three points of one triangle
        // -1 since obj does not use the index 0
        vertices.push_back(temp_vertices[ pointIdx.x-1 ]);
        vertices.push_back(temp_vertices[ pointIdx.y-1 ]);
        vertices.push_back(temp_vertices[ pointIdx.z-1 ]);
        
        normals.push_back(temp_normals[normalIdx.x-1]);
        normals.push_back(temp_normals[normalIdx.y-1]);
        normals.push_back(temp_normals[normalIdx.z-1]);
        
    }
    
    //Pei add this to store texture info
	for (int i = 0; i < temp_index_textures.size(); i++){
		glm::vec3 texturesIdx = temp_index_textures[i];
//		textures.push_back(texturesIdx);
		textures.push_back(temp_textures[texturesIdx.x - 1]);
		textures.push_back(temp_textures[texturesIdx.y - 1]);
		textures.push_back(temp_textures[texturesIdx.z - 1]);

	}
    
    
    
    
    if(normals.size() > 0) return true; // loaded normal vectors
    
    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
    for (int i = 0; i < elements.size(); i+=3)
    {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(
                                                     glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
                                                     glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
        
        
       // _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
      //  _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
       // _vertex_colors.push_back(glm::vec4(1.0,0.0,0.0,1.0));
    }
    
    return true;
}



void GLBird::GLloadTexture(){
//	std::string str1("../../data/textures/deer_texture.bmp");
//	std::string str2("../../data/textures/colorGradient.bmp");
	
	unsigned int channels;
	unsigned int width;
	unsigned int height;

	// Load the texture from file.
	unsigned char* g_image_data = GLloadBitmapFile(_texture_name, channels, width, height);
	GLuint texture;
	if (_texture_name.compare(str1) == 0){
		glGenTextures(0, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE0);// Set a texture as active texture.
		glUniform1i(glGetUniformLocation(_program, "birdBMP"), 0);
		// Change the parameters of your texture units.
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, g_image_data);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, g_image_data);
	}
	/*
	else if (_texture_name.compare(str2) == 0){
		cout << "texture2 print" << endl;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);// Set a texture as active texture.
		glUniform1i(glGetUniformLocation(_program, "birdBMP"), 1);
		// Change the parameters of your texture units.
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, g_image_data);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, g_image_data);
	}*/
}


/*!
 Create the vertex buffer object for this element
 */
void GLBird::initVBO(void)
{
    _num_vertices = _vertices.size();
    
    // create memory for the vertices, etc.
    float* vertices = new float[_num_vertices * 3];
    float* normals = new float[_normals.size() * 3];
	float* textures = new float[_textures.size() * 3];
    int* indices = new int[_elements.size()];
    
    // Copy all vertices
    for(int i=0; i<_vertices.size() ; i++)
    {
        glm::vec3 t = _vertices[i];
        for (int j=0; j<3; j++) {
            vertices[(i*3)+j] = t[j];
        }
    }
    
    // copy all normals
    for(int i=0; i<_normals.size() ; i++)
    {
        glm::vec3 n = _normals[i];
        for (int j=0; j<3; j++) {
            normals[(i*3)+j] = n[j];
        }
    }

	// copy all normals
	for (int i = 0; i<_textures.size(); i++)
	{
		glm::vec3 te = _textures[i];
		for (int j = 0; j<3; j++) {
			textures[(i * 3) + j] = te[j];
		}
	}

	if (_texture_name.compare(str1) == 0){
		GLloadTexture();//load the texture image
	}

    glGenVertexArrays(1, _vaoID); // Create our Vertex Array Object
    glBindVertexArray(_vaoID[0]); // Bind our Vertex Array Object so we can use it
    
    
    glGenBuffers(3, _vboID); // Generate our Vertex Buffer Object
    
    // vertices
    int locPos = glGetAttribLocation(_program, "in_Position");
    glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]); // Bind our Vertex Buffer Object
	glBufferData(GL_ARRAY_BUFFER, _num_vertices * 3 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    
    glVertexAttribPointer((GLuint)locPos, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(locPos); //
    
     // normals
    int locNorm = glGetAttribLocation(_program, "in_Normal");
    glBindBuffer(GL_ARRAY_BUFFER, _vboID[1]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, _normals.size() * 3 * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
    
    glVertexAttribPointer((GLuint)locNorm, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(locNorm); //

	//textures
	if (_texture_name.compare(str1) == 0){
		GLint texAttrib = glGetAttribLocation(_program, "in_texcoord");
		glBindBuffer(GL_ARRAY_BUFFER, _vboID[2]); // Bind our Vertex Buffer Object
		glBufferData(GL_ARRAY_BUFFER, _textures.size() * 3 * sizeof(GLfloat), &textures[0], GL_STATIC_DRAW); // Set the size and data of our VBO and set it to STATIC_DRAW
		glVertexAttribPointer((GLuint)texAttrib, 2, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(texAttrib);
	}

	 // Delete our data from memory
	delete[] vertices;
	delete[] normals;
	delete[] textures;	
    
    glBindVertexArray(0); // Disable our Vertex Buffer Object



}


/*
 Inits the shader program for this object
 */
void GLBird::initShader(void)
{
    
    if(!_appearance.exists())return;
    
    // This loads the shader program from a file
    _program = _appearance.getProgram();
    
    
    glUseProgram(_program);
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Vertex information / names
	
	/*
    glBindAttribLocation(_program, 0, "in_Position");
    glBindAttribLocation(_program, 1, "in_Normal");
	glBindAttribLocation(_program, 2, "in_texcoord");
	*/
	glBindAttribLocation(_program, glGetAttribLocation(_program, "in_Position"), "in_Position");
	glBindAttribLocation(_program, glGetAttribLocation(_program, "in_Normal"), "in_Normal");
	glBindAttribLocation(_program, glGetAttribLocation(_program, "in_texcoord"), "in_texcoord");
	
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Define the model view matrix.
    _modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)); // Create our model matrix which will halve the size of our model
    addModelViewMatrixToProgram(_program);
    
    
    
    glUseProgram(0);
    
}


void GLBird::setModelMatrix(glm::mat4& modelmatrix)
{
    _modelMatrix=modelmatrix;

}


/*!
 Draw the objects
 */
void GLBird::draw(void)
{
    
    glUseProgram(_program);

    // Bind the buffer and switch it to an active buffer
    glBindVertexArray(_vaoID[0]);
    
    
    // this changes the camera location
    glm::mat4 rotated_view =  rotatedViewMatrix();
    glUniformMatrix4fv(_viewMatrixLocation, 1, GL_FALSE, &rotated_view[0][0]); // send the view matrix to our shader
    glUniformMatrix4fv(_inverseViewMatrixLocation, 1, GL_FALSE, &invRotatedViewMatrix()[0][0]);
    glUniformMatrix4fv(_modelMatrixLocation, 1, GL_FALSE, &_modelMatrix[0][0]); //

    
   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    // Draw the triangles
    glDrawArrays(GL_TRIANGLES, 0, _num_vertices);
    
   // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementbuffer);
    //glDrawElements( GL_TRIANGLES, _elements.size(), GL_UNSIGNED_INT,(void*)0 );
    
    
    // Unbind our Vertex Array Object
    glBindVertexArray(0);
    
}



/*!
Returns the number of vertices
*/
int GLBird::size(void)
{
    return _num_vertices;
}


/*!
To update the vertices.
This function takes a vector of vertices and replaces the current vector.
*/
void GLBird::updateVertices(float* vertices)
{

    glBindVertexArray(_vaoID[0]); // Bind our Vertex Array Object so we can use it
    
    // vertices
    int locPos = glGetAttribLocation(_program, "in_Position");
    glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]); // Bind our Vertex Buffer Object
    glBufferData(GL_ARRAY_BUFFER, _num_vertices * 3 * sizeof(GLfloat), vertices, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer((GLuint)locPos, 3, GL_FLOAT, GL_FALSE, 0, 0); // Set up our vertex attributes pointer
    glEnableVertexAttribArray(locPos); //

}













