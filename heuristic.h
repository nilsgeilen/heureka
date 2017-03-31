#include "AAF.h"

typedef double rational_t;
typedef int degree_counter_t;

class Heuristic {
  const bool _dynamic, _const;
protected:
  std::vector<std::pair<int,rational_t>> order;
public:
  Heuristic(bool dyn = false, bool con = false) : _dynamic(dyn), _const(con) {}
  virtual int get(int from, const degree_counter_t * const adjusted_indegree) {return order[from].first;}
  void zip_in_place(const Heuristic* heuristic, char op);
  void sort(int from = 0) ;
  rational_t get_min_val() const;
  rational_t get_max_val() const;
  inline bool is_dynamic() {return _dynamic;}
  inline bool is_const() {return _const;}
  friend std::ostream& operator <<(std::ostream& stream, const Heuristic& heuristic) ;
};

/**
 * Parses a heuristic out of a formula
 * @param  aaf    an AAF
 * @param  ar     its attack relation
 * @param  source contians the formula
 * @return        the corresponding heuristic
 */
Heuristic * parseHeuristic (AAF&aaf, AttackRelation&ar, std::stringstream&source) ;


class ConstHeuristic: public Heuristic {
public:
  ConstHeuristic(rational_t val) : Heuristic(false, true) {order.push_back(std::pair<arg_t,rational_t>(0,val));}
  auto get(int from, const degree_counter_t * const adjusted_indegree) -> int override {return from;}
};

class PathHeuristic : public Heuristic {
public:
  PathHeuristic (AttackRelation&ar, int in_depth, rational_t in_alpha, int out_depth, rational_t out_alpha);
};


class ExtendedDegreeRatioHeuristic : public Heuristic {
public:
  ExtendedDegreeRatioHeuristic (AttackRelation &ar);
};

class WeightedDegreeHeuristic : public Heuristic {
public:
  WeightedDegreeHeuristic (AttackRelation &ar);
};

class DynamicDegreeHeuristic : public Heuristic {
  AttackRelation &ar;
  const int window_size;
  const rational_t weight;
public:
  DynamicDegreeHeuristic (AttackRelation &ar, int window_size, rational_t weight);
  int get(int from, const degree_counter_t * const adjusted_indegree) override ;
};

class EigenHeuristic : public Heuristic {
public:
  EigenHeuristic (AAF&aaf, int steps);
};

class BetweennessHeuristic : public Heuristic {
public:
  BetweennessHeuristic(AttackRelation&aaf);
};

Heuristic * getExponentialHeuristic (AAF &aaf);

class DenseExponentialHeuristic : public Heuristic {
public:
  DenseExponentialHeuristic (AAF &aaf);
};

class SCCHeuristic : public Heuristic {
public:
  SCCHeuristic(const AttackRelation&ar);
};

class OrderedSCCHeuristic : public Heuristic {
public:
  OrderedSCCHeuristic(const AttackRelation&ar);
};
