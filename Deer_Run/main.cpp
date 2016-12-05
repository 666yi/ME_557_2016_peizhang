//
//  main.cpp
//
//  This is project code for HCI/ME/CS/CprE 557 Computer Graphics
//  Template Created by Rafael Radkowski on 5/28/15.
//  Design by Pei on 11/20/16.
//  Iowa State University
//

// stl include
#include <iostream>
#include <string>
#include <map>

// GLEW include
#include <GL/glew.h>

// GLM include files
#define GLM_FORCE_INLINE 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// GLFW include
#include <GLFW/glfw3.h>

//additional dependency
#include "controls.h"
#include "HCI557Common.h"
#include "CoordSystem.h"
#include "Sphere3D.h"
//#include "GLObjectObj.h"
//#include "GLColoredBox.h"
#include "GLBird.h"
#include "Plane3D.h"
#include "Texture.h"

//game macro definition
#define UP_SPEED 8.0f
#define RUN_SPEED 60.0f;
#define HORIZON_SPEED 20.0f
#define GRAVITY 0.5f
#define tree_number 13
#define TREE_DISTANCE 30
#define MAP_BOUNDARY 60
//random position generation
#define LO -40.0f
#define HI 40.0f


using namespace std;
using namespace glm;

float bird_height = 0.0f;
float pre_bird_height = 0.0f;
float bird_forward = 0.0f;
float pre_bird_forward = 0.0f;
float delta_time = 0.0f;
glm::vec3 bird_move;
int scene = 0;
float last_fly_time = 0.0f;
float bird_horizon = 0.0f;
GLBird* bird;
GLfloat clear_color[] = { 0.6f, 0.7f, 1.0f, 1.0f };
static const GLfloat clear_depth[] = { 1.0f, 1.0f, 1.0f, 1.0f };



// The main window
GLFWwindow* window;

GLuint program;

extern int g_change_texture_blend;
/* A trackball to move and rotate the camera view */
extern Trackball trackball;

/*!
 A struct to define keyframe
 */
typedef struct _keyframe
{
    float               _t; // the time fraction
    glm::vec3           _p; // the position
    glm::quat           _q; // the orientation
    
    /*
     Constructor
     */
    _keyframe(float t, glm::vec3 p, glm::quat q)
    {
        _t = t;
        _p = p;
        _q = q;
    }
    
    /*
     Default constructor
     */
    _keyframe()
    {
        _t = -1.0;
        _p = glm::vec3(0.0,0.0,0.0);
        _q = glm::quat(0.0,0.0,0.0,0.0);
    }
    
    // prints the data into a terminal
    void print(void)
    {
        cout << "t: " << _t << "\tp: " << _p.x << ", " << _p.y << ", " << _p.z << "\tq: " << _q.x  << ", " << _q.y  << ", " << _q.z  << ", " << _q.w  << endl;
    }

}Keyframe;


/*
 Type for the keyframe animation
 */
typedef map<double, Keyframe> KeyframeAnimation;

// Variable to store the keyframes
KeyframeAnimation myKeyframes;
KeyframeAnimation startSceneAnimation;

/*!
 @brief returns the time fraction for a given time and animation duration
 @param time - the current animation time, application runtime, etc. in seconds
 @param duration - the duration of the animation in seconds
 @return the time fraction in an interval between 0 and 1.
 */
float getTimeFraction(const float time, const float duration)
{
    // we cast to an int. this results in the number of
    float interval = floor(time/duration);
    
    // return the current interval time
    float current_interval = time - interval*duration;
    
    // return the fraction / position in our current timeline
    float fraction = current_interval / duration;

    return fraction;
}



/*!
 @brief returns the two keyframes for a given time. 
 @param keyframes - a map with all keyframes of type KeyframeAnimation
 @param time - the time fraction between 0 and 1. 
 @param k0, reference to the first keyframe
 @param k2, reference to the second keyframe
 @return the number of keyframes. 1 if the time is equal to a keyframe, otherwise 2.
 */
int getKeyframes( KeyframeAnimation& keyframes, const double time, Keyframe& k0, Keyframe& k1)
{
    int num_keyframes = 0;
    
    // get a keyframe iterator
    KeyframeAnimation::iterator k_itr = keyframes.lower_bound(time);
    
    Keyframe k0_temp, k1_temp;
    
    // Obtain the first keyframe
    k1 = (*k_itr).second; num_keyframes++;
    
    
    // Check whether we are not at the beginning of this map
    if(k_itr != keyframes.begin())
    {
        k_itr--;  // decrement
        k0 = (*k_itr).second; // obtain the second keyframe
        num_keyframes++;
    }
  
    // write the first keyframe into k0 if we only have one
    if(num_keyframes == 1)
    {
        k0 = k1;
    }
  
    return num_keyframes;
    
}

