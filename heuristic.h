#include "AAF.h"

#include <Eigen/SparseCore>
#include <Eigen/Core>

namespace {
  typedef double rational_t;
  typedef int degree_counter_t;
  //doublette from collector.h
  typedef unsigned char label_t;
}

class HeuristicAlgorithm {
protected:
  int *pos_range, *neg_range, *agressor_cnt;
  std::vector<label_t> labels;
public:
  inline const int *get_aggressor_cnt() const {return agressor_cnt;}
  inline const int *get_neg_range() const {return neg_range;}
  inline const int *get_pos_range() const {return pos_range;}
  inline const std::vector<label_t> &get_labels() const {return labels;}
};

class Heuristic {
  const bool _dynamic, _const;
protected:
  std::vector<std::pair<int,rational_t>> order;
public:
  Heuristic(bool dyn = false, bool con = false) : _dynamic(dyn), _const(con) {}
  virtual int get(int from, const HeuristicAlgorithm &algo) {return order[from].first;}
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
  ConstHeuristic(rational_t val, int size) : Heuristic(false, true) { for(int i = 0; i< size; i++)order.push_back(std::pair<arg_t,rational_t>(i,val));}
  auto get(int from, const HeuristicAlgorithm &algo) -> int override {return from;}
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
  const int window_size;
  const rational_t weight;
public:
  DynamicDegreeHeuristic (AttackRelation &ar, int window_size, rational_t weight);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};

class DynamicDegreeRatioHeuristic : public Heuristic {
  const int window_size;
public:
  DynamicDegreeRatioHeuristic (AttackRelation &ar, int window_size);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};

class DefensorHeuristic : public Heuristic {
  AttackRelation &ar;
  const int window_size;
  const rational_t weight;
public:
  DefensorHeuristic (AttackRelation &ar, int window_size, rational_t weight);
  int get(int from, const HeuristicAlgorithm &algo) override ;
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
  Eigen::MatrixXf *adjacency_matrix;
  int window_size;
public:
  DenseExponentialHeuristic (AAF &aaf, int window_size = 0);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};


class SCCHeuristic : public Heuristic {
public:
  SCCHeuristic(const AttackRelation&ar);
};
