#include <simplestation.h>

m_simplestation_state m_simplestation;

int main(int argc, char **argv)
{
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

		// Check if BIOS file is specified
		if (m_biosname)
		{
			// Load the BIOS file
			m_bios_load(m_biosname);

			// Initialize the CPU Subsystem
			m_cpu_init();

			// Initialize the Memory Subsystem
			m_memory_init();

			// Initialize the Interrupts Subsystem
			m_interrupts_init();

			while (true)
			{
				// Fetch, decode, execute
				m_cpu_fde();
			}

			return m_simplestation_exit();
		}
	#endif
	}
}

uint8_t m_simplestation_exit()
{
	if (m_simplestation.m_memory_state)
	{
		m_memory_exit();
	}

	if (m_simplestation.m_interrupts_state)
	{
		// Do nothing for now
	}

	if (m_simplestation.m_cpu_state)
	{
		m_cpu_exit();
	}

	exit(EXIT_FAILURE);
	return 1;
}