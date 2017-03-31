#include "heuristic.h"

#include <algorithm>

PathHeuristic::PathHeuristic (AttackRelation&ar, int in_depth, rational_t in_alpha, int out_depth, rational_t out_alpha):Heuristic() {
  const int n = ar.arg_cnt;
  for (int i = 0; i < n; i++) {
    order.push_back(std::pair<int,rational_t>(i,
      + in_alpha*(rational_t)ar.indegree(i)
      + out_alpha*(rational_t)ar.outdegree(i)));
  }
  int * last = new int[n], * next = new int [n];
  if (in_depth > 1) {
    for (int i = 0; i < n; i++) {
      last[i] = ar.indegree(i);
    }
    for (int j = 2; j <= in_depth; j++) {
      const rational_t factor = std::pow(in_alpha, j);
      for (int i = 0; i < n; i++) {
        next[i] = 0;
        for (arg_t atter : ar.attacker_set(i)) {
          next[i] += last[atter];
        }
        order[i].second += factor * (rational_t)next[i];
      }
      int * temp = last;
      last = next;
      next = temp;
    }
  }
  if (out_depth > 1) {
    for (int i = 0; i < n; i++) {
      last[i] = ar.outdegree(i);
    }
    for (int j = 2; j <= out_depth; j++) {
      const rational_t factor = std::pow(out_alpha, j);
      for (int i = 0; i < n; i++) {
        next[i] = 0;
        for (arg_t atted : ar.attacked_set(i)) {
          next[i] += last[atted];
        }
        order[i].second += factor * (rational_t)next[i];
      }
      int * temp = last;
      last = next;
      next = temp;
    }
  }
  delete[] last;
  delete[] next;
}



ExtendedDegreeRatioHeuristic::ExtendedDegreeRatioHeuristic (AttackRelation &ar) :Heuristic(){
  const int n = ar.arg_cnt;
  rational_t * deg_ratio = new rational_t [n];
  for (int i = 0; i < n; i++) {
    //const rational_t divisor = ar.indegree(i) ? ar.indegree(i) : 1.0;
    deg_ratio[i] = ((rational_t)ar.outdegree(i)+1.0) / ((rational_t)ar.indegree(i)+1.0);//(rational_t)ar.outdegree(i) / divisor;
  }
  for (int i = 0; i < n; i++) {
    rational_t  atter_sum = 1.0;
    for (arg_t atter : ar.attacker_set(i))
      atter_sum += deg_ratio[atter];
    rational_t  atted_sum = 1.0;
    for (arg_t atted : ar.attacked_set(i))
      atted_sum += deg_ratio[atted];
    order.push_back(std::pair<int,rational_t>(i,
          atted_sum / atter_sum));
  }
  delete[] deg_ratio;
}

WeightedDegreeHeuristic::WeightedDegreeHeuristic (AttackRelation &ar):Heuristic() {
  const int n = ar.arg_cnt;
  for (int i = 0; i < n; i++) {
    rational_t weighted_in_degree = 0.0;
    for (arg_t atter : ar.attacker_set(i)) {
      if (ar.indegree(atter) == 0) {
        weighted_in_degree = - 900000;
        break;
      }
      weighted_in_degree -= 1000.0 / ar.indegree(atter);
    }
    order.push_back(std::pair<int,rational_t>(i,
        weighted_in_degree + (rational_t)ar.outdegree(i)));
  }
}

DynamicDegreeHeuristic::DynamicDegreeHeuristic (AttackRelation &ar, int window_size, rational_t weight)
  : ar(ar), window_size(window_size), weight(weight), Heuristic(true) {
    for (int i = 0; i < ar.arg_cnt; i++)
      order.push_back(std::pair<int,rational_t>(i, 0.0));
}

int DynamicDegreeHeuristic::get(int from, const degree_counter_t * const adjusted_indegree) {
  if (adjusted_indegree == nullptr)
    return order[from].first;

  arg_t max_pos = from;
  rational_t max_val = order[from].second +weight * (rational_t)adjusted_indegree[from];
  const int window_end = from + window_size;
  for (int i = from + 1; i < order.size() && i < window_end; i++) {
    const float val = order[i].second +weight * (rational_t)adjusted_indegree[i];
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
