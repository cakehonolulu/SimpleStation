#include <simplestation.h>
#include <cpu/cpu.h>
#include <cpu/bios.h>
#include <cpu/interrupts.h>
#include <gpu/gpu.h>
#include <gpu/command_buffer.h>
#include <cdrom/cdrom.h>
#include <memory/memory.h>
#include <ui/termcolour.h>
#include <SDL2/SDL.h>
#include <gpu/renderer.h>


#include <GLFW/glfw3.h>

#ifdef DUMP_VRAM
#include <GL/glew.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif

int main(int argc, char **argv)
{
	m_simplestation_state m_simplestation;
	SDL_Event m_event;

	// Define a char pointer that will hold the file name
	const char *m_biosname = NULL;

	printf("SimpleStation - Multiplatform PSX Emulator\n");

#if defined (__unix__) || defined (__APPLE__) || defined (__MINGW32__) || defined (__MINGW64__) 
	// Check if arguments were passed
	if (argc < 2)
	{	
		printf("Usage: ./simplestation [arguments]\n");
		printf("Arguments:\n");
		printf("-bios [...] - Specifies a PSX BIOS file\n");
	}
	else
	{
		m_simplestation.m_breakpoint = 0;
		m_simplestation.m_debugger = false;
		m_simplestation.m_sideload = false;

		for (int m_args = 1; m_args < argc; m_args++)
		{
			if (!strcmp(argv[m_args], "-bios"))
			{
				if (argv[m_args + 1] != NULL)
				{
					if ((strstr(argv[m_args + 1], ".bin") != NULL))
					{
						m_biosname = argv[m_args + 1];
						printf("BIOS File: %s\n", m_biosname);
						m_args++;
					}
					else
					{
						printf("BIOS File Extension not Supported!\n");
						printf("Valid Format: .bin\n");
						m_args++;
					}
				}
				else
				{
					printf("You must specify a filename to load the BIOS from!\n");
				}
			}
			else if (!strcmp(argv[m_args], "-prfrom"))
			{
				if (argv[m_args + 1] != NULL)
				{
					m_simplestation.m_wp = strtol(argv[m_args + 1], NULL, 16);
					printf("Printing registers from PC: 0x%08X\n", m_simplestation.m_breakpoint);
					m_args++;
				}
				else
				{
					printf("You must specify an address to start printing registers from!\n");
				}
			}
			else if (!strcmp(argv[m_args], "-break"))
			{
				if (argv[m_args + 1] != NULL)
				{
					m_simplestation.m_breakpoint = strtol(argv[m_args + 1], NULL, 16);
					printf("Breakpoint set: 0x%08X\n", m_simplestation.m_breakpoint);
					m_args++;
				}
				else
				{
					printf("You must specify breakpoint!\n");
				}
			}
			else if (!strcmp(argv[m_args], "-exe"))
			{
				if (argv[m_args + 1] != NULL)
				{
					m_simplestation.exename = malloc(strlen(argv[m_args + 1]) * sizeof(uint8_t) + 1);
					strcpy(m_simplestation.exename, argv[m_args + 1]);
					printf("PSX-EXE Name: %s\n", m_simplestation.exename);
					m_args++;
					m_simplestation.m_sideload = true;
				}
				else
				{
					printf("You must specify a PSX-EXE filename!\n");
				}
			}
			else if (!strcmp(argv[m_args], "-debugger"))
			{
				m_simplestation.m_debugger = true;
				printf("Enabled debugger...!\n");
				m_args++;
			}
			else
			{
				printf("Unknown argument: %s\n", argv[m_args]);
			}
		}
#endif

		m_simplestation.m_dma_state = OFF;

		m_simplestation.m_memory_state = OFF;

		m_simplestation.m_interrupts_state = OFF;

		m_simplestation.m_cpu_state = OFF;

		m_simplestation.m_gpu_state = OFF;

		m_simplestation.m_gpu_command_buffer_state = OFF;

		// Check if BIOS file is specified
		if (m_biosname)
		{
			// Initialize the Memory Subsystem
			if (m_memory_init(&m_simplestation) == 0)
			{
				// Load the BIOS file
				if (m_bios_load(&m_simplestation, m_biosname) == 0)
				{
					// Initialize the CPU Subsystem
					if (m_cpu_init(&m_simplestation) == 0)
					{
						// Initialize the Interrupts Subsystem
						if (m_interrupts_init(&m_simplestation) == 0)
						{
							if (m_gpu_init(&m_simplestation) == 0)
							{
								if (m_cdrom_init(&m_simplestation) == 0)
								{
									glfwInit();
									
									double lastTime = glfwGetTime();
									int nbFrames = 0;

									while (true)
									{
										for (int i = 0; i < 400000; i++)
										{
											// Fetch, decode, execute
											m_cpu_fde(&m_simplestation);
										}

										display(&m_simplestation);

										double currentTime = glfwGetTime();
										double delta = currentTime - lastTime;
										nbFrames++;

										if ( delta >= 1.0 )
										{
											double frametime = 1000.0/(double)nbFrames;

											double fps = (double) nbFrames / delta;

											char buffer[1024];
											snprintf(buffer, sizeof(buffer), "SimpleStation (SDL2) | MS/F: %.4F | FPS: %.2F", frametime, fps);

											m_window_changetitle(buffer);

											nbFrames = 0;
											lastTime = currentTime;
										}

										while( SDL_PollEvent( &m_event ) )
										{
											if( m_event.type == SDL_QUIT )
											{
												// Quit the program
												m_simplestation_exit(&m_simplestation, 1);
											}
										}
									}
								}
								else
								{
									// If CDROM couldn't be initialized, exit out
									m_simplestation_exit(&m_simplestation, 1);
								}
							}
							else
							{
								// If GPU couldn't be initialized, exit out
								m_simplestation_exit(&m_simplestation, 1);
							}
						}
						else
						{
							// If Interrupt systen couldn't be initialized, exit out
							m_simplestation_exit(&m_simplestation, 1);
						}			
					}
					else
					{
						// If CPU couldn't be initialized, exit out
						m_simplestation_exit(&m_simplestation, 1);
					}
				}
				else
				{
					// If BIOS couldn't be loaded, exit out
					m_simplestation_exit(&m_simplestation, 1);
				}
			}
			else
			{
				// If memory couldn't be initialized
				m_simplestation_exit(&m_simplestation, 1);
			}
		}
		else
		{
			printf("You must specify a BIOS filename!\n");
		}
	}
	
	return m_simplestation_exit(&m_simplestation, 0);
}

