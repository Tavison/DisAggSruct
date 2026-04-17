/**
 * @file Tests.h
 * @brief Declarations for the DisAggStruct test suite.
 *
 * Each function exercises a distinct aspect of the framework and prints
 * its results to @c stdout.  Call @c RunAllTests() from @c main() to
 * execute the full suite.
 */

#pragma once

/**
 * @brief Runs the complete DisAggStruct test suite.
 *
 * Calls each individual test function in order and prints a summary line
 * on completion.  Returns @c 0 if all tests pass, non-zero otherwise.
 *
 * @return Exit code suitable for returning from @c main().
 */
int RunAllTests();
