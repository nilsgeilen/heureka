#include "AAF.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>

AttackRelation::AttackRelation(AAF &aaf)
  : arg_cnt (aaf.args.size()), atters(aaf.args.size()), atteds(aaf.args.size()) {
  for(Attack att:aaf.atts) {
    atters[att.passive.id].push_back(att.active.id);
    atteds[att.active.id].push_back(att.passive.id);
    if (att.active.id == att.passive.id)
      self_atters.push_back(att.active.id);
  }
}

void AAF::print(std::vector<int> ext, std::ostream &str) {
  str << '[';
  bool first = true;
  for (int i : ext) {
    if (first)
      first = false;
    else
      str << ',';
    str << args[i];
  }
  str << ']';
}

void AAF::todot(std::ostream &str) {
  str << "digraph G {\n";
  for (auto arg : args)
    str << arg.label<< ";\n";
  for (auto att : atts)
    str << att.active.label << " -> " << att.passive.label << ";\n";
  str << "}";
}

std::ostream& operator <<(std::ostream& stream, const Argument& arg) {
  stream << arg.label;
}

bool operator==(const Argument &x, const Argument &y)  {
  return x.id == y.id;
}

bool operator!=(const Argument &x, const Argument &y) {
  return x.id != y.id;
}

std::ostream& operator <<(std::ostream& stream, const Attack& att) {
  stream << att.active << " -> " << att.passive;
}

bool operator==(const Attack &x, const Attack &y)  {
  return x.active.id == y.active.id && x.passive.id == y.passive.id;
}

bool operator!=(const Attack &x, const Attack &y) {
  return x.active.id != y.active.id || x.passive.id != y.passive.id;
}

std::ostream& operator <<(std::ostream& stream, const AAF& aaf) {
  for (auto &arg : aaf.args)
    stream << arg << ", ";
  stream <<"\n";
  for (auto &att : aaf.atts)
    stream << att << ", ";
  stream <<"\n";
}
