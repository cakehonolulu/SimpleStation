#include <renderer/opengl.h>
#include <simplestation.h>

// OpenGL's VAO (Vertex Array Object)
GLuint m_vao;

// OpenGL's VBO (Vertex Buffer Object)
GLuint m_vbo;

// Custom Vertex Array containing draw attributes
OpenGL_Vertex *m_vertex_buffer = NULL;

/* OpenGL Framebuffers */

// Original, OpenGL-provided framebuffer (Visible)
GLint m_original_fbo;

// Custom off-screen framebuffer
GLuint m_fbo;

/* OpenGL Textures */

// Texture containing the off-screen framebuffer
GLuint m_psx_vram_texel;

// Texture for the default (Visible) framebuffer
GLuint m_window_texture;

GLuint m_psx_gpu_vram;
/* OpenGL Shader Programs */

// GLSL Off-Screen Program
GLuint program = 0;

// GLSL Visible Program
GLuint fb_program = 0;

// GLSL Off-Screen Vertex Shader
GLuint vertex_shader = 0;

// GLSL Off-Screen Fragment Shader
GLuint fragment_shader = 0;

// GLSL (Visible) Vertex Shader
GLuint fb_vertex_shader = 0;

// GLSL (Visible) Fragment Shader
GLuint fb_fragment_shader = 0;

/* OpenGL Uniforms */

GLint uniform_offset;

/* Auxiliary variables */
uint32_t count_vertices = 0;

float output_window_vertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
};


SDL_Window   *m_window;
SDL_GLContext *m_gl_context;


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

RectWidthHeight rwh_from_gp0(uint32_t value)
{
	RectWidthHeight rwh;
	rwh.width = (GLshort)(value & 0xFFFF);
	rwh.height = (GLshort)(value >> 16);
	return rwh;
}

Colour color(GLubyte r, GLubyte g, GLubyte b) {
	Colour colorr;

	colorr.r = r;
	colorr.g = g;
	colorr.b = b;
	return colorr;
}
void m_window_changetitle(char *buffer)
{
	SDL_SetWindowTitle(m_window, buffer);
}

unsigned int output_window_vao, output_window_vbo;

uint8_t init_opengl_renderer(m_simplestation_state *m_simplestation)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL2: %s", SDL_GetError());
        return EXIT_FAILURE;
	}

	if (m_simplestation->m_vramview)
	{
		m_window = SDL_CreateWindow("SimpleStation (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  1024, 512, SDL_WINDOW_OPENGL);
	}
	else
	{
		m_window = SDL_CreateWindow("SimpleStation (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  640, 480, SDL_WINDOW_OPENGL);
	}
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	m_gl_context = SDL_GL_CreateContext(m_window);

	SDL_GL_MakeCurrent(m_window, m_gl_context);

	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		printf("[GLEW] glewInit(): %s\n", glewGetErrorString(err));
		m_simplestation_exit(m_simplestation, 1);
	}
	printf("[GLEW] glewInit(): Using GLEW %s\n", glewGetString(GLEW_VERSION));

	glEnable(GL_DEBUG_OUTPUT);

	// Store the original (Visible) framebuffer into an easily-accessible variable
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_original_fbo);

	m_renderer_setup_onscreen();

	m_renderer_setup_offscreen(m_simplestation);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef DUMP_VRAM
	m_simplestation->m_vram_data = (uint8_t *) malloc(sizeof(uint8_t[1024 * 1024 * 4]));
#endif

	return 0;
}

