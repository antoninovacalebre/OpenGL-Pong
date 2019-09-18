#include "Mate.h"
#include "iostream"

GLfloat Mate::map(GLfloat in, GLfloat inmin, GLfloat inmax, GLfloat outmin, GLfloat outmax)
{
	std::cout << ((in - inmin) / (inmax - inmin)) << std::endl;
	return outmin + ((in - inmin) / (inmax - inmin)) * (outmax - outmin);
}

GLfloat Mate::abs(GLfloat in)
{
	return (in >= 0) ? in : -in;
}

GLfloat Mate::clamp(GLfloat in, GLfloat min, GLfloat max)
{
	if (in < min)	return min;
	if (in > max)	return max;
	return in;	
}