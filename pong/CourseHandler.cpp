#include "CourseHandler.h"

glm::vec4 CourseHandler::MakeCourse(glm::vec4 padding, GLfloat thickness)
{
	glm::vec4 out_positions(
		padding.w,
		padding.x,
		1000 - thickness - padding.y,
		1000 - thickness - padding.z
	);

	return out_positions;
}
