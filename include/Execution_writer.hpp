#pragma once

#include <string>
#include <vector>

#include "Execution.hpp"

/*
 * Writes the executions to a txt file
 * @param executions: ref to the execution's vector
 * @param filepath: the txt file that will contain the report
 */
void write_exec_to_file(const std::vector<Execution>& executions,
                        const std::string& filepath);