/*!
 @brief Interpolate between two keyframes
 @param fraction - the time fraction for the interpolation / the location between two keyframes.
                    The value must be between 0 and 1.
 @param k0, the start keyframe
 @param k1, the end keyframe, 
 @param res, reference to a variable for the result.
 */
bool interpolateKeyframe(const float fraction, const Keyframe& k0, const Keyframe& k1, Keyframe& res)
{
    /////////////////////////////////////////////////////////////////////////
    // 1. Check the time delta
    
    // delta time
    float delta_t = k1._t - k0._t;
    
    // Check whether we have a delta time. Otherwise, we are at the location of exactly one keyframe
    if(delta_t == 0.0f){
        res = k0;
        return true;
    }
    
    /////////////////////////////////////////////////////////////////////////
    // 2. Interpolat the position
    
    // get the delta
    glm::vec3 delta_p = k1._p - k0._p;
    
    // position interpolation
    glm::vec3 p_int =  k0._p + delta_p * (fraction- k0._t)/(delta_t);
    
    
    /////////////////////////////////////////////////////////////////////////
    // 3. Rotation interpolation
    
    // Calculate the distance between the target angle and the current angle.
    float delta_angle = sqrt((k1._q.x - k0._q.x)*(k1._q.x - k0._q.x)  +
                             (k1._q.y - k0._q.y)*(k1._q.y - k0._q.y)  +
                             (k1._q.z - k0._q.z)*(k1._q.z - k0._q.z)  +
                             (k1._q.w - k0._q.w)*(k1._q.w - k0._q.w));
    
    
    // Linear interpolation of the rotation using slerp
    glm::quat r_int = glm::slerp(k0._q, k1._q, (fraction- k0._t)/(delta_t));
    
    
    /////////////////////////////////////////////////////////////////////////
    // 4. Write the result
    res = Keyframe(fraction, p_int, r_int);
    
    return true;
}

/*!
 This initializes the keyframes.
 */
void initKeyframeAnimation(void)
{
//start animation frame
	startSceneAnimation[0.0] = Keyframe(0.0, glm::vec3(1.0, 0.0, 0.0), angleAxis(0.0f, glm::vec3(0.0, 1.0, 0.0)));
	startSceneAnimation[0.5] = Keyframe(0.5, glm::vec3(0.0, 0.0, 0.0), angleAxis(1.98f, glm::vec3(0.0, 1.0, 0.0)));
	startSceneAnimation[1.0] = Keyframe(1.0, glm::vec3(1.0, 0.0, 0.0), angleAxis(0.0f, glm::vec3(0.0, 1.0, 0.0)));

//fly animation frame
	myKeyframes[0.0] = Keyframe(0.0, glm::vec3(0.0, 0, 0.0), angleAxis(0.0f, glm::vec3(0.0, 0.0, 1.0)));
	myKeyframes[0.5] = Keyframe(0.5, glm::vec3(0.0, 0, 0.0), angleAxis(-3.14f / 5, glm::vec3(0.0, 0.0, 1.0)));
	myKeyframes[1.0] = Keyframe(1.0, glm::vec3(0.0, 0, 0.0), angleAxis(0.0f, glm::vec3(0.0, 0.0, 1.0)));
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	switch (scene){
	case 0:{
		
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){//space, start the game 			
			scene = 1;
			bird->init();
		}
		else if (key == GLFW_KEY_Q && action == GLFW_PRESS){//space, start the game 			
			g_change_texture_blend = 1;
		}
		break;
	}
	
	case 1:{
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){//space, start the game 
			SetViewAsLookAt(glm::vec3(5.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			last_fly_time = glfwGetTime();//record the time when fly
			scene = 2;
			clear_color[0] = 0.050f;
			clear_color[1] = 0.741f;
			clear_color[2] =  0.949f;
			clear_color[3] = 0.0f;
			glClearBufferfv(GL_COLOR, 0, clear_color);
			bird->init();
		}
		break;
	}
	case 2:{
		/*
		if (key == GLFW_KEY_W && action == GLFW_PRESS){
			last_fly_time = glfwGetTime();//record the time when fly
			bird_height += UP_SPEED*delta_time;			
		}
		*/
		if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS) && bird_horizon>-MAP_BOUNDARY){
			last_fly_time = glfwGetTime();//record the time when fly
			bird_horizon -= HORIZON_SPEED*delta_time;
		}
		else if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS) && bird_horizon<MAP_BOUNDARY){
			last_fly_time = glfwGetTime();//record the time when fly
			bird_horizon += HORIZON_SPEED*delta_time;
		}
		break;
	}
	case 3:{
	/*	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
			scene = 0;
		}*/
		break;
	}
		
	}
}

