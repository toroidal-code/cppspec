#include <string>
#include "runnable.hpp"

class ItBase : public Runnable {
 protected:
  std::string descr = "";
  bool does_need_descr = false;

 public:
  ItBase(){};
  ItBase(std::string descr) : descr(descr){};

  bool run() { return false; };

  void set_needs_descr(bool nd) { does_need_descr = nd; }
  bool needs_descr() { return does_need_descr; }
};
