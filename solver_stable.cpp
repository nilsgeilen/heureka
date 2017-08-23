#include "solver.h"
#include "collector.h"

#include <iostream>
#include <stack>

namespace {

  using namespace labels;

  /**
   * An algorithm to enumerate stable extensions
   */
  class StableEnumerator : HeuristicAlgorithm {

    const AttackRelation &ar;
    const int n;

    std::stack<int> decisions_index, decisions_arg;

  public:
    StableEnumerator(const AttackRelation &ar)
        : ar(ar), n(ar.arg_cnt) {
      labels = labelling_t(n, BLANK);
      pos_range = new int [n]();
      neg_range = new int [n]();
      agressor_cnt = new int [n];
      for (int i = 0; i < n ; i++) {
        agressor_cnt[i] = ar.indegree(i);
      }
    }

    ~StableEnumerator() {
      delete[] pos_range;
      delete[] neg_range;
      delete[] agressor_cnt;
    }

    bool set_in (arg_t arg, int index, ExtensionCollector &results) {
      labels[arg] = IN;
      decisions_index.push(index);
      decisions_arg.push(arg);
      for (arg_t a : ar.attacker_set(arg)) {
        neg_range[a] ++;
      }
      for (arg_t atted : ar.attacked_set(arg)) {
        pos_range[atted] ++;
        if (pos_range[atted] == 1)
          for (arg_t attedatted : ar.attacked_set(atted)) {
            agressor_cnt[attedatted]--;
          }
      }

      for (arg_t atter : ar.attacker_set(arg)) {
        if(!set_out(atter, results))
          return false;
      }
      for (arg_t atted : ar.attacked_set(arg)) {
        set_out(atted, results);
      }
      for (arg_t atted : ar.attacked_set(arg)) {
          if (pos_range[atted] == 1)
            for (arg_t attedatted : ar.attacked_set(atted)) {
             if (labels[attedatted] != IN && agressor_cnt[attedatted] == 0) {
                if (labels[attedatted] == OUT)
                  return false;
                if (labels[attedatted] == BLANK)
                  if (!set_in(attedatted, indices::BACKTRACK, results))
                    return false;
              }
            }
      }
      if (is_stable())  {
        results.report_ext_labelling(labels);
        return false;
      }
      return true;
    }

    bool set_out (arg_t arg, ExtensionCollector &results, int index = indices::BACKTRACK) {
      if (labels[arg] & BLANK) {
        labels[arg] = labels::OUT;
        decisions_index.push(index);
        decisions_arg.push(arg);
      }
      if ( pos_range[arg] != 0 )
        return true;
      int candidate_cnt = 0;
      arg_t candidate = -1;
      for (arg_t atteratter : ar.attacker_set(arg)) {
        if (labels[atteratter] != OUT
            && pos_range[atteratter] == 0
            && neg_range[atteratter] == 0) {
          candidate_cnt ++;
          candidate = atteratter;
          if (candidate_cnt == 2)
            break;
        }
      }
      if (candidate_cnt == 0) {
        return false;
      }
      if (candidate_cnt == 1) {
        return set_in(candidate, indices::BACKTRACK, results);
      }
      return true;
    }

    inline bool is_stable() {
      for (int i = 0; i< n; i++)  {
        if (labels[i] & IN) {
    //      if (pos_range[i] > 0 || neg_range[i] > 0)
      //      return false;
          if (agressor_cnt[i] > 0)
            return false;
          continue;
        } else if (pos_range[i] == 0)// || agressor_cnt[i] == 0)
          	return false;
      }
      return true;
    }

    void enumStable(Heuristic &heuristic, ExtensionCollector &results) {

      int index = -1;

      for (arg_t arg : GroundedSolver().find_ext(ar)) {
          if(labels[arg] == BLANK ) {
            if (!set_in(arg, indices::STOP, results))
              return;
          } else if (labels[arg] == OUT)
            return;
      }

      for (arg_t arg : ar.self_attacker_set()) {
        if (labels[arg] == BLANK) {
            if(!set_out(arg, results, indices::STOP))
              return;
        } else if (labels[arg] == IN)
          return;
      }

      while (true) {

       if (++index < n) {
          arg_t arg = heuristic.get(index, *this);
          if (labels[arg] & BLANK) {
              if (!set_in(arg, index, results))
                goto backtrack;
          }
          continue;
        }
  backtrack:
          if (results.is_stopped()) {
            break;
          }
          if (decisions_index.empty()) {
            break;
          }
          index = decisions_index.top();
          arg_t arg = decisions_arg.top();
          decisions_index.pop();
          decisions_arg.pop();
          if (index == indices::STOP)
            break;
          if (labels[arg] == IN) {
            for (arg_t a : ar.attacker_set(arg)) {
              neg_range[a] --;
            }
            for (arg_t a : ar.attacked_set(arg)) {
              pos_range[a] --;
              if (pos_range[a] == 0)
                for (arg_t attedatted : ar.attacked_set(a)) {
                  agressor_cnt[attedatted]++;
                }
            }
          }
          labels[arg] = BLANK;
          if (index == indices::BACKTRACK || !set_out(arg, results)) {
            goto backtrack;
          }
      }
    }
  };

}

std::vector<std::vector<int>> StableSolver::enum_exts(const AttackRelation &ar, int max_cnt) {
  ExtensionEnumerator results {max_cnt};
  StableEnumerator enumerator {ar};
  enumerator.enumStable(heuristic, results);
  return results.exts;
}



bool StableSolver::justify (const AttackRelation &ar, arg_t arg, bool sceptical) {
  ArgumentJustifier results {arg, sceptical};
  StableEnumerator enumerator {ar};
  if (sceptical) {
    if(!enumerator.set_out(arg, results, indices::STOP))
      return results.is_justified();
  } else {
    if(!enumerator.set_in(arg, indices::STOP, results))
      return results.is_justified();
  }
  enumerator.enumStable(heuristic, results);
  return results.is_justified();
}