uint8_t m_simplestation_exit(m_simplestation_state *m_simplestation, uint8_t m_is_fatal)
{
#ifdef DUMP_VRAM
	FILE *f = fopen("vram.bin", "wb");
	fwrite(m_simplestation->m_gpu_image_buffer->buffer, sizeof(char), sizeof(m_simplestation->m_gpu_image_buffer->buffer), f);
	fclose(f);
	
	extern GLint texture;
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint width, height, level = 0;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, level, GL_TEXTURE_HEIGHT, &height);
	glGetTexImage(GL_TEXTURE_2D, level, GL_RGB, GL_UNSIGNED_BYTE, m_simplestation->m_vram_data);

	stbi_write_bmp( "vram.bmp", 1024, 512, 3, m_simplestation->m_vram_data );
#endif

	if (m_simplestation->m_cdrom_state)
	{
		m_cdrom_exit(m_simplestation);
	}

	if (m_simplestation->m_dma_state)
	{
		m_dma_exit(m_simplestation);
	}
		
	if (m_simplestation->m_gpu_command_buffer_state)
	{
		m_gpu_command_buffer_exit(m_simplestation);
	}

	if (m_simplestation->m_gpu_state)
	{
		m_gpu_exit(m_simplestation);
	}

	if (m_simplestation->m_memory_state)
	{
		m_memory_exit(m_simplestation);
	}

	if (m_simplestation->m_interrupts_state)
	{
		m_interrupts_exit(m_simplestation);
	}

	if (m_simplestation->m_cpu_state)
	{
		m_printregs(m_simplestation);
		m_cpu_exit(m_simplestation);
	}

	if (m_is_fatal == 1)
	{
		printf(RED "Fatal error found, exiting...\n" NORMAL);
		exit(EXIT_FAILURE);
	}

	return m_is_fatal;
}