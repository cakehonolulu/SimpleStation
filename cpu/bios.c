#include <cpu/bios.h>
#include <memory/memory.h>

// Load the BIOS into a Byte Buffer
uint8_t m_bios_load(m_simplestation_state *m_simplestation, const char *m_bios_name)
{
	// Set up a BIOS File Descriptor
	FILE *m_bios;

	// Status for the function
	uint8_t m_status = 1;

	// BIOS Filesizem_simplestation
	int64_t m_bios_size;

	// Open the BIOS in Binary Mode and Read-Only
	m_bios = fopen(m_bios_name, "rb");

	// Check if BIOS fopen() worked
	if (m_bios)
	{
		// Get file size in bytes, also checks if fseek() succeeds
		if (fseek(m_bios, 0, SEEK_END) == 0)
		{
			m_bios_size = ftell(m_bios);

			// Check if ftell() returned a valid size number
			if (m_bios_size != -1)
			{
				// Check if fseek() call succeeded
				if (fseek(m_bios, 0, SEEK_SET) == 0)
				{
					// BIOS Files are exactly 512KiB (524288 bytes), check for it
					/*
						TODO:	BIOS file *can* be corrupted, inform the user and ask for
								execution confirmation; but bugs reported under those
								conditions will probably be ignored.
					*/
					if ((m_bios_size / KiB) == 512)
					{
						// Allocate a buffer for the BIOS file
						m_simplestation->m_memory->m_mem_bios = (int8_t*) malloc(sizeof(int8_t) * m_bios_size);

						// Error out on memory exhaustion
						if (m_simplestation->m_memory->m_mem_bios != NULL)
						{
							// Load the file into host memory
							if (fread(m_simplestation->m_memory->m_mem_bios, sizeof(int8_t), m_bios_size, m_bios) == (size_t) m_bios_size)
							{
								// Check if file was loaded correctly into memory
								if (!ferror(m_bios))
								{
									printf("BIOS Size: %zu bytes\n", (size_t) m_bios_size);

									m_status = 0;
								}
								else
								{
									printf("Error loading file into memory!\nExiting...\n");
								}
							}
							else
							{
								printf("Unknown error loading the file...!\nExiting...\n");
							}
						}
						else
						{
							printf("Simplestation: Couldn't allocate PSX BIOS Memory...\n Exiting...\n");
						}
					}
					else
					{
						printf("BIOS File Size not correct, check your BIOS file...\nExiting...\n");
					}
				}
				else
				{
					printf("Error resetting file position...\nExiting...\n");
				}
			}
			else
			{
				printf("Unable to get BIOS File size...\nExiting...\n");
			}
		}
		else
		{
			printf("Error seeking file bytes...\nExiting...\n");
			
		}

		// Close the file handle
		if (!fclose(m_bios))
		{
			printf("BIOS File Handle closed succesfully!\n");
		}
		else
		{
			printf("BIOS File Handle couldn't be closed, fatal error!\n");
			m_simplestation_exit(m_simplestation, 1);
		}
	}
	else
	{
		printf("BIOS File couldn't be opened!\nExiting...\n");
	}

	return m_status;
}

// Free the BIOS Buffer
void m_bios_exit(m_simplestation_state *m_simplestation)
{
	if (m_simplestation->m_memory->m_mem_bios != NULL)
	{
		free(m_simplestation->m_memory->m_mem_bios);
	}
}