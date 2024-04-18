
#ifndef AE_ENABLE_GLFW
# error AE_ENABLE_GLFW required
#endif

#include "GLFW/glfw3.h"

int main ()
{
	if ( glfwInit() != GLFW_TRUE )
		return 1;

	if ( glfwVulkanSupported() != GLFW_TRUE )
		return 1;

	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );

	auto* wnd = glfwCreateWindow( 800, 600, "title", nullptr, nullptr );
	if ( !wnd )
		return 1;

	glfwDestroyWindow( wnd );
	glfwTerminate();

	return 0;
}
