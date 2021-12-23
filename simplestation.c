#include <simplestation.h>

int main(int argc, char **argv)
{
	printf("SimpleStation - Multiplatform PSX Emulator\n");

#ifdef __unix__
	// Check if arguments were passed
	if (argc < 2)
	{
		printf("Usage: ./simplestation [arguments]\n");
		printf("Arguments:\n");
		printf("-bios [...] - Specifies a PSX BIOS file\n");
		exit(EXIT_SUCCESS);
	}

	// Define a char pointer that will hold the file name
	const char *m_biosname = NULL;

	for (int m_args = 1; m_args < argc; m_args++)
	{
		if (!strcmp(argv[m_args], "-bios"))
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
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			printf("Unknown argument: %s\n", argv[m_args]);
			exit(EXIT_FAILURE);
		}
	}

	// Check if BIOS file is specified
	if (m_biosname)
	{
		// Load the BIOS file
		m_bios_load(m_biosname);
	}
	else
	{
		// BIOS file is required for emulation
		printf("Must specify a BIOS file in order for SimpleStation to work!\n");
	}
#endif

	// Initialize the CPU Subsystem
	m_cpu_init();

	// Uninitialize the BIOS Subsystem
	m_bios_close();

	// Uninitialize the CPU Subsystem
	m_cpu_exit();
	return 0;
}