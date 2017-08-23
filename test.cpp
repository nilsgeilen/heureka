#include "test.h"
#include "AAF.h"
#include "algebra.h"
#include "heuristic.h"

#include <iostream>
#include <fstream>

void print(std::vector<int> v){
  for (int i = 0; i< v.size();i++)
    std::cout << v[i] <<", ";
  std::cout << std::endl;
}

extern std::vector<float> betweenness_centrality(AttackRelation &ar);

extern std::vector<int> compute_sccs(const AttackRelation&ar);

void SCCTest() {
  AAF aaf = {
   {{0,"a"}, {1,"b"} ,{2,"c"},{3,"e"},{4,"d"},{5,"f"}},
   {{{0,"a"}, {1,"b"}},{{1,"b"}, {0,"a"}},{{3,"e"}, {0,"a"}}, {{0,"a"}, {3,"e"}}, {{4,"d"}, {2,"c"}}, {{4,"d"}, {5,"f"}}, {{5,"f"}, {4,"d"}}}
 };

 //std::cerr << aaf << std::endl;
  AttackRelation ar (aaf);

  auto sccs = compute_sccs(ar);
  print(sccs);

  SCCHeuristic scch {ar};
  std::cout << scch << std::endl;
}

void PowerIterationTest () {
  AAF aaf = {
   {{0,"a"}, {1,"b"} ,{2,"c"}},
   {{{0,"a"}, {1,"b"}},{{1,"b"}, {0,"a"}}}
 };
 std::cerr << aaf << std::endl;
  int n = aaf.args.size();
  Eigen::SparseMatrix<float> A(n,n);
  A.reserve(Eigen::VectorXf::Constant(n,100));
  populateAdjacencyMatrix(A, aaf, 1.f);

  std::cerr << A  << std::endl;

  Eigen::VectorXf x(n);
  powerIteration(A, x, n, 10);
  std::cerr << x << std::endl;
}




void test() {

 SCCTest();

  //PowerIterationTest();

}
