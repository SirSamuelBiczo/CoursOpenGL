#include "SceneOpenGL.h"

int main()
{
	SceneOpenGL scene("OpenGL", 800, 600);

	if (!scene.Initialize())
	{
		std::cerr << "Scene connot be init" << std::endl;
		return -1;
	}

	return scene.Run();
}