void m_renderer_setup_onscreen()
{
	/* OpenGL On-Screen Framebuffer Configuration */

	GLint m_opengl_status = 0;

	/* Setup the on-screen VAO and VBO */

	// First generate the Vertex Array...
	glGenVertexArrays(1, &output_window_vao);

	// ... then generate the Vertex Buffer.
	glGenBuffers(1, &output_window_vbo);

	// Bind to the new Vertex Array
	glBindVertexArray(output_window_vao);

	// Also bind to the Vertex Buffer...
	glBindBuffer(GL_ARRAY_BUFFER, output_window_vbo);

	// ...and tell the GPU which data to grab-off the memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(output_window_vertices), &output_window_vertices, GL_STATIC_DRAW);

	// Specify the values for the VAO (2 triangles)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	// Compile the default framebuffer's shaders
	fb_vertex_shader = renderer_LoadShader("fb_vertex.glsl", GL_VERTEX_SHADER);
	fb_fragment_shader = renderer_LoadShader("fb_fragment.glsl", GL_FRAGMENT_SHADER);

	// Create the default framebuffer program
	fb_program = glCreateProgram();

	// Attatch the compiled shaders
	glAttachShader(fb_program, fb_vertex_shader);
	glAttachShader(fb_program, fb_fragment_shader);

	// Link the program...
	glLinkProgram(fb_program);

	glGetProgramiv(fb_program, GL_LINK_STATUS, &m_opengl_status);

	if(m_opengl_status != GL_TRUE)
	{
    	printf("OpenGL program linking failed!\n");
    	glDeleteProgram(fb_program);
    	exit(1);
	}

	// ...and run it!
	glUseProgram(fb_program);

	glUniform1i(glGetUniformLocation(fb_program, "screenTexture"), 0);
}

void m_renderer_setup_offscreen(m_simplestation_state *m_simplestation)
{
	/* OpenGL Off-Screen Framebuffer Configuration */

	GLint m_opengl_status = 0;

	// Generate a new Framebuffer Object...
	glGenFramebuffers(1, &m_fbo);

	// ...and bind to it
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_psx_vram_texel);
	glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// The newly-created texture will point at the pixel data provided by the last-bound texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_vram_texel, 0);

	// Unbind from the newly-created texture
	glBindTexture(GL_TEXTURE_2D, 0);
	
	// Check if the custom framebuffer was correctly created
	m_opengl_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (m_opengl_status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("[OPENGL] Framebuffer could not be created (%d)! Exiting...\n", m_opengl_status);
		exit(1);
	}
	
	vertex_shader = renderer_LoadShader("vertex.glsl", GL_VERTEX_SHADER);
	fragment_shader = renderer_LoadShader("fragment.glsl", GL_FRAGMENT_SHADER);

	// Create program
	program = glCreateProgram();

	// Attatch the compiled shaders
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	uniform_offset = glGetUniformLocation(program,
                                     "offset");

	glUniform2i(uniform_offset, 0, 0);

	// Link the program...
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &m_opengl_status);

	if(m_opengl_status != GL_TRUE)
	{
    	printf("OpenGL program linking failed!\n");
    	glDeleteProgram(program);
    	exit(1);
	}

	glGenTextures(1, &m_psx_gpu_vram);
	glBindTexture(GL_TEXTURE_2D, m_psx_gpu_vram);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 512, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Initialize the buffers
	m_renderer_buffers_init();

	// Generate a new texture that will contain the off-screen's pixel data...
	glGenTextures(1, &m_window_texture);

	// ...bind to it...
	glBindTexture(GL_TEXTURE_2D, m_window_texture);

	// ...allocate space for it...
	if (m_simplestation->m_vramview)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	}

	// ...and set the appropiate parameters (To fill the screen and the texture filters)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ...and run it!
	glUseProgram(program);
}

