#include "solver.h"

namespace {
  /**
   * Defines the structure of labellings
   */
  typedef unsigned char label_t;

  namespace labels {
  constexpr label_t
      IN=0x2,
      UNDEC=0x4,
      OUT=0x8
    ;
  }
}

auto GroundedSolver::enum_exts (const AttackRelation &ar, int max_cnt) -> std::vector<std::vector<int>> {
  return std::vector<std::vector<int>> (1,find_ext(ar));
}


// Algorithm to find the grounded extension according to Nofal, Atkinson and Dunne (2014)
std::vector<int> GroundedSolver::find_ext(const AttackRelation &ar) {
    using namespace labels;
    std::vector<label_t> labels (ar.arg_cnt, UNDEC);
    int * agressor_cnt = new int [ar.arg_cnt];
    for (int i = 0; i < ar.arg_cnt ; i++) {
      agressor_cnt[i] = ar.indegree(i);
    }

    bool change = true;
    while (change) {
      change = false;
      for (int i = 0; i< ar.arg_cnt; i++) {
        if (labels[i] == UNDEC) {
          if (agressor_cnt[i] == 0) {
            labels[i] = IN;
            change = true;
            for (int atted : ar.attacked_set(i)) {
              if (labels[atted] != OUT) {
                labels[atted] = OUT;
                for (int attedatted : ar.attacked_set(atted)) {
                  agressor_cnt[attedatted] --;
                }
              }
            }
          }
        }
      }
    }


  std::vector<int> ext;
  for (int i = 0; i<ar.arg_cnt; i++) {
    if(labels[i] & IN)
      ext.push_back(i);
  }

  delete[] agressor_cnt;

  return ext;
}

bool GroundedSolver::justify (const AttackRelation &ar, arg_t arg, bool sceptical) {
  using namespace labels;
  std::vector<label_t> labels (ar.arg_cnt, UNDEC);
  while (true) {
    bool change = false;
    for (int i = 0; i< ar.arg_cnt; i++) {
      if (labels[i] == UNDEC) {
        bool set_in = true;
        for (int atter : ar.attacker_set(i)) {
          if (labels[atter] != OUT) {
            set_in = false;
            break;
          }
        }
        if (set_in) {
          if (i == arg) {
            return true;
          }
          change = true;
          labels[i] = IN;
          for (int atted : ar.attacked_set(i)) {
            if (atted == arg) {
              return false;
            }
            labels[atted] = OUT;
          }
        }
      }
    }
    if (!change)
      return false;
  }
}
