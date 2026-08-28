#include <cstdint>

/**
 * Basic creation of an order object.
 */
struct Order {
  int price;
  int size;
  int order_id;
  uint64_t timestamp;
  bool buy;

  Order() = delete;
  Order(int p, int s, int o, uint64_t t, bool b)
      : price(p), size(s), order_id(o), timestamp(t), buy(b) {}
};
