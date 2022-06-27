#include <simplestation.h>
#include <cpu/cpu.h>
#include <cpu/bios.h>
#include <cpu/interrupts.h>
#include <memory/memory.h>
#include <ui/termcolour.h>
#include <tests/tests.h>
#include <limits.h>

int main(int argc, char **argv)
{
	m_simplestation_state m_simplestation;

	// Define a char pointer that will hold the file name
	const char *m_biosname = "scph1001.bin";

    printf(GREEN "SimpleStation - TEST MODE\n" NORMAL);

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
					
                    m_exec_tests(&m_simplestation);
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

void m_exec_tests(m_simplestation_state *m_simplestation)
{
	printf(YELLOW "[TEST] Starting the tests...\n" NORMAL);
    m_signed_integer_overflow_test(m_simplestation);
	m_memory_test(m_simplestation);
}

void m_signed_integer_overflow_test(m_simplestation_state *m_simplestation)
{
	size_t m_passing_tests = 0;
	int x = 45, y = 2147483647;

	printf(YELLOW "[TEST] Starting signed_integer_overflow_test...\n" NORMAL);

	// Overflow
	if (m_cpu_check_signed_addition(x, y) == true)
	{
		m_passing_tests++;
	}
	
    x = 45;
	y = 10;

	// No overflow
	if (m_cpu_check_signed_addition(x, y) == false)
	{
		m_passing_tests++;
	}
	
    x = -45;
	y = -2147483647;

	// Overflow
	if (m_cpu_check_signed_addition(x, y) == true)
	{
		m_passing_tests++;
	}

    x = 45;
	y = 2147483647;

	// Overflow
	if (m_cpu_check_signed_addition(x, y) == true)
	{
		m_passing_tests++;
	}

    x = INT_MAX;
	y = 1;

	// Overflow
	if (m_cpu_check_signed_addition(x, y) == true)
	{
		m_passing_tests++;
	}

    x = INT_MIN;
	y = -1;

	// Overflow
	if (m_cpu_check_signed_addition(x, y) == true)
	{
		m_passing_tests++;
	}

    if (m_passing_tests == 6)
	{
		printf(GREEN "[ OK ] signed_integer_overflow_test\n");
	}
	else
	{
		printf(RED "[FAIL] signed_integer_overflow_test (%lu tests)\n" NORMAL, (6 - m_passing_tests));
	}
}

void m_memory_test(m_simplestation_state *m_simplestation)
{
	size_t i, m_passing_tests = 0;

	bool m_good = true;

	for (i = 0; i < sizeof(PSX_MEM); i++)
	{
		if (m_simplestation->m_memory->m_mem_ram[i] != 0)
		{
			m_good = false;
		}
	}

	if (m_good) m_passing_tests++;

	m_good = true;

	for (i = 0; i < (1 * KiB); i++)
	{
		if (m_simplestation->m_memory->m_mem_scratchpad[i] != 0)
		{
			m_good = false;
		}
	}

	if (m_good) m_passing_tests++;
	
	m_good = true;

	for (i = 0; i < (0x20); i++)
	{
		if (m_simplestation->m_memory->m_mem_memctl1[i] != 0)
		{
			m_good = false;
		}
	}

	if (m_good) m_passing_tests++;
	
	m_good = true;

    if (m_passing_tests == 3)
	{
		printf(GREEN "[ OK ] memory_write_test\n");
	}
	else
	{
		printf(RED "[FAIL] memory_write_test (%lu tests)\n" NORMAL, (3 - m_passing_tests));
	}

	m_memory_write_test(m_simplestation);
}

void m_memory_write_test(m_simplestation_state *m_simplestation)
{
	printf("Initial value: 0x%08X\n", m_memory_read(0x1FFFFC, dword, m_simplestation));

	m_memory_write(0x1FFFFC, 0xFFFFFFFF , dword, m_simplestation);

	printf("Value: 0x%08X\n", m_memory_read(0x1FFFFC, dword, m_simplestation));

}
