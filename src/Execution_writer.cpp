#include "Execution_writer.hpp"

#include <fstream>
#include <iostream>

void write_exec_to_file(const std::vector<Execution>& executions,
                        const std::string& filepath) {
  std::ofstream out(filepath);

  if (!out) {
    std::cerr << "Error, could not open " << filepath
              << " for writing reports.\n"
              << std::endl;
    return;
  }

  for (const auto& exec : executions) {
    double price = static_cast<double>(exec.price) / 1000000000.0;
    out << " Taker id: " << exec.taker_order_id << " matched: " << exec.size
        << " stocks at: " << price << " each.\n";
  }
}
