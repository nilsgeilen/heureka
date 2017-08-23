#include <stack>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "heuristic.h"
#include "AAF.h"



class TarjansAlgorithm {
public:
  std::vector<int> exec() {
    for (node_t v = 0; v < n; v++)
      if (!visited[v])
        visit(v);
      return sccs;
  }


  TarjansAlgorithm (const AttackRelation&ar)
    : ar(ar), n(ar.arg_cnt), indices(n), lowlink(n), sccs(n),
    visited(n, false), on_stack(n, false) {}

private:
  typedef int node_t;

  const int n;
  const AttackRelation&ar;
  std::stack<node_t> stack {};
  std::vector<bool> visited, on_stack;
  std::vector<int> indices, lowlink, sccs;
  int index = 0, component = 0;


  void visit (node_t v) {
    indices[v] = index;
    lowlink[v] = index ++;
    visited[v] = true;
    on_stack[v] = true;
    stack.push(v);
    for (node_t w : ar.attacked_set(v)) {
      if (!visited[w])
        {visit(w);
        lowlink[v] = std::min(lowlink[v], lowlink[w]);}
      else if (on_stack[w]) {
        lowlink[v] = std::min(lowlink[v], indices[w]);
      }
    }
    if (lowlink[v] == indices[v]) {
      while (true) {
        node_t w = stack.top();
        stack.pop();
        on_stack[w] = false;
        sccs[w] = component;
        if (w==v)
          break;
      }
      component++;
    }
  }



};




/*class TarjansAlgorithm {
  typedef int node_t;

  const int n;
  const AttackRelation&ar;
  std::stack<node_t> stack {};
  std::vector<bool> visited, in_component;
  std::vector<int> sccs;
  int scc = 0;

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
        sccs[w] = scc;
        if (w==v)
          break;
      }
      scc ++;
    }
  }

public:
  std::vector<node_t> root;

  std::vector<int> exec() {
    for (node_t v = 0; v < n; v++)
      if (!visited[v])
        visit(v);
    return sccs;
  }

  TarjansAlgorithm (const AttackRelation&ar)
    : ar(ar), n(ar.arg_cnt), root(n), visited(n, false), in_component(n, true), sccs(n) {}

};*/

//Tarjan's algorithm
std::vector<int> compute_sccs(const AttackRelation &ar) {
  TarjansAlgorithm t(ar) ;
  return t. exec();
}

struct SCC {
  std::unordered_set<SCC*> parents;
  int id, root;

  SCC (int id, int root) : id (id), root(root), parents() {}

  void attacks(SCC* b, int i = 0) {
    if (id < b -> id) {
      if (!parents.count(b)) // for security
        b -> parents.insert(this);
      int temp = id;
      id = b->id;
      b->id = temp;
      for (SCC* parent : parents) {
        parent -> attacks(this, i+1);
      }
    }
  }
};

SCCHeuristic::SCCHeuristic(const AttackRelation&ar) : Heuristic()  {
  TarjansAlgorithm t(ar) ;
  auto  s = t.exec();


  std::unordered_map<int, SCC*> sccs;
  int id = 0;

  for (int i : s) {
    if (!sccs.count(i))
      sccs[i] = new SCC {++id, i};
  }


  for (arg_t atter = 0; atter < ar.arg_cnt; atter ++) {
    for (arg_t atted : ar.attacked_set(atter)) {
      const int root_a = (s)[atter], root_b = (s)[atted];
      sccs[root_a] -> attacks(sccs[root_b]);

    }
  }

/*std::unordered_map<int, int> sccs;
int id = 0;

for (int i : t.root) {
  if (!sccs.count(i))
    sccs[i] = ++id;
}


for (int i = 0; i < sccs.size(); i ++)
for (arg_t atter = 0; atter < ar.arg_cnt; atter ++) {
  for (arg_t atted : ar.attacked_set(atter)) {
    const int root_a = t.root[atter], root_b = t.root[atted];
    if (sccs[root_a] < sccs[root_b])
      std::swap(sccs[root_a],sccs[root_b]);
  }
}*/


  for (int i = 0 ; i< ar.arg_cnt; i++) {
    order . push_back ({i, sccs[(s)[i]]->id});
  }
}
