#include "solver.h"
#include "collector.h"

#include <iostream>
#include <stack>



namespace {

  class MaximalFilter : public ExtensionCollector {
    ExtensionCollector &collector;
    std::vector<labelling_t> ext_labellings;
  public:
    MaximalFilter (ExtensionCollector &collector) :collector(collector) {}
    void report_ext_labelling (const labelling_t &labels) {
      std::vector<int> ext;
      for (int i = 0; i<labels.size(); i++) {
        if(labels[i] == labels::IN)
          ext.push_back(i);
      }
      for (auto &ext_labelling : ext_labellings) {
        bool included = true;
        for (int arg : ext) {
          if (ext_labelling[arg] != labels::IN) {
            included = false;
            break;
          }
        }
        if (included) {
          return;
        }
      }
      ext_labellings.push_back(labels);
      collector.report_ext_labelling(labels, ext);
      if (collector.is_stopped())
        stop();
    }
  };


using namespace labels;

/**
 * An algorithm to enumerate complete extensions
 */
class CompleteEnumerator : HeuristicAlgorithm {

  const AttackRelation &ar;
  const int n;


  bool *defended;
  std::stack<int> decisions_index, decisions_arg;
  int in_cnt = 0, defended_cnt = 0;

  const bool preferred;
  bool poss_max = true;

public:

  CompleteEnumerator(const AttackRelation &ar, bool preferred = false)
      : ar(ar), n(ar.arg_cnt), preferred(preferred)  {
    labels = labelling_t(n, BLANK);
    pos_range = new int [n]();
    neg_range = new int [n]();
    agressor_cnt = new int [n];
    for (int i = 0; i < n ; i++) {
      agressor_cnt[i] = ar.indegree(i);
      if (agressor_cnt[i] == 0)
        defended_cnt ++;
    }
  }

  ~CompleteEnumerator() {
    delete[] pos_range;
    delete[] neg_range;
    delete[] agressor_cnt;
  }

  bool set_in (arg_t arg, int index) {
    labels[arg] = IN;
    in_cnt ++;
    decisions_index.push(index);
    decisions_arg.push(arg);
    poss_max = true;
    for (arg_t atter : ar.attacker_set(arg)) {
      neg_range[atter] ++;
    }
    for (arg_t atted : ar.attacked_set(arg)) {
      pos_range[atted] ++;
      if (pos_range[atted] == 1)
        for (arg_t attedatted : ar.attacked_set(atted)) {
          agressor_cnt[attedatted]--;
          if (agressor_cnt[attedatted] == 0)
            defended_cnt ++;
        }
    }
//    if (neg_range[arg] > 0)
  //    return false;
    for (arg_t atter : ar.attacker_set(arg)) {
      if(!set_out(atter))
        return false;
    }
    for (arg_t atted : ar.attacked_set(arg)) {
          if(!set_out(atted))
            return false;
        }
    for (arg_t atted : ar.attacked_set(arg)) {
            if (pos_range[atted] == 1)
              for (arg_t attedatted : ar.attacked_set(atted)) {
               if (agressor_cnt[attedatted] == 0) {
                  if (labels[attedatted] == OUT)
                    return false;
                  if (labels[attedatted] == BLANK)
                    if (!set_in(attedatted, indices::BACKTRACK))
                      return false;
                }
              }
        }
/*    for (arg_t atted : ar.attacked_set(arg)) {
      if(!set_out(atted))
        return false;
        if (pos_range[atted] == 1)
          for (arg_t attedatted : ar.attacked_set(atted)) {
           if (agressor_cnt[attedatted] == 0) {
              if (labels[attedatted] & OUT)
                return false;
              if (labels[attedatted] & BLANK)
                if (!set_must_in(attedatted, -1))
                  return false;
            }
          }
    }*/
    return true;
  }

