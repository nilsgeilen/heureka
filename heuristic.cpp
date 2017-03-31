#include "heuristic.h"

#include <iostream>
#include <sstream>
#include <algorithm>

Heuristic * parseHeuristic (AAF&aaf, AttackRelation&ar, std::stringstream&source) {
  std::string name;
  source >> name;
  if (name == "+" || name == "-" || name == "*" || name == "/" || name == "^") {
    auto * a = parseHeuristic(aaf, ar, source),
      * b = parseHeuristic(aaf, ar, source);
    if (b -> is_dynamic() || a -> is_const()) {
      auto * temp = a;
      a = b;
      b = temp;
    }
    if (b -> is_dynamic())
      std::cerr << "Fail: cannot combine two dynamic heuristics"<<std::endl;
    a -> zip_in_place (b, name[0]);
    delete b;
    return a;
  } else if (name == "scc") {
    return new OrderedSCCHeuristic {ar};
  } else if (name == "deg") {
    float indeg_weight, outdeg_weight;
    if (source >> indeg_weight >> outdeg_weight)
      return new PathHeuristic{ar,1,indeg_weight,1,outdeg_weight};
    else std::cerr << "Fail: Heuristic expects different params: " << name <<std::endl;
  } else if (name == "indeg") {
    return new PathHeuristic{ar,1,1,0,0};
  } else if (name == "outdeg") {
    return new PathHeuristic{ar,0,0,1,1};
  } else if (name == "dynindeg") {
    int window_size;
    rational_t weight;
    if (source >> window_size >> weight)
      return new DynamicDegreeHeuristic {ar, window_size, weight};
    else std::cerr << "Fail: Heuristic expects different params: " << name <<std::endl;
  } else if (name == "path") {
   int in_depth, out_depth;
   rational_t in_alpha, out_alpha;
   if (source >> in_depth >> in_alpha >> out_depth >> out_alpha)
     return new PathHeuristic{ar,in_depth , in_alpha , out_depth , out_alpha};
    else std::cerr << "Fail: Heuristic expects different params: " << name <<std::endl;
  } else if (name == "eig") {
    return new EigenHeuristic {aaf, 10};
  } else if (name == "exp") {
    return new DenseExponentialHeuristic {aaf};
  } else if (name == "exps") {
    return  getExponentialHeuristic (aaf);
  } else {
    std::stringstream str (name);
    rational_t const_val;
    if (str >> const_val) {
      return new ConstHeuristic(const_val);
    }
  }
  return nullptr;
}



namespace {
    bool tuple_comp_desc (const std::pair<int,rational_t>& a, const std::pair<int,rational_t>& b) {
       return a.second > b.second;
    }

    bool tuple_comp_asc (const std::pair<int,rational_t>& a, const std::pair<int,rational_t>& b) {
       return a.second < b.second;
    }
}


void Heuristic::zip_in_place(const Heuristic* heuristic, char op) {
  if (order.size() == heuristic->order.size()) {
    switch (op) {
    case '+':
      for (int i = 0; i < order.size(); i++) {
        order[i].second += heuristic->order[i].second;
      }
      return;
    case '-':
      for (int i = 0; i < order.size(); i++) {
        order[i].second -= heuristic->order[i].second;
      }
      return;
    case '*':
      for (int i = 0; i < order.size(); i++) {
        order[i].second *= heuristic->order[i].second;
      }
      return;
    case '/':
      for (int i = 0; i < order.size(); i++) {
        if (heuristic->order[i].second != 0)
          order[i].second /= heuristic->order[i].second;
        else std::cerr << "Fail: division by zero";
      }
      return;
    case '^':
      for (int i = 0; i < order.size(); i++) {
        order[i].second = std::pow (order[i].second, heuristic->order[i].second);
      }
      return;
    }
  } else if (heuristic-> order.size() == 1) {
    const rational_t c = heuristic -> order [0] .second;
    switch (op) {
    case '+':
      for (int i = 0; i < order.size(); i++) {
        order[i].second += c;
      }
      return;
    case '-':
      for (int i = 0; i < order.size(); i++) {
        order[i].second -= c;
      }
      return;
    case '*':
      for (int i = 0; i < order.size(); i++) {
        order[i].second *= c;
      }
      return;
    case '/':
      if (c == 0)
        std::cerr << "Fail: division by zero";
      else for (int i = 0; i < order.size(); i++) {
        order[i].second /= c;
      }
      return;
    case '^':
      for (int i = 0; i < order.size(); i++) {
        order[i].second = std::pow (order[i].second, c);
      }
      return;
    }
  }
}


void Heuristic::sort(int from) {
  std::sort(order.begin() + from, order.end(), tuple_comp_desc);
}

rational_t Heuristic::get_min_val() const {
  rational_t min = 0.0;
  for (auto p : order)
    if (p.second < min)
      min = p.second;
    return min;
  //return order[0].second;
}

rational_t Heuristic::get_max_val() const {
  rational_t max = 0.0;
  for (auto p : order)
    if (p.second > max)
      max = p.second;
    return max;
  //return order[order.size()-1].second;
}

std::ostream& operator <<(std::ostream& stream, const Heuristic& heuristic) {
  for (int i = 0; i < heuristic.order.size(); i++) {
    stream << heuristic.order[i].first << "\t";
  }
  stream << std::endl;
  for (int i = 0; i < heuristic.order.size(); i++) {
    stream << heuristic.order[i].second << "\t";
  }
  return stream;
}
