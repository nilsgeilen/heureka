#include "AAF.h"
#include "heuristic.h"

/**
 * An interface for solvers for abstract argumentation problems
 */
class Solver {
public:
  /**
   * Enumerates extensions
   * @param  ar      the attack relation of the underlying AAF
   * @param  max_cnt number of needed extensions, will stop after enough are found
   * @return         extensions of the AAF
   */
  virtual auto enum_exts (const AttackRelation &ar, int max_cnt = 0) -> std::vector<std::vector<int>> = 0;
  /**
   * Dtermines whether an argument is justified
   * @param  ar        the attack relation of the underlying AAF
   * @param  arg       the argument to be justified
   * @param  sceptical the argument has to be sceptically justified iff true and creduously elsewise
   * @return           true iff the argument is justified
   */
  virtual bool justify (const AttackRelation &ar, arg_t arg, bool sceptical) = 0;
};

/**
 * A solver for grounded semantics
 */
class GroundedSolver : public Solver {
public:
  /**
   * Computes the grounded extension
   * @param  ar      the attack relation of the underlying AAF
   * @return         the grouded extension
   */
  std::vector<int> find_ext(const AttackRelation &ar);
  auto enum_exts (const AttackRelation &ar, int max_cnt = 0) -> std::vector<std::vector<int>> override;
  bool justify (const AttackRelation &ar, arg_t arg, bool sceptical) override;
};

/**
 * A heuristic backtracking solver for stable semantics
 */
class StableSolver : public Solver {
  Heuristic &heuristic;
public:
  /**
   * Creates a new solver
   * @param  heuristic  the heuristic used by the solver
   */
  StableSolver(Heuristic &heuristic):heuristic(heuristic){}
  auto enum_exts (const AttackRelation &ar, int max_cnt = 0) -> std::vector<std::vector<int>> override;
  bool justify (const AttackRelation &ar, arg_t arg, bool sceptical) override;
};

/**
 * A heuristic backtracking solver for complete semantics
 */
class CompleteSolver : public Solver {
  Heuristic &heuristic;
public:
  /**
   * Creates a new solver
   * @param  heuristic  the heuristic used by the solver
   */
  CompleteSolver(Heuristic &heuristic):heuristic(heuristic){}
  auto enum_exts (const AttackRelation &ar, int max_cnt = 0) -> std::vector<std::vector<int>> override;
  bool justify (const AttackRelation &ar, arg_t arg, bool sceptical) override;
};

/**
 * A heuristic backtracking solver for preferred semantics
 */
class PreferredSolver : public Solver {
  Heuristic &heuristic;
public:
  /**
   * Creates a new solver
   * @param  heuristic  the heuristic used by the solver
   */
  PreferredSolver(Heuristic &heuristic):heuristic(heuristic){}
  auto enum_exts (const AttackRelation &ar, int max_cnt = 0) -> std::vector<std::vector<int>> override;
  bool justify (const AttackRelation &ar, arg_t arg, bool sceptical) override;
};
