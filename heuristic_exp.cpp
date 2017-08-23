#include "heuristic.h"

#include "collector.h"



template<typename entry_t, int major>
class ExponentialHeuristic : public Heuristic {
public:
  ExponentialHeuristic (AAF &aaf) {
    int n = aaf.args.size();
    Eigen::SparseMatrix<entry_t, major> a(n,n);
    a.reserve(Eigen::VectorXf::Constant(n,100));
    std::vector<Eigen::Triplet<entry_t>> tripletList(aaf.atts.size());
    for(auto&att:aaf.atts)
    {
      tripletList.push_back(Eigen::Triplet<entry_t>(att.active.id, att.passive.id,-1));
    }
    a.setFromTriplets(tripletList.begin(), tripletList.end());

    auto pot = a, result = a;
    int divisor = 1;
    for (int i = 2 ; i < 4 ; i++) {
      pot = pot * a;
      divisor *= i;
      result += pot / divisor;
    }

    for (int i=0;i<n;i++) {
      order.push_back(std::pair<int,float>(i,result.col(i).sum()));
    }

  }
};

Heuristic * getExponentialHeuristic(AAF&aaf) {
  return new ExponentialHeuristic<float, Eigen::ColMajor>(aaf);
}


DenseExponentialHeuristic::DenseExponentialHeuristic (AAF &aaf, int window_size)
    :Heuristic(window_size), window_size(window_size) {
  using namespace Eigen;
  typedef float entry_t;
  const int n = aaf.args.size();
  MatrixXf a(n,n);
  for(auto&att:aaf.atts) {
    a(att.active.id, att.passive.id) = -1;
  }

  auto pot = a, result = a;
  int divisor = 1;
  for (int i = 2 ; i < 4 ; i++) {
    pot = pot * a;
    divisor *= i;
    result += pot / divisor;
  }

  if (is_dynamic())
    adjacency_matrix = new MatrixXf(result);

  for (int i=0;i<n;i++) {
    order.push_back(std::pair<int,rational_t>(i,result.col(i).sum()));
  }
}

int DenseExponentialHeuristic::get(int from, const HeuristicAlgorithm &algo)  {
  using namespace Eigen;

  if (!is_dynamic()) {
    return order[from].first;
  }

  auto &labels = algo.get_labels();

  arg_t max_pos = from;
  rational_t max_val = 0.0;
  const int window_end = from + window_size;
  for (int i = from; i < order.size() && i < window_end; i++) {
    if (labels[i] != labels::BLANK) {
      continue;
    }
    rational_t val = 0.0;
    for (int j = 0; j < order.size(); j++)
      if (labels[j] == labels::IN)
        val += (*adjacency_matrix)(j,i);
    if (val > max_val) {
      max_pos = i;
      max_val = val;
    }
  }
  auto temp = order[from];
  order[from] = order[max_pos];
  order[max_pos] = temp;

  return order[from].first;
}