void m_renderer_buffers_init()
{
	m_vertex_buffer = (OpenGL_Vertex *) malloc(sizeof(OpenGL_Vertex) * VERTEX_BUFFER_LEN);

	// Generate the Vertex Arary Object
	glGenVertexArrays(1, &m_vao);
	
	// Bind to it
	glBindVertexArray(m_vao);

	// Generate the Vertex Buffer Object
	glGenBuffers(1, &m_vbo);

	// Bind to it
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(OpenGL_Vertex) * VERTEX_BUFFER_LEN, m_vertex_buffer, GL_DYNAMIC_DRAW);

	/* Setup OpenGL Vertex Attributes */
	glVertexAttribIPointer(0, 2, GL_SHORT, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, colour));
	glEnableVertexAttribArray(1);

	glVertexAttribIPointer(2, 2, GL_UNSIGNED_SHORT, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, texPage));
	glEnableVertexAttribArray(2);

	glVertexAttribIPointer(3, 2, GL_UNSIGNED_BYTE, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, texCoord));
	glEnableVertexAttribArray(3);

	glVertexAttribIPointer(4, 2, GL_UNSIGNED_SHORT, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, clut));
	glEnableVertexAttribArray(4);

	glVertexAttribIPointer(5, 1, GL_UNSIGNED_BYTE, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, texDepth));
	glEnableVertexAttribArray(5);

	glVertexAttribIPointer(6, 1, GL_UNSIGNED_BYTE, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, blendMode));
	glEnableVertexAttribArray(6);

	glVertexAttribIPointer(7, 1, GL_UNSIGNED_BYTE, sizeof(OpenGL_Vertex), (void *) offsetof(OpenGL_Vertex, drawTexture));
	glEnableVertexAttribArray(7);
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
            if (fread(*contents, 1, length, file) == length)
			{
				(*contents)[length] = '\0';
			}
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
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		GLchar infoLog[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		printf("Error: %s\n", infoLog);

		exit(1);
	}

	free(data);
	return shader;
}

int display_area_x = 0, display_area_y = 0, display_area_width = 0, display_area_height = 0;

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

int res_w, res_h;

void m_renderer_update_display_area(m_simplestation_state *m_simplestation)
{
	display_area_x = m_simplestation->m_gpu->m_drawing_area_left;
    display_area_width = max(m_simplestation->m_gpu->m_drawing_area_right - display_area_x + 1, 0);
    display_area_y = m_simplestation->m_gpu->m_drawing_area_top;
    display_area_height = max(m_simplestation->m_gpu->m_drawing_area_bottom - m_simplestation->m_gpu->m_drawing_area_top + 1, 0);
	
	//printf("Display area:\nx -> %d; y -> %d; width -> %d; height -> %d\n", display_area_x,display_area_y, display_area_width,display_area_height);


	//printf("Resolution: ");
	
	if (m_simplestation->m_gpu->m_horizontal_resolution == XRes256)
	{
		res_w = 256;
		//printf("256 by ");
	}
	else if (m_simplestation->m_gpu->m_horizontal_resolution == XRes320)
	{
		res_w = 320;
		//printf("320 by ");
	}
	else if (m_simplestation->m_gpu->m_horizontal_resolution == XRes512)
	{
		res_w = 512;
		//printf("512 by ");
	}
	else if (m_simplestation->m_gpu->m_horizontal_resolution == XRes640)
	{
		res_w = 640;
		//printf("640 by ");
	}
	else if (m_simplestation->m_gpu->m_horizontal_resolution == XRes368)
	{
		res_w = 368;
		//printf("368 by ");
	}

	if (!m_simplestation->m_gpu->m_vertical_resolution)
	{
		res_h = 240;
		//printf("240\n");
	}
	else if (m_simplestation->m_gpu->m_vertical_resolution)
	{
		res_h = 480;
		//printf("480\n");
	}

	//printf("hline_start: %d ; hline_end: %d\n", m_simplestation->m_gpu->m_display_horizontal_start, m_simplestation->m_gpu->m_display_horizontal_end);
	//printf("vline_start: %d ; vline_end: %d\n\n", m_simplestation->m_gpu->m_display_line_start, m_simplestation->m_gpu->m_display_line_end);
}

