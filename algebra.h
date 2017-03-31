#include <Eigen/SparseCore>

/**
 * Constructs the adjacency matrix of an AAF
 * @param A   the matrix to be populated
 * @param aaf the AAF
 * @param val the value which indicates an edge
 */
template<class entry_t>
void populateAdjacencyMatrix (Eigen::SparseMatrix<entry_t>&A, AAF&aaf, entry_t val) {
  std::vector<Eigen::Triplet<entry_t>> tripletList(aaf.atts.size());
  for(auto&att:aaf.atts)
  {
    tripletList.push_back(Eigen::Triplet<entry_t>(att.active.id, att.passive.id,val));
  }
  A.setFromTriplets(tripletList.begin(), tripletList.end());
}

/**
 * Computes the dominant eigenvector
 * @param A     an adjacency matrix
 * @param x     the eigenvector to be populated
 * @param n     = |x|
 * @param steps the number of steps the algorithm performs
 */
template<class entry_t>
void powerIteration (Eigen::SparseMatrix<entry_t>&A, Eigen::VectorXf&x, int n, int steps) {
  for (int i=0; i<n;i++)
    x[i] = 1.0;///n;

  for (int i=0; i<steps; i++) {
    Eigen::VectorXf y = A*x;
    y.normalize();
    x = y;
  }
}
