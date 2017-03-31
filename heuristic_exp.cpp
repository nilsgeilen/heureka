#include "heuristic.h"

#include <Eigen/SparseCore>

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

#include <Eigen/Core>
DenseExponentialHeuristic::DenseExponentialHeuristic (AAF &aaf) :Heuristic() {
  using namespace Eigen;
  typedef float entry_t;
  const int n = aaf.args.size();
  MatrixXf a(n,n);
  for(auto&att:aaf.atts)
  {
    a(att.active.id, att.passive.id) = -1;
  }

  auto pot = a, result = a;
  int divisor = 1;
  for (int i = 2 ; i < 4 ; i++) {
    pot = pot * a;
    divisor *= i;
    result += pot / divisor;
  }

  for (int i=0;i<n;i++) {
    order.push_back(std::pair<int,rational_t>(i,result.col(i).sum()));
  }
}