void draw(m_simplestation_state *m_simplestation, bool clear_colour, bool part, bool isline)
{

	/* Off-screen Framebuffer */

	// Bind to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	// Use off-screen shaders
	glUseProgram(program);

	// Bind to off-screen VAO
	glBindVertexArray(m_vao);

	// Bind to off-screen VVBO
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(OpenGL_Vertex) * VERTEX_BUFFER_LEN, m_vertex_buffer, GL_DYNAMIC_DRAW);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_gpu_vram, 0);

	if (clear_colour)
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}

	// Set viewport to accomodate VRAM
	glViewport(0, 0, 1024, 512);
	
	// Off-screen shaders sample-off the off-screen VRAM Texture
	glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);

	// Check if we need to draw lines
	if (isline)
	{
		glDrawArrays(GL_LINES, 0, (GLsizei) (count_vertices));
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, (GLsizei) (count_vertices));
	}

	// Copy the display area from the VRAM off to the on-screen texture
	glBindTexture(GL_TEXTURE_2D, m_window_texture);

	if (!m_simplestation->m_vramview)
	{
		glViewport(0, 0, res_w, res_h);
		
		// Copy the texture to the window
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, res_w, res_h, 0);
	}
	else
	{
		glViewport(0, 0, 1024, 512);
		
		// Copy the texture to the window
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 1024, 512, 0);
	}
	
	// Clear the vertex count
	if (!part) count_vertices = 0;

	// Bind GL_COLOR_ATTACHMENT0 w/the on-screen final texture
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_window_texture, 0);

	/* On-screen Framebuffer */

	// Bind to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Use on-screen shaders
	glUseProgram(fb_program);

	// Bind to on-screen VAO
	glBindVertexArray(output_window_vao);

	// Bind to on-screen VBO
	glBindBuffer(GL_ARRAY_BUFFER, output_window_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(output_window_vertices), &output_window_vertices, GL_STATIC_DRAW);

	// Draw data-off the custom Framebuffer's Texture (GL_COLOR_ATTACHMENT0)
	glBindTexture(GL_TEXTURE_2D, m_window_texture);

	// Set viewport back to window size
	if (!m_simplestation->m_vramview)
	{
		glViewport(0, 0, 640, 480);
	}
	else
	{
		glViewport(0, 0, 1024, 512);
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void m_sync_vram(m_simplestation_state *m_simplestation)
{
	(void) m_simplestation;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 640, 480);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_vram_texel, 0);
	glBindTexture(GL_TEXTURE_2D, m_psx_gpu_vram);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 1024, 512, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glViewport(0, 0, 1024, 512);
}

void display(m_simplestation_state *m_simplestation) {
  draw(m_simplestation, false, false, false);
  SDL_GL_SwapWindow(m_window);
}


int put_triangle(OpenGL_Vertex v1, OpenGL_Vertex v2, OpenGL_Vertex v3, m_simplestation_state *m_simplestation) {
	if (count_vertices + 3 > VERTEX_BUFFER_LEN)
	{
		//printf("Vertex attribute buffers full, forcing_draw\n");
		draw(m_simplestation, false, false, false);
	}

	m_vertex_buffer[count_vertices] = v1;
	count_vertices++;

	m_vertex_buffer[count_vertices] = v2;
	count_vertices++;
	
	m_vertex_buffer[count_vertices] = v3;
	count_vertices++;

	return 0;	
}

int put_line(OpenGL_Vertex v1, OpenGL_Vertex v2, m_simplestation_state *m_simplestation)
{
	draw(m_simplestation, false, false, false);

	m_vertex_buffer[count_vertices] = v1;
	count_vertices++;

	m_vertex_buffer[count_vertices] = v2;
	count_vertices++;

	draw(m_simplestation, false, false, true);
	
	return 0;	
}

int put_quad(OpenGL_Vertex v1, OpenGL_Vertex v2, OpenGL_Vertex v3, OpenGL_Vertex v4, m_simplestation_state *m_simplestation) {
	put_triangle(v1, v2, v3, m_simplestation);
	put_triangle(v2, v3, v4, m_simplestation);
	return 0;
}

