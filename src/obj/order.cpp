/**
 * Basic creation of an order object.
 */

struct Order {
  int price;
  int size;
  int order_id;
  bool buy;

  Order() = delete;
  Order(int p, int s, int o, bool b) : price(p), size(s), order_id(o), buy(b) {}
};
