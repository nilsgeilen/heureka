#include "heuristic.h"
#include "algebra.h"

#include <queue>
#include <stack>
#include <algorithm>


namespace {
  typedef int node_t;
}

  // Brandes' Algorithm
  std::vector<float>  betweenness_centrality(AttackRelation &ar) {
    int n = ar.arg_cnt;
    std::vector<float> c_b (n, 0.0);
    for (int s = 0; s < n; s++) {
      std::stack<node_t> S;
      std::vector<std::vector<node_t>> path (n);
      std::vector<float> sigma (n, 0.0);
      sigma[s] = 1.0;
      std::vector<int> d (n, -1);
      d[s] = 0;
      std::queue<node_t> Q;
      Q.push(s);
      while (!Q.empty()) {
        node_t v = Q.front();
        Q.pop();
        S.push(v);
        for (node_t w : ar.attacked_set(v)) {
          if (d[w] < 0) {
            Q.push(w);
            d[w] = d[v] + 1;
          }
          if (d[w] == d[v] + 1) {
            sigma[w] += sigma[v];
            path[w].push_back(v);
          }
        }
      }
      std::vector<float> delta (n, 0.0);
      while(!S.empty()) {
        node_t w = S.top();
        S.pop();
        for (node_t v : path[w]) {
          delta[v] += sigma[v] / sigma[w] * (1.0 + delta[w]);
        }
        if (w != s) {
          c_b[w] += delta[w];
        }
      }
    }
    return c_b;
  }

BetweennessHeuristic::BetweennessHeuristic (AttackRelation &ar) :Heuristic(){
  int i = 0;
  for (float c_b : betweenness_centrality(ar))
    order.push_back(std::pair<int,float>(i++,c_b));
}


EigenHeuristic::EigenHeuristic(AAF&aaf, int steps) :Heuristic(){
  using namespace Eigen;
  typedef float entry_t;
  int n = aaf.args.size();
  Eigen::SparseMatrix<entry_t> A(n,n);
  A.reserve(Eigen::VectorXf::Constant(n,100));
  populateAdjacencyMatrix(A, aaf, 1.f);

  VectorXf x(n);
  powerIteration(A, x, n, steps);

  for (int i=0;i<n;i++) {
    order.push_back(std::pair<int,rational_t>(i,-x[i]));
  }

}
