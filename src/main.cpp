#include <map>

int main() {
  using Price = int;

  // Create the Sell order book
  std::map<Price, Inventory, std::less<Price>> sell_book;

  // Create the Buy order book
  std::map<Price, Inventory, std::greater<Price>> buy_book;
}
