#include <gpu/renderer.h>

SDL_Window   *m_window;
SDL_GLContext *m_gl_context;

// OpenGL's VAO (Vertex Array Object)
GLuint m_vao;

// OpenGL's VBO (Vertex Buffer Object)
GLuint m_vbo;

Vertex *m_vertex_buffer = NULL;

// GLSL Vertex Shader
GLuint vertex_shader = 0;

// GLSL Fragment Shader
GLuint fragment_shader = 0;

// GLSL Program
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


	m_window = SDL_CreateWindow("SimpleStation (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  640, 480, SDL_WINDOW_OPENGL);
						  // 1024, 512

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	m_gl_context = SDL_GL_CreateContext(m_window);

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		printf("[GLEW] glewInit(): %s\n", glewGetErrorString(err));
		m_simplestation_exit(m_simplestation, 1);
	}
	printf("[GLEW] glewInit(): Using GLEW %s\n", glewGetString(GLEW_VERSION));


	glEnable(GL_DEBUG_OUTPUT);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, 640, 480);

	// Initialize the buffers
	m_renderer_buffers_init();

	vertex_shader = renderer_LoadShader("vertex.glsl", GL_VERTEX_SHADER);
	fragment_shader = renderer_LoadShader("fragment.glsl", GL_FRAGMENT_SHADER);

	// Create program
	program = glCreateProgram();

	// Attatch the compiled shaders
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	// Linke the program...
	glLinkProgram(program);

	status = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if(status != GL_TRUE)
	{
    	printf("OpenGL program linking failed!\n");
    	glDeleteProgram(program);
    	return -1;
	}

	// ...and run it!
	glUseProgram(program);
}

void m_renderer_buffers_init()
{
	// Generate the Vertex Arary Object
	glGenVertexArrays(1, &m_vao);

	// Bind to it
	glBindVertexArray(m_vao);

	// Generate the Vertex Buffer Object
	glGenBuffers(1, &m_vbo);

	// Bind to it
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	/* Allocate the buffer for use in the emulator */

	// Get the size of the structure
	GLsizeiptr elementSize = sizeof(Vertex);

	// Find the total buffer size
	GLsizeiptr bufferSize = elementSize * VERTEX_BUFFER_LEN;

	// Setup the buffer...
	glBufferStorage(GL_ARRAY_BUFFER, bufferSize, NULL, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
	m_vertex_buffer = (Vertex *) glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

	// ...and clear it
	memset(m_vertex_buffer, 0, bufferSize);

	/* Setup OpenGL Vertex Attributes */
	glVertexAttribIPointer(0, 2, GL_SHORT, sizeof(Vertex), (void *) offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (void *) offsetof(Vertex, colour));
	glEnableVertexAttribArray(1);

	glVertexAttribIPointer(2, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), (void *) offsetof(Vertex, texPage));
	glEnableVertexAttribArray(2);

	glVertexAttribIPointer(3, 2, GL_UNSIGNED_BYTE, sizeof(Vertex), (void *) offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(3);

	glVertexAttribIPointer(4, 2, GL_UNSIGNED_SHORT, sizeof(Vertex), (void *) offsetof(Vertex, clut));
	glEnableVertexAttribArray(4);

	glVertexAttribIPointer(5, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void *) offsetof(Vertex, texDepth));
	glEnableVertexAttribArray(5);

	glVertexAttribIPointer(6, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (void *) offsetof(Vertex, blendMode));
	glEnableVertexAttribArray(6);
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

Position pos_from_gp0(uint32_t value)
{
	Position pos;
	pos.x = (GLshort) (value & 0xFFFF);
	pos.y = (GLshort) (value >> 16);
	return pos;
}

Colour col_from_gp0(uint32_t value)
{
	Colour col;
	col.r = (GLubyte) (value & 0xFF);
	col.g = (GLubyte) ((value >> 8) & 0xFF);
	col.b = (GLubyte) ((value >> 16) & 0xFF);
	return col;
}

TexPage texpage_from_gp0(uint32_t value)
{
	TexPage texp;
	texp.xBase = ((value >> 16) & 0xF) * 64;
	texp.yBase = (((value >> 16) >> 4) & 1) * 256;
	return texp;
}

TexCoord texcoord_from_gp0(uint32_t value)
{
	TexCoord texc;
	texc.x = value & 0xFF;
	texc.y = (value >> 8) & 0xFF;
	return texc;
}

ClutAttr clutattr_from_gp0(uint32_t value)
{
	ClutAttr clut;
	clut.x = ((value >> 16) & 0x3F) * 16;
	clut.y = ((value >> 16) >> 6) & 0x1FF;
	return clut;
}

TextureColourDepth tcd_from_gp0(uint32_t value)
{
	TextureColourDepth tcd;
	tcd.depth = ((value >> 16) >> 7) & 0x3;
	return tcd;
}

TextureColourDepth tcd_from_val(textureColourDepthValue value)
{
	TextureColourDepth tcd;
	tcd.depth = (GLubyte) value;
	return tcd;
}

Colour color(GLubyte r, GLubyte g, GLubyte b) {
	Colour colorr;

	colorr.r = r;
	colorr.g = g;
	colorr.b = b;
	return colorr;
}

int put_triangle(Vertex v1, Vertex v2, Vertex v3) {
	if (count_vertices + 3 > VERTEX_BUFFER_LEN)
	{
		printf("Vertex attribute buffers full, forcing_draw\n");
		draw();
	}

	m_vertex_buffer[count_vertices] = v1;
	count_vertices++;

	m_vertex_buffer[count_vertices] = v2;
	count_vertices++;
	
	m_vertex_buffer[count_vertices] = v3;
	count_vertices++;
}

int put_quad(Vertex v1, Vertex v2, Vertex v3, Vertex v4) {
	put_triangle(v1, v2, v3);
	put_triangle(v2, v3, v4);
	return 0;
}