int main( void )
{
		
    // Initialise GLFW
    if( !glfwInit() )
    {
        cout <<  "Failed to initialize GLFW\n" << endl;
        system("pause");
        return -1;
    }
    
    
    // Set the version which we expect, here, 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    
    // Open a window and create its OpenGL context
//    window = glfwCreateWindow( 1024, 768, "Deer Run", NULL, NULL);

	window = initWindow();
    if( window == NULL ){
        cout <<  "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible." << endl;
        glfwTerminate();
        system("pause");
        return -1;
    }
    
    // Switch the window as the object that we work with.
    glfwMakeContextCurrent(window);
    
	initGlew();
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        cout <<  "Failed to initialize GLEW\n" << endl;
        system("pause");
        return -1;
    }
    
	int last_bump_tree = 0;
	bool bump_flag = false;
	int bump = 0;
    // This inits the keyframe animation
    initKeyframeAnimation();
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Create some models
    
    // coordinate system
    CoordSystem* cs = new CoordSystem(4.0);
    
    // This example shows three boxes at different locations
    // The first box is at its local origin, the location where the box was created.
    
    // three identical boxes
    // on its original location.
    //GLColoredBox* box = new GLColoredBox(0.5,1.0,0.5);	
	GLAppearance* appearance = new GLAppearance("../../data/shaders/flappy_bird.vs", "../../data/shaders/flappy_bird.fs");
	GLDirectLightSource  light_source;
	light_source._lightPos = glm::vec4(20.0, 20.0, 0.0, 0.0);
	light_source._ambient_intensity = 0.2;
	light_source._specular_intensity = 5.5;
	light_source._diffuse_intensity = 2.0;
	light_source._attenuation_coeff = 0.0;

	appearance->addLightSource(light_source);

	// Create a material object
	GLMaterial material_GLBird;
	material_GLBird._diffuse_material = glm::vec3(1.0, 0.0, 0.2);
	material_GLBird._ambient_material = glm::vec3(1.0, 0.0, 0.2);
	material_GLBird._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_GLBird._shininess = 12.0;
	material_GLBird._transparency = 1.0;

	// Add the material to the appearance object
	appearance->setMaterial(material_GLBird);
	appearance->finalize();
	// If you want to change appearance parameters after you init the object, call the update function
	appearance->updateLightSources();


	//load the bird and tree model
	bird = new GLBird("../../data/deer-obj.obj", "../../data/textures/deer_texture.bmp");
	bird->setApperance(*appearance);
	bird->init();
	GLAppearance* appearance_1 = new GLAppearance("../../data/shaders/flappy_bird.vs", "../../data/shaders/flappy_bird.fs");
	GLDirectLightSource  light_source_1;
	light_source_1._lightPos = glm::vec4(20.0, 20.0, 0.0, 0.0);
	light_source_1._ambient_intensity = 0;
	light_source_1._specular_intensity = 15.5;
	light_source_1._diffuse_intensity = 12.0;
	light_source_1._attenuation_coeff = 1.0;
	GLMaterial material_GLTree;
	material_GLTree._diffuse_material = glm::vec3(0.952, 0.737, 0.086);
	material_GLTree._ambient_material = glm::vec3(0.952, 0.737, 0.086);
	material_GLTree._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_GLTree._shininess = 12.0;
	material_GLTree._transparency = 0.8;
	appearance_1->setMaterial(material_GLTree);
	appearance_1->addLightSource(light_source_1);
	appearance_1->finalize();
	GLBird* tree = new GLBird("../../data/Cartoon low poly trees.obj","");	
	tree->setApperance(*appearance_1);
	tree->init();
