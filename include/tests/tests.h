#ifndef TESTS_H
#define TESTS_H

#include <simplestation.h>

/* Function definitions */
void m_exec_tests(m_simplestation_state *m_simplestation);

/* Tests function definitions */
void m_signed_integer_overflow_test(m_simplestation_state *m_simplestation);
void m_memory_test(m_simplestation_state *m_simplestation);
void m_memory_write_test(m_simplestation_state *m_simplestation);

#endif /* TESTS_H */
