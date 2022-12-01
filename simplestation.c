#include <simplestation.h>
#include <cpu/cpu.h>
#include <cpu/bios.h>
#include <cpu/interrupts.h>
#include <gpu/gpu.h>
#include <memory/memory.h>
#include <ui/termcolour.h>

int main(int argc, char **argv)
{
	m_simplestation_state m_simplestation;

	// Define a char pointer that will hold the file name
	const char *m_biosname = NULL;

	printf("SimpleStation - Multiplatform PSX Emulator\n");

#if defined (__unix__) || defined (__APPLE__)
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
						m_interrupts_init(&m_simplestation);

						if (m_gpu_init(&m_simplestation) == 0)
						{
							while (true)
							{
								// Fetch, decode, execute
								m_cpu_fde(&m_simplestation);
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
	if (m_simplestation->m_dma_state)
	{
		m_dma_exit(m_simplestation);
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
		// Do nothing for now
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