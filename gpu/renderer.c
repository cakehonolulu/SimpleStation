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
							  1024, 512, SDL_WINDOW_OPENGL);
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

	glewInit();

	glEnable(GL_DEBUG_OUTPUT);

	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, 1024, 512);

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

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	GLsizei stride = sizeof(Vertex);
	uint64_t offset = 0;

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	GLsizeiptr elementSize = sizeof(Vertex);
	GLsizeiptr bufferSize = elementSize * VERTEX_BUFFER_LEN;

	glBufferStorage(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
	m_vertex_buffer = (Vertex *) glMapBufferRange(GL_ARRAY_BUFFER, 0, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

	memset(m_vertex_buffer, 0, bufferSize);


	glBindAttribLocation(program, 0, "vertex_position");
	glEnableVertexAttribArray(0);
	glVertexAttribIPointer(0, 2, GL_SHORT, stride, (void*)offset);
	offset += sizeof(Position);

	glBindAttribLocation(program, 1, "vertex_color");
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, stride, (void*)offset);
	offset += sizeof(Colour);

	glBindAttribLocation(program, 2, "texture_page");
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 2, GL_UNSIGNED_SHORT, stride, (void*)offset);
	offset += sizeof(TexPage);

	glBindAttribLocation(program, 3, "texture_coord");
	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 2, GL_UNSIGNED_BYTE, stride, (void*)offset);
	offset += sizeof(TexCoord);

	glBindAttribLocation(program, 4, "clut");
	glEnableVertexAttribArray(4);
	glVertexAttribIPointer(4, 2, GL_UNSIGNED_SHORT, stride, (void*)offset);
	offset += sizeof(ClutAttr);

	glBindAttribLocation(program, 5, "texture_depth");
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 1, GL_UNSIGNED_BYTE, stride, (void*)offset);
	offset += sizeof(TextureColourDepth);

	glBindAttribLocation(program, 6, "texture_blend_mode");
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 1, GL_UNSIGNED_BYTE, stride, (void*)offset);
	offset += sizeof(GLubyte);

	/*glGenTextures(1, &vramTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, vramTexture);
	glUniform1i(glGetUniformLocation(program, "vramTexture"), 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);*/

	glUseProgram(program);
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