  bool set_out (arg_t arg, int index = indices::BACKTRACK) {
    if (labels[arg] & BLANK) {
      labels[arg] = labels::OUT;
      decisions_index.push(index);
      decisions_arg.push(arg);
    }
    if (neg_range[arg] > 0 && pos_range[arg] == 0) {
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
      if (!candidate_cnt) {
        return false;
      }
      if (candidate_cnt ==1) {
        if (!set_in(candidate, indices::BACKTRACK))
          return false;
      }
    }
    return true;
  }

  void enumComplete(Heuristic &heuristic, ExtensionCollector &results) {

    for (arg_t arg : GroundedSolver().find_ext(ar)) {
      if (labels[arg] == BLANK) {
        if(!set_in(arg, indices::STOP))
          return;
      } else if (labels[arg] == OUT)
        return;
    }

    for (arg_t arg : ar.self_attacker_set()) {
      //no return statement, because of empty set
      if (labels[arg] == BLANK) {
          if(!set_out(arg, indices::STOP));
      }
    }

    int index = -1;

    while (true) {

      if (++index == n) {
        if (preferred && ! poss_max)
          goto backtrack;
        if (in_cnt != defended_cnt)
          goto backtrack;
        for (int i = 0 ; i < n ; i++) {
          if (neg_range[i] > 0 && pos_range[i] == 0)
           goto backtrack;
        //  if (neg_range[i] == 0 && pos_range[i] == 0) {
          //  if (labels[i] == OUT && agressor_cnt[i] == 0)
            //  goto backtrack;
            //if (labels[i] == IN && agressor_cnt[i] > 0)
            //  goto backtrack;
          //}
        }
        poss_max = false;
        results.report_ext_labelling(labels);
        if (results.is_stopped()) {
          break;
        }
      } else {
        arg_t arg = heuristic.get(index, *this);
        if (labels[arg] == BLANK) {
            if (!set_in(arg, index))
              goto backtrack;
        }
        continue;
      }
backtrack:
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
        in_cnt --;
        for (arg_t a : ar.attacker_set(arg)) {
          neg_range[a] --;
        }
        for (arg_t a : ar.attacked_set(arg)) {
          pos_range[a] --;
          if (pos_range[a] == 0)
            for (arg_t attedatted : ar.attacked_set(a)) {
              agressor_cnt[attedatted]++;
              if (agressor_cnt[attedatted] == 1)
                defended_cnt --;
            }
        }
      }
      labels[arg] = BLANK;
      if (index == indices::BACKTRACK || !set_out(arg)) {
        goto backtrack;
      }
    }

  }
};

}

std::vector<std::vector<int>> CompleteSolver::enum_exts(const AttackRelation &ar, int max_cnt) {
  ExtensionEnumerator results {max_cnt};
  CompleteEnumerator enumor(ar);
  enumor.enumComplete(heuristic, results);
  return results.exts;
}


bool CompleteSolver::justify (const AttackRelation &ar, arg_t arg, bool sceptical) {
  if (sceptical) {
    return GroundedSolver().justify(ar, arg, sceptical);
  } else {
    ArgumentJustifier results {arg, sceptical};
    CompleteEnumerator enumor(ar);
  //  enumor.set_must_in(arg, -2);
    enumor.enumComplete(heuristic, results);
    return results.is_justified();
  }
}


std::vector<std::vector<int>> PreferredSolver::enum_exts(const AttackRelation &ar, int max_cnt) {
  ExtensionEnumerator results {max_cnt};
  MaximalFilter filter {results};
  CompleteEnumerator enumor(ar, true);
  enumor.enumComplete(heuristic, filter);
  return results.exts;
}


bool PreferredSolver::justify (const AttackRelation &ar, arg_t arg, bool sceptical) {
  ArgumentJustifier results {arg, sceptical};
  MaximalFilter filter {results};
  CompleteEnumerator enumor(ar, true);
  if (sceptical) {
      //nothing, because maxity
  } else {
  //  if(!enumor.set_must_in(arg, -2))
    //  return results.is_justified();
  }
  enumor.enumComplete(heuristic, filter);
  return results.is_justified();
}
