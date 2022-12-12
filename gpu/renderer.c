#include <gpu/renderer.h>

SDL_Window   *m_window;
SDL_GLContext *m_gl_context;

RendererColor *colorsBuffer = NULL;
RendererPosition *positionsBuffer = NULL;

GLuint colorsObject;
GLuint positionsObject;

GLuint vertex_arrays = 0;

GLuint vertex_shader = 0;
GLuint fragment_shader = 0;
GLuint program = 0;

uint32_t count_vertices = 0;

uint8_t m_renderer_init(m_simplestation_state *m_simplestation)
{
	GLint status = 0;
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL2: %s", SDL_GetError());
        return EXIT_FAILURE;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	m_window = SDL_CreateWindow("SimpleStation (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  640, 480, SDL_WINDOW_OPENGL);
						  // 1024, 512

	m_gl_context = SDL_GL_CreateContext(m_window);

	SDL_GL_SetSwapInterval(1);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(m_window);

	vertex_shader = renderer_LoadShader("vertex.glsl", GL_VERTEX_SHADER);
	fragment_shader = renderer_LoadShader("fragment.glsl", GL_FRAGMENT_SHADER);

	// Create program
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if(status != GL_TRUE) {
    printf("OpenGL program linking failed!\n");
    glDeleteProgram(program);
    return -1;
  }

	glDetachShader(program, vertex_shader);
	glDetachShader(program, fragment_shader);
	glUseProgram(program);

	m_renderer_buffers_init(m_simplestation);
}

uint8_t m_renderer_buffers_init(m_simplestation_state *m_simplestation)
{
	// Create vertex buffer
	glGenVertexArrays(1, &vertex_arrays);
	glBindVertexArray(vertex_arrays);

	
	{
		glGenBuffers(1, &positionsObject);
		glBindBuffer(GL_ARRAY_BUFFER, positionsObject);

		// Compute size
		GLsizeiptr buffer_size = (sizeof(RendererPosition)) * VERTEX_BUFFER_LEN;
		GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

		glBufferStorage(GL_ARRAY_BUFFER, buffer_size, NULL, flags);
		positionsBuffer = (RendererPosition *) glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);

		memset(positionsBuffer, 0, buffer_size);

		GLuint index = glGetAttribLocation(program, "vertex_position");
		glEnableVertexAttribArray(index);
		glVertexAttribIPointer(index, 2, GL_SHORT, 0, NULL);
	}

	{
		// Colors buffer
		glGenBuffers(1, &colorsObject);
		glBindBuffer(GL_ARRAY_BUFFER, colorsObject);

		GLsizeiptr buffer_size = (sizeof(RendererColor)) * VERTEX_BUFFER_LEN;
		GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;

		glBufferStorage(GL_ARRAY_BUFFER, buffer_size, NULL, flags);
		colorsBuffer = (RendererColor *) glMapBufferRange(GL_ARRAY_BUFFER, 0, buffer_size, flags);

		memset(colorsBuffer, 0, buffer_size);

		// Colors attr
		GLuint index = glGetAttribLocation(program, "vertex_color");
		glEnableVertexAttribArray(index);
		glVertexAttribIPointer(index, 3, GL_UNSIGNED_BYTE, 0, NULL);
	}
}

uint64_t readFile(char *filePath, char **contents) {
    uint64_t length = 0;
    FILE *file = fopen(filePath, "r");
    if (file) {
        // Check length
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        // Back to beginning of file
        fseek(file, 0, SEEK_SET);
        *contents = malloc(sizeof(char) * length + 1);
        if (*contents != NULL) {
            fread(*contents, 1, length, file);
			(*contents)[length] = '\0';
        }
        fclose(file);
    }
    return length;
}
GLuint renderer_LoadShader(char *path, GLenum type) {
	char *data = NULL;
	readFile(path, &data);
	
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, (const char**)&data, NULL);
	glCompileShader(shader);
	GLint status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		printf("Failed to compile shader %s\n", path);
		exit(1);
	}
	return shader;
}

static GLuint find_program_attrib(GLuint program, const char *attr) {
  GLuint attr_index = glGetAttribLocation(program, attr);

  if(attr_index < 0) {
    printf("Attribute '%s' not found in program\n", attr);
  }

  return attr_index;
}

void draw() {
  glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (count_vertices));

  GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);

  GLenum status;
  do {
    status = glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 10000000);
  } while (status != GL_ALREADY_SIGNALED && status != GL_CONDITION_SATISFIED);

  count_vertices = 0;
}

void display() {
  draw();
  SDL_GL_SwapWindow(m_window);
}

RendererPosition pos_from_gp0(uint32_t val) {
	RendererPosition pos;
	int16_t a = val;
	int16_t b = val >> 16;
	pos.x = a;
	pos.y = b;
	return pos;
}

RendererColor color_from_gp0(uint32_t val) {
	RendererColor col;
	uint8_t a = val;
	uint8_t b = val >> 8;
	uint8_t c = val >> 16;
	col.r = a;
	col.g = b;
	col.b = c;
	return col;
}

int put_triangle(const RendererPosition positions[3], const RendererColor colors[3]) {
  if (count_vertices + 3 >= VERTEX_BUFFER_LEN) {
    printf("Vertex attribute buffers full, forcing_draw\n");
    draw();
  }

  for (int i = 0; i < 3; ++i) {
    positionsBuffer[count_vertices] = positions[i];
    colorsBuffer[count_vertices] = colors[i];
    ++count_vertices;
  }

  return 0;
}

int put_quad(const RendererPosition positions[4], const RendererColor colors[4]) {
  if (count_vertices + 6 >= VERTEX_BUFFER_LEN) {
    printf("Vertex attribute buffers full, forcing_draw\n");
    draw();
  }

  for (int i = 0; i < 3; ++i) {
    positionsBuffer[count_vertices] = positions[i];
    colorsBuffer[count_vertices] = colors[i];
    ++count_vertices;
  }

  for (int i = 1; i < 4; ++i) {
    positionsBuffer[count_vertices] = positions[i];
    colorsBuffer[count_vertices] = colors[i];
    ++count_vertices;
  }

  return 0;
}