//construct multiple trees
	GLBird* Obtree[tree_number];
	float tree_loc[tree_number];
	float tree_horizon[tree_number];
	GLAppearance* appearance_4;
	GLDirectLightSource  light_source_4;
	light_source_4._lightPos = glm::vec4(2000.0, 2000.0, 0.0, 0.0);
	light_source_4._ambient_intensity = 0.01;
	light_source_4._specular_intensity = 15.5;
	light_source_4._diffuse_intensity = 120.0;
	light_source_4._attenuation_coeff = 1.0;
	GLMaterial material_ObTree[tree_number];
	for (int i = 0; i < tree_number; i++){
		appearance_4 = new GLAppearance("../../data/shaders/flappy_bird_2.vs", "../../data/shaders/flappy_bird_2.fs");
		float rand1 = 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0.1)));
		float rand2 = 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0.1)));
		float rand3 = 0.1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - 0.1)));
		material_ObTree[i]._diffuse_material = glm::vec3(rand1, rand2, rand3);
		material_ObTree[i]._ambient_material = glm::vec3(rand1, rand2, rand3);
		material_ObTree[i]._specular_material = glm::vec3(1.0, 1.0, 1.0);
		material_ObTree[i]._shininess = 12.0;
		material_ObTree[i]._transparency = 1;
		appearance_4->setMaterial(material_ObTree[i]);
		appearance_4->addLightSource(light_source_4);
		appearance_4->finalize();
		tree_loc[i] = TREE_DISTANCE*i + TREE_DISTANCE;
		//tree_horizon[i] = 0;
		tree_horizon[i] = LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
		Obtree[i] = new GLBird("../../data/Cartoon low poly trees.obj", "");
		Obtree[i]->setApperance(*appearance_4);
		Obtree[i]->init();
	}	
	glm::mat4 bird_scale = glm::scale(glm::vec3(0.05f, 0.05, 0.05f));


	//text display
	GLAppearance* appearance_tex = new GLAppearance("../../data/shaders/multi_texture.vs", "../../data/shaders/multi_texture.fs");
	GLDirectLightSource  light_source_tex;
	light_source_tex._lightPos = glm::vec4(00.0, 20.0, 20.0, 0.0);
	light_source_tex._ambient_intensity = 0.2;
	light_source_tex._specular_intensity = 5.5;
	light_source_tex._diffuse_intensity = 2.0;
	light_source_tex._attenuation_coeff = 0.0;
	appearance_tex->addLightSource(light_source_tex);
	// Create a material object
	GLMaterial material_tex;
	material_tex._diffuse_material = glm::vec3(0.470, 0.631, 0.290);
	material_tex._ambient_material = glm::vec3(0.470, 0.631, 0.290);
	material_tex._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_tex._shininess = 12.0;
	material_tex._transparency = 0.5;
	// Add the material to the apperance object
	appearance_tex->setMaterial(material_tex);
	// add the light to this apperance object
	GLMultiTexture* texture = new GLMultiTexture();
	int texid = texture->loadAndCreateTextures("../../data/textures/gameover.bmp", "../../data/textures/deerrun.bmp", "../../data/textures/title.bmp");
	appearance_tex->setTexture(texture);
	appearance_tex->finalize();
	// create the sphere geometry
	GLPlane3D* plane_tex = new GLPlane3D(0.0, 0.0, 0.0, 10.0, 10.0);
	plane_tex->setApperance(*appearance_tex);
	plane_tex->init();

	//Ground and cliff appearance
	GLPlane3D* plane_0 = new GLPlane3D(0.0, 0.0, 0.0, 1000.0, 1000.0);
	GLPlane3D* plane_1 = new GLPlane3D(0.0, 0.0, 0.0, 1000.0, 1000.0);
	GLPlane3D* plane_2 = new GLPlane3D(0.0, 0.0, 0.0, 1000.0, 1000.0);
	GLAppearance* appearance_2 = new GLAppearance("../../data/shaders/flappy_bird_1.vs", "../../data/shaders/flappy_bird_1.fs");
	GLDirectLightSource  light_source_2;
	light_source_2._lightPos = glm::vec4(0.0, 120.0, 0.0, 0.0);
	light_source_2._ambient_intensity = 0.1;
	light_source_2._specular_intensity = 15.5;
	light_source_2._diffuse_intensity = 12.0;
	light_source_2._attenuation_coeff = 1.0;
	GLMaterial material_GLPlane;
	material_GLPlane._diffuse_material = glm::vec3(0.313, 0.905, 0.317);
	material_GLPlane._ambient_material = glm::vec3(0.313, 0.905, 0.317);
	material_GLPlane._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_GLPlane._shininess = 12.0;
	material_GLPlane._transparency = 1;

	appearance_2->setMaterial(material_GLPlane);
	appearance_2->addLightSource(light_source_2);
	plane_0->setApperance(*appearance_2);
	plane_0->init();
	plane_1->setApperance(*appearance_2);
	plane_1->init();
	plane_2->setApperance(*appearance_2);
	plane_2->init();
	appearance_2->finalize();
	appearance_2->updateLightSources();
    //Sun1 appearance
	GLAppearance* appearance_3 = new GLAppearance("../../data/shaders/flappy_bird_1.vs", "../../data/shaders/flappy_bird_1.fs");
	GLDirectLightSource  light_source_3;
	light_source_3._lightPos = glm::vec4(1000.0, 2000.0, 2000.0, 0.0);
	light_source_3._ambient_intensity = 1.01;
	light_source_3._specular_intensity = 1.5;
	light_source_3._diffuse_intensity = 3.1;
	light_source_3._attenuation_coeff = 1;
	GLMaterial material_GLSun;
	material_GLSun._diffuse_material = glm::vec3(0.921, 0.125, 0.117);
	material_GLSun._ambient_material = glm::vec3(0.921, 0.125, 0.117);
	material_GLSun._specular_material = glm::vec3(0.921, 0.125, 0.117);
	material_GLSun._shininess = 10.0;
	material_GLSun._transparency = 1;
	appearance_3->setMaterial(material_GLSun);
	appearance_3->addLightSource(light_source_3);
	GLSphere3D* sphere_1 = new GLSphere3D(0.0, 2.0, -120.0, 10.0, 100, 100);
	sphere_1->setApperance(*appearance_3);
	sphere_1->init();
	appearance_3->finalize();

	//Sun2 appearance
	GLAppearance* appearance_5 = new GLAppearance("../../data/shaders/flappy_bird_1.vs", "../../data/shaders/flappy_bird_1.fs");
	GLDirectLightSource  light_source_5;
	light_source_5._lightPos = glm::vec4(1000.0, 2000.0, 2000.0, 0.0);
	light_source_5._ambient_intensity = 1.01;
	light_source_5._specular_intensity = 1.5;
	light_source_5._diffuse_intensity = 3.1;
	light_source_5._attenuation_coeff = 1;
	GLMaterial material_GLSun2;
	material_GLSun2._diffuse_material = glm::vec3(0.921, 0.125, 0.117);
	material_GLSun2._ambient_material = glm::vec3(0.921, 0.125, 0.117);
	material_GLSun2._specular_material = glm::vec3(0.921, 0.125, 0.117);
	material_GLSun2._shininess = 1000.0;
	material_GLSun2._transparency = 1;
	material_GLSun2._emissive_material = glm::vec3(0.921, 0.125, 0.117);
	appearance_5->setMaterial(material_GLSun2);
	appearance_5->addLightSource(light_source_5);
	GLSphere3D* sphere_2 = new GLSphere3D(-360*3.0, 25*3.0, 0.0, 100.0, 100, 100);
	sphere_2->setApperance(*appearance_5);
	sphere_2->init();
	appearance_5->finalize();

	//score ball
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	GLAppearance* appearance_10 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* appearance_11 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* appearance_12 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLAppearance* appearance_13 = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	// The spotlight object
	GLSpotLightSource  light_source10;
	light_source10._lightPos = glm::vec4(20.0, 30.0, 5.0, 1.0);
	light_source10._ambient_intensity = 0.1;
	light_source10._specular_intensity = 1.0;
	light_source10._diffuse_intensity = 1.0;
	light_source10._attenuation_coeff = 0.0;
	light_source10._inner_cone_angle = 4.0;
	light_source10._outer_cone_angle = 6.8;
	light_source10._cone_direction = glm::vec3(20+1.5, 30-5, 5+2); // this must be aligned with the object and light position.
	GLDirectLightSource  light_source11;
	light_source11._lightPos = glm::vec4(10.0, -30.0, 40.0, 0.0);
	light_source11._ambient_intensity = 0.003;
	light_source11._specular_intensity = 0.5;
	light_source11._diffuse_intensity = 0.9;
	light_source11._attenuation_coeff = 0.1;
	// light_source11._cone_angle = 12.0; // in degree
	// light_source11._cone_direction = glm::vec3(-1.0, 0.0, 0.0); // this must be aligned with the object and light position.
	GLDirectLightSource  light_source12;
	light_source12._lightPos = glm::vec4(2.0, 2.0, 8.0, 0.0);
	light_source12._ambient_intensity = 0.01;
	light_source12._specular_intensity = 1.5;
	light_source12._diffuse_intensity = 1;
	light_source12._attenuation_coeff = 0.08;
	//	light_source12._cone_angle = 180.0; // in degree
	//	light_source12._cone_direction = glm::vec3(-100.0, -20.0, 120.0); // this must be aligned with the object and light position.
	GLDirectLightSource  light_source13;
	light_source13._lightPos = glm::vec4(2.0, -0.0, 6.0, 0.0);
	light_source13._ambient_intensity = 0.01;
	light_source13._specular_intensity = 0;
	light_source13._diffuse_intensity = 0.8;
	light_source13._attenuation_coeff = 0.08;
	GLSpotLightSource  light_source14;
	light_source14._lightPos = glm::vec4(20.5, 25, 5, 0.0);
	light_source14._ambient_intensity = 0.1;
	light_source14._specular_intensity = 0;
	light_source14._diffuse_intensity =0.15;
	light_source14._attenuation_coeff = 0.01;
	//	light_source5._inner_cone_angle = 5;
	light_source14._outer_cone_angle = 10; // in degree
	light_source14._cone_direction = glm::vec3(2.5-2.5, 2.5-7, 0.5-3); // this must be aligned with the object and light position.
	// add the spot light to this apperance object
	appearance_10->addLightSource(light_source12);
	appearance_11->addLightSource(light_source13);
	appearance_12->addLightSource(light_source14);
	//	appearance_13->addLightSource(light_source0);
	appearance_13->addLightSource(light_source10);
	appearance_13->addLightSource(light_source11);
	// Create a material object
	GLMaterial material_10, material_11, material_12, material_13;
	material_10._diffuse_material = glm::vec3(0.949, 0.043, 0.007);
	material_10._ambient_material = glm::vec3(0.949, 0.043, 0.007);
	material_10._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_10._shininess = 100;
	material_11._diffuse_material = glm::vec3(0.117, 0.196, 0.964);
	material_11._ambient_material = glm::vec3(0.117, 0.196, 0.964);
	material_11._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_11._shininess = 0.1;
	material_12._diffuse_material = glm::vec3(0.349, 0.968, 0.133);
	material_12._ambient_material = glm::vec3(0.349, 0.968, 0.133);
	material_12._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_12._shininess = 300;
	material_13._diffuse_material = glm::vec3(0.933, 0.898, 0.105);
	material_13._ambient_material = glm::vec3(0.933, 0.898, 0.105);
	material_13._specular_material = glm::vec3(1.0, 1.0, 1.0);
	material_13._shininess = 100.0;
	// Add the material to the apperance object
	appearance_10->setMaterial(material_10);
	appearance_11->setMaterial(material_11);
	appearance_12->setMaterial(material_12);
	appearance_13->setMaterial(material_13);
	appearance_10->finalize();
	appearance_11->finalize();
	appearance_12->finalize();
	appearance_13->finalize();
	// create the sphere geometry
	GLSphere3D* sphere_10 = new GLSphere3D(0.0, 0.0, 9.0, 1.0, 100, 100);
	GLSphere3D* sphere_11 = new GLSphere3D(0.0, 0.0, 6.0, 1.0, 100, 100);
	GLSphere3D* sphere_12 = new GLSphere3D(0.0, 0.0, 3.0, 1.0, 100, 100);
	GLSphere3D* sphere_13 = new GLSphere3D(0.0, 0.0, 0.0, 1.0, 100, 100);
	sphere_10->setApperance(*appearance_10);
	sphere_10->init();
	sphere_11->setApperance(*appearance_11);
	sphere_11->init();
	sphere_12->setApperance(*appearance_12);
	sphere_12->init();
	sphere_13->setApperance(*appearance_13);
	sphere_13->init();

	GLAppearance* appearance_heart = new GLAppearance("../../data/shaders/multi_vertex_lights.vs", "../../data/shaders/multi_vertex_lights.fs");
	GLDirectLightSource  light_source_heart;
	light_source_heart._lightPos = glm::vec4(200000.0, 0.0, 0.0, 0.0);
	light_source_heart._ambient_intensity =0.5;
	light_source_heart._specular_intensity = 0.1;
	light_source_heart._diffuse_intensity = 0.8;
	light_source_heart._attenuation_coeff = 0.08;
	GLMaterial material_heart;
	material_heart._diffuse_material = glm::vec3(0.890, 0.050, 0.145);
	material_heart._ambient_material = glm::vec3(0.890, 0.050, 0.145);
	material_heart._specular_material = glm::vec3(0.890, 0.050, 0.145);
	material_heart._shininess = 10.0;
	material_heart._transparency = 1;
	material_heart._emissive_material = glm::vec3(0.890, 0.050, 0.145);
	appearance_heart->setMaterial(material_heart);
	appearance_heart->addLightSource(light_source_heart);
	GLSphere3D* sphere_heart_0 = new GLSphere3D(-3.0, 0.0, -1.0, 0.1, 100, 100);
	GLSphere3D* sphere_heart_1 = new GLSphere3D(-3.0, 0.0, 0.0, 0.1, 100, 100);
	GLSphere3D* sphere_heart_2 = new GLSphere3D(-3.0, 0.0, 1.0, 0.1, 100, 100);
	sphere_heart_0->setApperance(*appearance_heart);
	sphere_heart_0->init();
	sphere_heart_1->setApperance(*appearance_heart);
	sphere_heart_1->init();
	sphere_heart_2->setApperance(*appearance_heart);
	sphere_heart_2->init();
	appearance_heart->finalize();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// This defines two matrices, one for translation and one for rotations
	glm::mat4 model_matrix = glm::mat4(1.0);

	//keyboard callback
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);//poll the system for the current state of the key

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Main render loop
    
    // Set up our green background color
   
	g_change_texture_blend = 1;
    // This sets the camera to a new location
    // the first parameter is the eye position, the second the center location, and the third the up vector. 
    
    
	//SetViewAsLookAt(glm::vec3(1.0f, 1.0f, 25.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // Enable depth test
    // ignore this line, it allows us to keep the distance value after we proejct each object to a 2d canvas.
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float pre_time = glfwGetTime();//the time of last frame
		// Ensure that we can notice the escape key being pressed
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
		while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 )
    {
        // Clear the entire buffer with our green color (sets the background to be green).
			glClearBufferfv(GL_COLOR , 0, clear_color);
			glClearBufferfv(GL_DEPTH , 0, clear_depth);
			//////////////////////////////////////////////////////////////////
			// Interpolate between keyframes
			Keyframe k0_0, k1_0, k_res_0;
			Keyframe k0_1, k1_1, k_res_1;
			float time = glfwGetTime();
			delta_time = time - pre_time;//use to compute FPS	
//			k_res.print();			
			switch (scene) {
				case 0: {

					if (glfwGetTime() - pre_time >= 1){
						pre_time = glfwGetTime();
						g_change_texture_blend++;
						if (g_change_texture_blend >= 3)g_change_texture_blend = 1;
					}
					bool ret = texture->setTextureBlendMode(g_change_texture_blend);
					if (ret)appearance_tex->updateTextures();
					SetViewAsLookAt(glm::vec3(0.2f, -6.45f, 0.2f), glm::vec3(0.2f, -5.45f, 0.2f), glm::vec3(0.0f, 0.0f, 1.0f));
					plane_tex->draw();
					break;									
				}
				case 1: {				
					sphere_2->draw();
					SetViewAsLookAt(glm::vec3(1.0f, 1.0f, 25.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));		
					bird_scale = glm::scale(glm::vec3(0.15f, 0.15, 0.15f));
					sphere_1->draw();
					float f_0 = getTimeFraction(time, 10.0); // we assume that the animation takes 10 seconds
					int num_0 = getKeyframes(startSceneAnimation, f_0, k0_0, k1_0);
					bool ret_0 = interpolateKeyframe(f_0, k0_0, k1_0, k_res_0);
					model_matrix = glm::translate(k_res_0._p)* glm::mat4_cast(k_res_0._q);				
					appearance_tex->updateTextures();
					tree->setModelMatrix(glm::translate(k_res_0._p)*glm::mat4_cast(k_res_0._q)*glm::scale(glm::vec3(0.8f, 0.8, 0.8f)));
					tree->draw();
					bird_move = glm::vec3(-bird_forward, 0.0, -bird_horizon);
					bird->setModelMatrix(model_matrix*bird_scale);
					appearance->updateTextures();
					bird->draw();
					pre_time = time;
					sphere_1->draw();
					sphere_10->setModelMatrix(glm::translate(glm::vec3(-2, 6, 2)));
					sphere_10->draw();
					sphere_11->setModelMatrix(glm::translate(glm::vec3(4, 4, -1)));
					sphere_11->draw();
					sphere_12->setModelMatrix(glm::translate(glm::vec3(2.5, 7, 3)));
					sphere_12->draw();//green
					sphere_13->setModelMatrix(glm::translate(glm::vec3(-1.5, 5, -2)));
					sphere_13->draw();
					plane_0->draw();
					break;
				}
				case 2:{
					bird_scale = glm::scale(glm::vec3(0.05f, 0.05, 0.05f));
					//pre_bird_height = bird_height;
					//bird_height -= GRAVITY*(glfwGetTime() - last_fly_time)*delta_time;
					//bird_move = glm::vec3(0.0, bird_height, 0.0);
					sphere_2->setModelMatrix(glm::translate(glm::vec3(-bird_forward, 0.0, 0)));
					sphere_2->draw();
					pre_bird_forward = bird_forward;
					bird_forward += delta_time*RUN_SPEED;					
					for (int i = 0; i < tree_number; i++){
						if ((abs(bird_horizon - (-tree_horizon[i] + 19)) < 1.74 || abs(bird_horizon - (-tree_horizon[i] - 15.74)) < 1.25) && abs(bird_forward -tree_loc[i])< 2.5){
							if (i != last_bump_tree && bump_flag == false){
								bump_flag = true;
								last_bump_tree = i;
								bump++;
								cout << "You bump into tree at " << bird_forward << endl;
							}								
						}
						if (tree_loc[i] < bird_forward+2){
							tree_loc[i] += TREE_DISTANCE * tree_number;
							bump_flag = false;
						}
						Obtree[i]->setModelMatrix(glm::translate(glm::vec3(-tree_loc[i], 0.0, tree_horizon[i]))*glm::mat4_cast(angleAxis(3.14f / 2, glm::vec3(0.0, 1.0, 0.0))));
						Obtree[i]->draw();
						
					}
					bird_move = glm::vec3(-bird_forward, 0.0, -bird_horizon);
					float f_1 = getTimeFraction(time, 0.9); // we assume that the animation takes 0.5 seconds
					initKeyframeAnimation();//update table
					int num_1 = getKeyframes(myKeyframes, f_1, k0_1, k1_1);
					bool ret_1 = interpolateKeyframe(f_1, k0_1, k1_1, k_res_1);
					//model_matrix = glm::translate(bird_move)*glm::translate(k_res_1._p)*glm::mat4_cast(k_res_1._q)*bird_scale;
					model_matrix = glm::translate(bird_move)*glm::translate(k_res_1._p)*glm::mat4_cast(k_res_1._q)*bird_scale;
					SetViewAsLookAt(glm::vec3(-bird_forward + 15.5, 1.0f, -bird_horizon), glm::vec3(-bird_forward + 14.0f, 1.0f, -bird_horizon), glm::vec3(0.0f, 1.0f, 0.0f));
					plane_0->setModelMatrix(glm::translate(bird_move));//shift the plane according to object location
					plane_0->draw();
					plane_1->setModelMatrix(glm::translate(glm::vec3(-bird_forward, 0.0, MAP_BOUNDARY))*glm::mat4_cast(angleAxis(3.14f / 2 + 0.2f, glm::vec3(1.0, 0.0, 0.0))));
					plane_1->draw();
					plane_2->setModelMatrix(glm::translate(glm::vec3(-bird_forward, 0.0, -MAP_BOUNDARY))*glm::mat4_cast(angleAxis(3.14f / 2 - 0.2f, glm::vec3(1.0, 0.0, 0.0))));
					plane_2->draw();
					bird->setModelMatrix(model_matrix);
					appearance->updateTextures();
					bird->draw();
					sphere_heart_0->setModelMatrix(glm::translate(glm::vec3(-bird_forward + 15, 0.3, -bird_horizon+0.001)));
					if (bump<1) sphere_heart_0->draw();
					sphere_heart_1->setModelMatrix(glm::translate(glm::vec3(-bird_forward + 15, 0.3, -bird_horizon)));
					if (bump<2)sphere_heart_1->draw();
					sphere_heart_2->setModelMatrix(glm::translate(glm::vec3(-bird_forward + 15, 0.3, -bird_horizon-0.001)));
					if (bump<3) sphere_heart_2->draw();
					pre_time = time;
					if (bump >= 3) {
						cout << "Your Kept Running for " << bird_forward <<" Meters !"<< endl<<endl;
						scene = 3;
						appearance_tex = new GLAppearance("../../data/shaders/multi_texture.vs", "../../data/shaders/multi_texture.fs");
						GLDirectLightSource  light_source_tex;
						light_source_tex._lightPos = glm::vec4(00.0, 20.0, 20.0, 0.0);
						appearance_tex->addLightSource(light_source);	
						appearance_tex->setMaterial(material_tex);
						texture = new GLMultiTexture();
						int texid = texture->loadAndCreateTextures("../../data/textures/gameover.bmp", "../../data/textures/deerrun.bmp", "../../data/textures/title.bmp");
						appearance_tex->setTexture(texture);
						appearance_tex->finalize();
						plane_tex = new GLPlane3D(0.0, 0.0, 0.0, 10.0, 10.0);
						plane_tex->setApperance(*appearance_tex);
						plane_tex->init();
					}
					break;
				}			
				case 3:{
					SetTrackballLocation(trackball.getRotationMatrix());
					SetViewAsLookAt(glm::vec3(0.0f, -7.0f, 0.3f), glm::vec3(0.0f, -5.8f, 0.3f), glm::vec3(0.0f, 0.0f, 1.0f));
					g_change_texture_blend = 0;
					bool ret = texture->setTextureBlendMode(g_change_texture_blend);
					if (ret)appearance_tex->updateTextures();
					plane_tex->draw();
					bump = 0;
					bird_forward = 0;
					bird_horizon = 0;
					break;
				}
				default: break;
			}
		//	cs->draw();
			glfwSwapBuffers(window);
			glfwPollEvents();
    } // Check if the ESC key was pressed or the window was closed  
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
   
    return 0;
}