int put_rect(Rectangle r0, m_simplestation_state *m_simplestation)
{
	OpenGL_Vertex v1 = {
		r0.position,
		r0.colour, 
		{ m_simplestation->m_gpu->m_page_base_x * 64, m_simplestation->m_gpu->m_page_base_y * 256 },
		r0.texCoord,
		r0.clut,
		tcd_from_val(m_simplestation->m_gpu->m_texture_depth),
		r0.blendMode,
		r0.drawTexture };

	OpenGL_Vertex v2 = {
		{ r0.position.x + r0.widthHeight.width, r0.position.y },
		r0.colour,
		{ m_simplestation->m_gpu->m_page_base_x * 64, m_simplestation->m_gpu->m_page_base_y * 256 },
		{ (GLubyte)(r0.texCoord.x + (GLubyte)(r0.widthHeight.width)), r0.texCoord.y },
		r0.clut,
		tcd_from_val(m_simplestation->m_gpu->m_texture_depth),
		r0.blendMode,
		r0.drawTexture };

	OpenGL_Vertex v3 = {
		{ r0.position.x, r0.position.y + r0.widthHeight.height },
		r0.colour,
		{ m_simplestation->m_gpu->m_page_base_x * 64, m_simplestation->m_gpu->m_page_base_y * 256 },
		{ r0.texCoord.x, (GLubyte)(r0.texCoord.y + (GLubyte)r0.widthHeight.height) },
		r0.clut,
		tcd_from_val(m_simplestation->m_gpu->m_texture_depth),
		r0.blendMode,
		r0.drawTexture };

	OpenGL_Vertex v4 = {
		{ r0.position.x + r0.widthHeight.width, r0.position.y + r0.widthHeight.height },
		r0.colour,
		{ m_simplestation->m_gpu->m_page_base_x * 64, m_simplestation->m_gpu->m_page_base_y * 256 },
		{ (GLubyte)(r0.texCoord.x + (GLubyte)r0.widthHeight.width), (GLubyte)(r0.texCoord.y + (GLubyte)r0.widthHeight.height) },
		r0.clut,
		tcd_from_val(m_simplestation->m_gpu->m_texture_depth),
		r0.blendMode,
		r0.drawTexture };

	put_quad(v1, v2, v3, v4, m_simplestation);
}


/* GP0 Commands */

void m_gpu_clear_cache(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    for (int i = 0; i < (1024 * 512); i++) m_simplestation->m_gpu->write_buffer[i] = 0;

    return;
}

void m_gpu_fill_rect(uint32_t m_value, m_simplestation_state *m_simplestation) {
    (void) m_value;

    uint32_t colour24 = m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xFFFFFF;
	float r = (colour24 & 0xFF) / 255.0f;
	float g = ((colour24 >> 8) & 0xFF) / 255.0f;
	float b = ((colour24 >> 16) & 0xFF) / 255.0f;

    glClearColor(r, g, b, 0.f);

    glEnable(GL_SCISSOR_TEST);

    display_area_x = m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xffff;
    display_area_y = m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16;
    display_area_width = m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xffff;
    display_area_height = m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16;

    glScissor(display_area_x, display_area_y, display_area_width, display_area_height);
    draw(m_simplestation, true, false, false);

    glDisable(GL_SCISSOR_TEST);
}


void m_gpu_draw_monochrome_opaque_line(uint32_t m_value, m_simplestation_state *m_simplestation)
{
	OpenGL_Vertex v1, v2;

	memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));

	uint32_t colour = m_simplestation->m_gpu_command_buffer->m_buffer[0] & 0xffffff;

	v1.position.x = m_simplestation->m_gpu_command_buffer->m_buffer[1] & 0xffff;
	v1.position.y = m_simplestation->m_gpu_command_buffer->m_buffer[1] >> 16;

	v2.position.x = m_simplestation->m_gpu_command_buffer->m_buffer[2] & 0xffff;
	v2.position.y = m_simplestation->m_gpu_command_buffer->m_buffer[2] >> 16;

	v1.colour.r = (((colour) >> 0) & 0xff);
	v1.colour.g = (((colour) >> 8) & 0xff);
	v1.colour.b = (((colour) >> 16) & 0xff);

	v2.colour.r = (((colour) >> 0) & 0xff);
	v2.colour.g = (((colour) >> 8) & 0xff);
	v2.colour.b = (((colour) >> 16) & 0xff);

	put_line(v1, v2, m_simplestation);
}

