#include "AAF.h"

#include <Eigen/SparseCore>
#include <Eigen/Core>

namespace {
  typedef double rational_t;
  typedef int degree_counter_t;
  //doublette from collector.h
  typedef unsigned char label_t;
}

/**
 * An abstract interface which allows backtracking algorithm to store all information
 * which is relevant for heuristics in a unified way
 */
class HeuristicAlgorithm {
protected:
  /**
   * stores information about conflicts whith the partial solution for every argument and
   * how many of an argument's attackers are defeated by the partial solution
   */
  int *pos_range, *neg_range, *agressor_cnt;
  /**
   * stores every argument's label
   */
  std::vector<label_t> labels;
public:
  /**
   * @return an array which stores the number of undefeated attackers of every argument
   */
  inline const int *get_aggressor_cnt() const {return agressor_cnt;}
  /**
   * @return an array which stores the number of attacks against the solution from every
   * single argument
   */
  inline const int *get_neg_range() const {return neg_range;}
  /**
   * @return an array which stores the number of attacks from the solution of every argument
   */
  inline const int *get_pos_range() const {return pos_range;}
  /**
   * @return an array which stores every argument's label
   */
  inline const std::vector<label_t> &get_labels() const {return labels;}
};

/**
 * An object which represents a heuristic function
 */
class Heuristic {
  const bool _dynamic, _const;
protected:
  /**
   * Array which holds a static heuristic value for each argument
   */
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
 * Will parse a Heuristic object from a stream
 * @param  aaf    the AAF which contains the arguments which the heuristic sorts
 * @param  ar     the attack relation between those arguments
 * @param  source a stream which includes the definition of the heuristic as a formula in Polish notation
 * @return        the heuristic as a Heuristic object
 */
Heuristic * parseHeuristic (AAF&aaf, AttackRelation&ar, std::stringstream&source) ;

/**
 * A heuristic which assigns the same value to every argument
 * @param val  the value assigned to each argument
 * @param size the number of arguments of the aaf
 */
class ConstHeuristic: public Heuristic {
public:
  ConstHeuristic(rational_t val, int size) : Heuristic(false, true) { for(int i = 0; i< size; i++)order.push_back(std::pair<arg_t,rational_t>(i,val));}
  auto get(int from, const HeuristicAlgorithm &algo) -> int override {return from;}
};

/**
 * A heuristic which assigns each argument a score based on in- and outgoing paths
 * @param ar        the aaf as a attack relation
 * @param in_depth  ingoing path are considered up to this length
 * @param in_alpha  weighting parameter for inging paths
 * @param out_depth outgoing path are considered up to this length
 * @param out_alpha weighting parameter for outging paths
*/
class PathHeuristic : public Heuristic {
public:
  PathHeuristic (AttackRelation&ar, int in_depth, rational_t in_alpha, int out_depth, rational_t out_alpha);
};

/**
 * A heuristic baed on the outdegree-to-indegree-ratio of an argument's attackers and
 * attackees
 * @param ar the aaf as a attack relation
 */
class ExtendedDegreeRatioHeuristic : public Heuristic {
public:
  ExtendedDegreeRatioHeuristic (AttackRelation &ar);
};


/**
 * A heuristic which counts an argument's undefeated attackers and adds this number
 * to a static heuristic score
 * @param ar          the aaf as a attack relation
 * @param window_size the hehristic only considers this many arguments
 * @param weight      the score is added to a static heuristic with this weight
 */
class DynamicDegreeHeuristic : public Heuristic {
  const int window_size;
  const rational_t weight;
public:
  DynamicDegreeHeuristic (AttackRelation &ar, int window_size, rational_t weight);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};

/**
 * A heuristic which counts an argument's undefeated attackers, but divides a static
 * heuristic score through this number
 * @param ar          the aaf as a attack relation
 * @param window_size the hehristic only considers this many arguments
 */
class DynamicDegreeRatioHeuristic : public Heuristic {
  const int window_size;
public:
  DynamicDegreeRatioHeuristic (AttackRelation &ar, int window_size);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};

/**
 * This heuristic counts all attacks from an argument to attackers of the solution
 * and adds this number to a static heristic
 * @param ar          the aaf as a attack relation
 * @param window_size the hehristic only considers this many arguments
 * @param weight      the score is added to a static heuristic with this weight
 */
class DefensorHeuristic : public Heuristic {
  AttackRelation &ar;
  const int window_size;
  const rational_t weight;
public:
  DefensorHeuristic (AttackRelation &ar, int window_size, rational_t weight);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};

/**
 * Computes an argument's eigen centrality
 * @param aaf   the aaf as a attack relation
 * @param steps the number of iterations taken by power iteration
 */
class EigenHeuristic : public Heuristic {
public:
  EigenHeuristic (AAF&aaf, int steps);
};

/**
 * Computes an argument's betweenness centrality
 * @param aaf the AAF
 */
class BetweennessHeuristic : public Heuristic {
public:
  BetweennessHeuristic(AttackRelation&aaf);
};

Heuristic * getExponentialHeuristic (AAF &aaf);

/**
 * The matrix exponential heuristic as propsed by Corea and Thimm in its dynamic or
 * static form
 * @param aaf         an AAF
 * @param window_size the hehristic only considers this many arguments, the heuristic
 * is static if this id 0
 */
class DenseExponentialHeuristic : public Heuristic {
  Eigen::MatrixXf *adjacency_matrix;
  int window_size;
public:
  DenseExponentialHeuristic (AAF &aaf, int window_size = 0);
  int get(int from, const HeuristicAlgorithm &algo) override ;
};

/**
 * A heuirstic based on SCCs
 * @param ar the aaf as a attack relation
 */
class SCCHeuristic : public Heuristic {
public:
  SCCHeuristic(const AttackRelation&ar);
};
