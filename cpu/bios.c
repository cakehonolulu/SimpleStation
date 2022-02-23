#include <cpu/bios.h>

// Load the BIOS into a Byte Buffer
void m_bios_load(const char *m_bios_name)
{
	// Set up a BIOS File Descriptor
	FILE *m_bios;

	// Open the BIOS in Binary Mode and Read-Only
	m_bios = fopen(m_bios_name, "rb");

	// Check if BIOS fopen() worked
	if (!m_bios)
	{
		printf("BIOS File couldn't be opened!\nExiting...\n");
		exit(EXIT_FAILURE);
	}

	// Get file size in bytes, also checks if fseek() succeeds
	if (fseek(m_bios, 0, SEEK_END) != 0)
	{
		printf("Error seeking file bytes...\nExiting...\n");
		fclose(m_bios);
		exit(EXIT_FAILURE);
	}

	int32_t m_bios_size = ftell(m_bios);

	// Check if ftell() returned a size number
	if (m_bios_size == -1)
	{
		printf("Unable to get BIOS File size...\nExiting...\n");
		fclose(m_bios);
		exit(EXIT_FAILURE);
	}

	// Check if fseek() call succeeded
	if (fseek(m_bios, 0, SEEK_SET) != 0)
	{
		printf("Error resetting file position...\nExiting...\n");
		fclose(m_bios);
		exit(EXIT_FAILURE);
	}

	// BIOS Files are exactly 512KiB (524288 bytes)
	if ((m_bios_size / KiB) != 512)
	{
		printf("BIOS File Size not correct, check your BIOS file...\n");
		fclose(m_bios);
		exit(EXIT_FAILURE);
	}

	// Allocate a buffer for the program
	m_mem_bios = (int8_t*) malloc(sizeof(int8_t) * m_bios_size);

	// Error out on memory exhaustion
	if (m_mem_bios == NULL)
	{
		printf("Simplestation: Couldn't allocate PSX BIOS Memory, exiting...");
		fclose(m_bios);
		exit(EXIT_FAILURE);
	}

	// Load the file into host memory
	fread(m_mem_bios, sizeof(int8_t), m_bios_size, m_bios); 

	printf("BIOS Size: %zu bytes\n", (size_t) m_bios_size);

	// Close the file handle
	fclose(m_bios);
}

// Free the BIOS Buffer
void m_bios_exit()
{
	free(m_mem_bios);
}