void m_gpu_draw_monochrome_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    OpenGL_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));
    memset(&v3, 0, sizeof(OpenGL_Vertex));
    memset(&v4, 0, sizeof(OpenGL_Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col;
    v1.drawTexture = 0;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v2.colour = col;
    v2.drawTexture = 0;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v3.colour = col;
    v3.drawTexture = 0;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v4.colour = col;
    v4.drawTexture = 0;

    put_quad(v1, v2, v3, v4, m_simplestation);
}

void m_gpu_draw_texture_blend_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    ClutAttr clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
	TexPage texPage = texpage_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	TextureColourDepth texDepth = tcd_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	GLubyte blend = (GLubyte) BlendTexture;

    OpenGL_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));
    memset(&v3, 0, sizeof(OpenGL_Vertex));
    memset(&v4, 0, sizeof(OpenGL_Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col;
    v1.texPage = texPage;
    v1.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v1.clut = clut;
    v1.texDepth = texDepth;
    v1.blendMode = blend;
    v1.drawTexture = 1;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col;
    v2.texPage = texPage;
    v2.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v2.clut = clut;
    v2.texDepth = texDepth;
    v2.blendMode = blend;
    v2.drawTexture = 1;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col;
    v3.texPage = texPage;
    v3.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[6]);
    v3.clut = clut;
    v3.texDepth = texDepth;
    v3.blendMode = blend;
    v3.drawTexture = 1;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[7]);
    v4.colour = col;
    v4.texPage = texPage;
    v4.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[8]);
    v4.clut = clut;
    v4.texDepth = texDepth;
    v4.blendMode = blend;
    v4.drawTexture = 1;

    put_quad(v1, v2, v3, v4, m_simplestation);
}

void m_gpu_draw_texture_raw_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
        (void) m_value;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    ClutAttr clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
	TexPage texPage = texpage_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	TextureColourDepth texDepth = tcd_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	GLubyte blend = (GLubyte) RawTexture;

    OpenGL_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));
    memset(&v3, 0, sizeof(OpenGL_Vertex));
    memset(&v4, 0, sizeof(OpenGL_Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col;
    v1.texPage = texPage;
    v1.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v1.clut = clut;
    v1.texDepth = texDepth;
    v1.blendMode = blend;
    v1.drawTexture = 1;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col;
    v2.texPage = texPage;
    v2.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v2.clut = clut;
    v2.texDepth = texDepth;
    v2.blendMode = blend;
    v2.drawTexture = 1;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col;
    v3.texPage = texPage;
    v3.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[6]);
    v3.clut = clut;
    v3.texDepth = texDepth;
    v3.blendMode = blend;
    v3.drawTexture = 1;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[7]);
    v4.colour = col;
    v4.texPage = texPage;
    v4.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[8]);
    v4.clut = clut;
    v4.texDepth = texDepth;
    v4.blendMode = blend;
    v4.drawTexture = 1;

    put_quad(v1, v2, v3, v4, m_simplestation);
}

void m_gpu_draw_texture_raw_semitransparent_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
        (void) m_value;

    Colour col = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    ClutAttr clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
	TexPage texPage = texpage_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	TextureColourDepth texDepth = tcd_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

	GLubyte blend = (GLubyte) RawTexture;

    OpenGL_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));
    memset(&v3, 0, sizeof(OpenGL_Vertex));
    memset(&v4, 0, sizeof(OpenGL_Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col;
    v1.texPage = texPage;
    v1.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v1.clut = clut;
    v1.texDepth = texDepth;
    v1.blendMode = blend;
    v1.drawTexture = 1;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col;
    v2.texPage = texPage;
    v2.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v2.clut = clut;
    v2.texDepth = texDepth;
    v2.blendMode = blend;
    v2.drawTexture = 1;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col;
    v3.texPage = texPage;
    v3.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[6]);
    v3.clut = clut;
    v3.texDepth = texDepth;
    v3.blendMode = blend;
    v3.drawTexture = 1;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[7]);
    v4.colour = col;
    v4.texPage = texPage;
    v4.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[8]);
    v4.clut = clut;
    v4.texDepth = texDepth;
    v4.blendMode = blend;
    v4.drawTexture = 1;

    put_quad(v1, v2, v3, v4, m_simplestation);
}

