#include <gpu/renderer.h>

SDL_Window   *m_window;
SDL_GLContext *m_gl_context;

uint8_t m_renderer_init(m_simplestation_state *m_simplestation)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL2: %s", SDL_GetError());
        return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	m_window = SDL_CreateWindow("SimpleStation (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  1024, 512, SDL_WINDOW_OPENGL);

	m_gl_context = SDL_GL_CreateContext(m_window);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(m_window);
}
