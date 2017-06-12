#include <stack>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "heuristic.h"
#include "AAF.h"


class TarjansAlgorithm {
  typedef int node_t;

  const int n;
  const AttackRelation&ar;
  std::stack<node_t> stack;
  std::vector<bool> visited, in_component;

  void visit (node_t v) {
    root[v] = v;
    visited[v] = true;
    in_component[v] = false;
    stack.push(v);
    for (node_t w : ar.attacked_set(v)) {
      if (!visited[w])
        visit(w);
      if (!in_component[w]) {
        root[v] = std::min(root[v], root[w]);
      }
    }
    if (root[v] == v) {
      while (true) {
        node_t w = stack.top();
        stack.pop();
        in_component[w] = true;
        if (w==v)
          break;
      }
    }
  }

public:
  std::vector<node_t> root;

  TarjansAlgorithm (const AttackRelation&ar)
    : ar(ar), n(ar.arg_cnt), root(n), visited(n, false), in_component(n) {
      for (node_t v = 0; v < n; v++)
        if (!visited[v])
          visit(v);
  }

};

//Tarjan's algorithm
std::vector<int> compute_sccs(const AttackRelation &ar) {
  TarjansAlgorithm t(ar) ;
  return t. root;
}

OrderedSCCHeuristic::OrderedSCCHeuristic(const AttackRelation&ar) : Heuristic()  {
  TarjansAlgorithm t(ar) ;
  std::unordered_map<int, int> scc_size;
  for (int i : t.root) {
    if (scc_size.count(i) == 0)
      scc_size[i] = 1;
    else
      scc_size[i]++;
  }
  std::vector<std::pair<int, int>> sccs;
  for (auto key : scc_size)
    sccs.push_back(key);
  std::sort(sccs.begin(), sccs.end(), [](const std::pair<int,int>& a, const std::pair<int,int>& b) {
    return a.second < b.second;
  });
  std::unordered_map<int, int> new_scc_id;
  for (int i = 0; i < sccs.size(); i++)
    new_scc_id[sccs[i].first] = i;

  //std::cerr << "# of sccs:" << new_scc_id.size();

  for (int i = 0 ; i< ar.arg_cnt; i++) {
    order . push_back ({i, new_scc_id[t.root[i]]});
  }
}
