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

#include "BirdModel.h"
#include "HCI557Common.h"
#include "Plane3D.h"

using namespace std;

class Game {

private:

	GLDirectLightSource _default_light_source;
	GLSpotLightSource _spot_light_source;
	
	GLMaterial _bird_material;
	GLMaterial _block_material;

	void setInitPicking(BirdModel* obj);

	GLPlane3D * _ground_layer;
	
	const static glm::vec3 moveUp;
	const static glm::vec3 moveDown;
	const static glm::vec3 moveLeft;
	const static glm::vec3 moveRight;
	
	const static glm::vec3 rayUp;
	const static glm::vec3 rayDown;
	const static glm::vec3 rayLeft;
	const static glm::vec3 rayRight;

public:

	Game();

	ChessPiece* getChessPiecesAt(int idx);
	void build_bird();
	void build_obstacle();
	void setup_light_and_material();
	void draw();
	
	void handleKeyPress(int, int);
	void handleCollision();

};