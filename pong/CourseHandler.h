#ifndef COURSEHANDLER_H
#define COURSEHANDLER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

class CourseHandler
{
public:
	//creates a level returning the positions of the walls
	static glm::vec4 MakeCourse(glm::vec4 padding, GLfloat thickness);

private:
	CourseHandler();
};

#endif
