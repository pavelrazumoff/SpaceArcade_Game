#pragma once
#define GLM_FORCE_CTOR_INIT

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <random>
#include <map>
#include <time.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <irrKlang.h>
using namespace irrklang;

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

//#define m_new new

#if defined(DEBUG) || defined(_DEBUG)
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif  // _DEBUG

#if defined(DEBUG) || defined(_DEBUG)
#define m_free(A) _dbgfree( __FILE__, __LINE__, (A) )
#endif  // _DEBUG

struct Character {
	GLuint TextureID; // ID handle of the glyph texture
	glm::ivec2 Size; // Size of glyph
	glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
	GLuint Advance; // Offset to advance to next glyph
};

struct Font
{
	std::map<GLchar, Character> characters;
	int fontSize;
};

float lerp(float a, float b, float f);

void healthBarChanged(float health, float initialHealth);
void energyBarChanged(float usedEnergy, float maxEnergy);
void rocketIntegrityChanged(int integrity, int maxIntegrity);