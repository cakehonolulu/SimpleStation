#include <simplestation.h>
#include <cpu/cpu.h>
#include <cpu/bios.h>
#include <cpu/interrupts.h>
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
			else
			{
				printf("Unknown argument: %s\n", argv[m_args]);
			}
		}
#endif

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

						while (true)
						{
							// Fetch, decode, execute
							m_cpu_fde(&m_simplestation);
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
	if (m_simplestation->m_interrupts_state)
	{
		m_memory_exit(m_simplestation);
	}

	if (m_simplestation->m_interrupts_state)
	{
		// Do nothing for now
	}

	if (m_simplestation->m_cpu_state)
	{
		m_cpu_exit(m_simplestation);
	}

	if (m_is_fatal == 1)
	{
		printf(RED "Fatal error found, exiting...\n" NORMAL);
		exit(EXIT_FAILURE);
	}

	return m_is_fatal;
}