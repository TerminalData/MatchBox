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
    out << "maker id: " << exec.maker_order_id
        << ", taker id: " << exec.taker_order_id << ", size: " << exec.size
        << ", price: " << exec.price << "\n";
  }
}
