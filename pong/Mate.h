#ifndef MATE_H
#define MATE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class Mate
{
public:
	static GLfloat map(GLfloat in, GLfloat inmin, GLfloat inmax, GLfloat outmin, GLfloat outmax);
	static GLfloat abs(GLfloat in);
	static GLfloat clamp(GLfloat in, GLfloat min, GLfloat max);
	
private:
	Mate();
};

#endif