#include <ostream>
#include <vector>

#pragma once

typedef int arg_t;

/**
 * Stores information about an argument
 */
struct Argument {
  int id;
  std::string label;
};

std::ostream& operator <<(std::ostream& stream, const Argument& arg) ;

bool operator==(const Argument &x, const Argument &y)  ;

bool operator!=(const Argument &x, const Argument &y) ;

/**
 * Represents an attack
 */
struct Attack {
  Argument active, passive;
};

std::ostream& operator <<(std::ostream& stream, const Attack& att) ;

bool operator==(const Attack &x, const Attack &y)  ;

bool operator!=(const Attack &x, const Attack &y) ;

/**
 * Straight forward representation of an AAF
 */
struct AAF {
  std::vector<Argument> args;
  std::vector<Attack> atts;
  void print(std::vector<int> ext, std::ostream &str);
  void todot(std::ostream &str);
};

std::ostream& operator <<(std::ostream& stream, const AAF& aaf) ;

/**
 * Neighbourhood-centric representation of an AAF
 */
class AttackRelation {
   std::vector<std::vector<int>> atters, atteds;
   std::vector<arg_t> self_atters;
 public:
   /**
    * the number of arguments in the AAF
    */
   const int arg_cnt;
   /**
    * Creates an AttackRelation according to an AAF
    */
   AttackRelation(AAF &aaf);
   /**
    * Maps an argument a to its attacker set {a}^-
    * @param  arg an argument
    * @return     the set of attackers
    */
   inline const std::vector<int> &  attacker_set(int i) const {return atters[i];}
   /**
    * Maps an argument a to the set of arguments attacked by it {a}^+
    * @param  arg an argument
    * @return     the set of attacked arguments
    */
   inline const std::vector<int> &  attacked_set(int i) const {return atteds[i];}
   /**
    * Maps an argument to its indegree
    * @param  arg an argument
    * @return     the indegree deg^-(arg)
    */
   inline int indegree(int arg) const {return atters[arg].size();}
   /**
    * Maps an argument to its outdegree
    * @param  arg an argument
    * @return     the outdegree deg^+(arg)
    */
   inline int outdegree(int arg) const {return atteds[arg].size();}
   /**
    * @return the set of arguments which attack themselves {a in A | a -> a}
    */
   inline const std::vector<arg_t> & self_attacker_set() const {return self_atters;}
};
