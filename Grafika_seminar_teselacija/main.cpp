#include"main.h"

int main()
{
	init();
	createContext();
	glEnableConfig();
	initRender();

	//zakomentirati za iskljucit teselaciju
	glPatchParameteri(GL_PATCH_VERTICES, 4); // Quads
	//----------------------------------------
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.075f, 0.133f, 0.173f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		
		glDisable(GL_CULL_FACE);
		
		double cursorPos[2];
		glfwGetCursorPos(window, &cursorPos[0], &cursorPos[1]);
		glProgramUniform2f(
			shaderProgram, 0, 
			float(cursorPos[0]) / SCREEN_WIDTH, 1.0 - float(cursorPos[1]) / SCREEN_HEIGHT
		);
		glBindVertexArray(VAO);

		glDrawElements(GL_PATCHES, indices.size(), GL_UNSIGNED_INT, 0); // GL_LINE_STRIP za iskljuciti teselaciju

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

