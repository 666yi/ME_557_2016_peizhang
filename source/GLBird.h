#pragma once

// stl include
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLObject.h"

using namespace std;

class GLBird : public GLObject
{
    // Ray intersection test has access to protected functions
    friend class RayIntersectionTest;
    
public:
	GLBird(string filename, string texturename);
    GLBird();
    ~GLBird();
    
    
    /*!
     Draw the objects
     */
    void draw(void);
    
    /*!
     Set the model matrix for this object
     @param modelmatrix: 4x4 model matrix
     */
    void setModelMatrix(glm::mat4& modelmatrix);
		
    /*!
     Init the geometry object
     */
    void init(void);
    
    
    /*!
    Returns the number of vertices
    */
    int size(void);
    
	/*!
	Loads a texture from a file and creates the necessary texture objects
	@param path_and_file to the texture object
	@return unsigned char - a pointer to the texture data
	*/
	static unsigned char* Load(string path_and_file);

    /*!
    Returns a reference too the vertices.
    */
    vector<glm::vec3>& getVertices(void){return  _vertices; };
    
    /*!
    To update the vertices. 
    This function takes a vector of vertices and replaces the current vector.
    */
    void updateVertices(float* vertices);
    
private:
    
    /*!
     Load an obj model from a file
     */
	bool load_obj(const char* filename, vector<glm::vec3> &vertices, vector<glm::vec3> &normals, vector<glm::vec3> &textures,vector<GLuint> &elements);
	
	/*!
	Verifies wheterh a file [name] exits
	@param name - the path and the name of the file.
	*/
	static bool GLBird::GLSearchFile(const std::string& name, string& new_name);
    
    /*!
     Extract one obj face coded as:f v/vt/vn v/vt/vn v/vt/vn
     */
    bool extractNextFace3(string& in, string& out, int& pointIdx, int& texIdx, int& normalIdx );
    bool extractNextFace1(string& in, string& out, int& pointIdx0, int& pointIdx1, int& pointIdx2  );
    
    /*!
     Create the vertex buffer object for this element
     */
    virtual void initVBO(void);
    
    /*
     Inits the shader program for this object
     */
    virtual void initShader(void);
    
	void GLBird::GLloadTexture();
    // the program
    GLuint                  _program;
    

    // file and path of the loaded object
    string                  _file_and_path;
    bool                    _file_ok;
   
protected:
    // The data
    vector<glm::vec3>       _vertices;
    vector<glm::vec4>       _vertex_colors;
    vector<glm::vec3>       _normals;
	vector<glm::vec3>       _textures;
    vector<GLuint>          _elements;
    
private:
    // obj file data
    string                  _material_file;
    string                  _model_name;
	string					_texture_name;
	string					str1;
	string					str2;
	unsigned char * g_image_data;
    int                     _num_vertices;
    
	glm::mat4               _modelMatrix; // Store the model matrix
		
    unsigned int            _vaoID[1]; // Our Vertex Array Object
    
    unsigned int            _vboID[3]; // Our Vertex Buffer Object
    
    GLuint                  _elementbuffer;
};