void m_gpu_draw_shaded_opaque_triangle(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    OpenGL_Vertex v1, v2, v3;

    memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));
    memset(&v3, 0, sizeof(OpenGL_Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);

    put_triangle(v1, v2, v3, m_simplestation);
    
    //printf(CYAN "[OPENGL] Draw Shaded Opaque Triangle\n" NORMAL);
}

void m_gpu_draw_shaded_opaque_quad(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;
    (void) m_simplestation;

    OpenGL_Vertex v1, v2, v3, v4;

    memset(&v1, 0, sizeof(OpenGL_Vertex));
    memset(&v2, 0, sizeof(OpenGL_Vertex));
    memset(&v3, 0, sizeof(OpenGL_Vertex));
    memset(&v4, 0, sizeof(OpenGL_Vertex));
    
    v1.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    v1.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
    v1.drawTexture = 0;

    v2.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);
    v2.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    v2.drawTexture = 0;

    v3.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[5]);
    v3.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[4]);
    v3.drawTexture = 0;

    v4.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[7]);
    v4.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[6]);
    v4.drawTexture = 0;

    put_quad(v1, v2, v3, v4, m_simplestation);
    //printf(CYAN "[OPENGL] Draw Shaded Opaque Quadrilateral\n" NORMAL);
}

void m_gpu_draw_texture_raw_variable_size_rect(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    Rectangle r0;

    memset(&r0, 0, sizeof(Rectangle));

    r0.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
    r0.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
    r0.texCoord = texcoord_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    r0.clut = clutattr_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[2]);
    r0.blendMode = (GLubyte) RawTexture;
    r0.drawTexture = 1;
    r0.widthHeight = rwh_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[3]);

	put_rect(r0, m_simplestation);
}

void m_gpu_draw_monochrome_opaque_1x1(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    Rectangle r0;
    RectWidthHeight r0_wh;

    memset(&r0, 0, sizeof(Rectangle));

    r0_wh.width = 1;
    r0_wh.height = 1;

    r0.position = pos_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[1]);
	r0.colour = col_from_gp0(m_simplestation->m_gpu_command_buffer->m_buffer[0]);
	r0.widthHeight = r0_wh;

    put_rect(r0, m_simplestation);
}

void m_gpu_image_draw(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    uint32_t m_resolution = m_simplestation->m_gpu_command_buffer->m_buffer[2];

    uint16_t m_width = m_resolution & 0xFFFF;

    uint16_t m_height = m_resolution >> 16;

    m_width = ((m_width - 1) & 0x3ff) + 1;
    m_height = ((m_height - 1) & 0x1ff) + 1;

    uint32_t m_image_sz = ((m_height * m_width) + 1) & ~1;

    m_simplestation->m_gpu->m_gp0_words_remaining = m_image_sz / 2;

    m_simplestation->m_gpu->m_gp0_write_mode = cpu_to_vram;
}

void m_gpu_image_store(uint32_t m_value, m_simplestation_state *m_simplestation)
{
    (void) m_value;

    draw(m_simplestation, false, true, false);
    

    const uint32_t coords = m_simplestation->m_gpu_command_buffer->m_buffer[1];
    const uint32_t res = m_simplestation->m_gpu_command_buffer->m_buffer[2];
    // TODO: Sanitize this
    const auto x = coords & 0x3ff;
    const auto y = (coords >> 16) & 0x1ff;

    uint32_t width = res & 0xffff;
    uint32_t height = res >> 16;

    width = ((width - 1) & 0x3ff) + 1;
    height = ((height - 1) & 0x1ff) + 1;

    // The size of the texture in 16-bit pixels. If the number is odd, force align it up
    const uint32_t size = ((width * height) + 1) & ~1;

    m_simplestation->m_gpu->m_gp0_read_mode = vram_to_cpu;
    m_simplestation->m_gpu->m_vram_image_size = size / 2;

    m_simplestation->m_gpu->m_vram_image_index = 0;

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_gpu_vram, 0);

    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, m_simplestation->m_gpu->read_buffer);


    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_psx_vram_texel, 0);

    glBindTexture(GL_TEXTURE_2D, m_psx_vram_texel);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, m_simplestation->m_gpu->read_buffer);
}
