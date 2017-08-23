#include <iostream>
#include <chrono>
#include <regex>
#include <fstream>

#include "parser.h"
#include "solver.h"
#include "test.h"


namespace {
  using namespace std::chrono;
  /**
   * Asimple class which records time
   */
  class Clock {
    high_resolution_clock::time_point start = high_resolution_clock::now();
  public:
    /**
     * Prints the time past since creation or the last stop
     * @param info to be printed along with passed time
     */
    void stop(std::string info) {
      auto now = high_resolution_clock::now();
      auto duration = duration_cast<milliseconds>( now-start ).count();
      std::cerr <<"runtime " << info << ":\t"<< duration<<"ms"<<std::endl;
      start = now;
    }
  };
}


int main(int argcnt, char ** args) {
  if (argcnt == 1) {
    std::cout << "heureka 0.2" << std::endl
      << "Nils Geilen <geilenn@uni-koblenz.de>" << std::endl
      << "Matthias Thimm <thimm@uni-koblenz.de>" << std::endl;
  } else if (std::string {args[1]} == "--formats") {
    std::cout << "[apx,tgf]" << std::endl;
  } else if (std::string {args[1]} == "--problems") {
    std::cout << "[EE-ST,EE-CO,EE-PR,SE-ST,SE-CO,SE-PR,SE-GR,DC-ST,DC-CO,DC-PR,DC-GR,DS-ST,DS-CO,DS-PR]" << std::endl;
  } else if (std::string {args[1]} == "--test") {
    test();
  } else {
    std::string task, semantics, path, format, item, param_heuristic, param_debug_options;
    for (int i = 1; i < argcnt - 1; i+=2) {
      std::string param_specifier {args[i]};
      if (param_specifier == "-p") {
        std::cmatch ms;
        std::string arg = args[i+1];
        if (arg.size() == 5) {
          task = {arg[0], arg[1]};
          semantics = {arg[3], arg[4]};
        } else {
          std::cerr << "Fail: Unknown task: " <<  arg << std::endl;
          return 0;
        }
      } else if (param_specifier == "-f") {
        path = args[i+1];
      } else if (param_specifier == "-fo") {
        format = args[i+1];
      } else if (param_specifier == "-a") {
        item = args[i+1];
      } else if (param_specifier == "-H") {
        param_heuristic = args[i+1];
      } else if (param_specifier == "-d") {
        param_debug_options = args[i+1];
      }
    }


    /**
     * infer format from path
     */
    if (!format.size() && path.size() > 4) {
      auto ending = path.substr(path.length() -4);
      if (ending == ".apx")
        format = "apx";
      else if (ending == ".tgf")
        format = "tgf";
    }

    /**
     * abort if an obligatory param is missing
     */
    if (!(task.size() && semantics.size() && path.size() && format.size())) {
      std::cerr << "Fail: Obligatory params missing" << std::endl;
      return 0;
    }

    Clock clock;

    AAF aaf ;

    std::ifstream file;
    file.open (path);
    if (!file.is_open()) {
      std::cerr << "Fail: Could not open file \"" << path << '"' << std::endl;
      return 0;
    }

    if (format == "apx") {
      parseAPX(aaf, file);
    } else {
      parseTGF(aaf, file);
    }
    file.close();

    if (param_debug_options.size()) {
      std::cerr << format << " ; ";
      std::cerr << task << "-" << semantics<<" ; ";
      std::cerr << param_heuristic << " ; ";
      std::cerr <<"|A| = "<<aaf.args.size()<< " ; ";
      std::cerr <<"|R| = "<<aaf.atts.size()<<std::endl;
    }


    AttackRelation ar (aaf);

  //  clock.stop("parsing");

    Solver *solver = nullptr;
    Heuristic *heuristic = nullptr;

    if (semantics == "GR" || semantics == "CO" && (task == "DS" || task == "SE")) {
      /**
       * no heuristics necessary
       */
    } else {
      if (param_heuristic.size()) {
        std::stringstream source(param_heuristic);
        heuristic = parseHeuristic(aaf, ar, source);
      } else {
        /**
         * standard heurstic (0.25*A)^5
         */
        if (semantics == "ST") {
          std::stringstream strstr ("+ * scc 1000 + / + outdeg 1 + indeg 1 /  +  outpath inpath 1000");
          heuristic = parseHeuristic (aaf, ar, strstr);
        } else {
          std::stringstream strstr ("+ * scc 1000 * -1 * outpath inpath");
          heuristic = parseHeuristic (aaf, ar, strstr);
        }
      }
      if (! heuristic) {
        std::cerr << "Fail: heuristic could not be parsed"<< std::endl;
        return 0;
      }
      if (param_debug_options == "2") {
        std::cerr << *heuristic << std::endl;
        std::cerr << '[' << heuristic->get_min_val() <<", "<<heuristic->get_max_val()<<']'<<std::endl;
      }
      heuristic->sort();
    }

    if (semantics == "GR" || semantics == "CO" && (task == "DS" || task == "SE")){
      solver = new GroundedSolver {};
    } else if (semantics == "ST") {
      solver = new StableSolver {*heuristic};
    } else if (semantics == "CO") {
      solver = new CompleteSolver {*heuristic};
    } else if (semantics == "PR") {
      solver = new PreferredSolver {*heuristic};
    } else {
        std::cerr << "Fail: Semantics " << semantics << " not supported" << std::endl;
        return 0;
    }

  //  clock.stop("heuristic");

    if (task == "EE") {
      std::vector<std::vector<int>> results = solver -> enum_exts(ar);

      std::cout<<'[';
      bool first = true;
      for (auto &ext: results){
        if (first)
          first = false;
        else
          std::cout << ',';
        aaf.print(ext, std::cout);
      }
      std::cout<<']'<<std::endl;
    } else if (task == "SE") {
      auto exts = solver -> enum_exts(ar, 1);
      if (exts.size()) {
        aaf.print(exts[0], std::cout);
      } else
        std::cout << "NO";
      std::cout << std::endl;
    } else if (task == "DS" || task == "DC") {
      if (! item.size()) {
        std::cerr << "Fail: Params missing" << std::endl;
        return 0;
      }
      arg_t arg;
      bool found = false;
      for (auto a : aaf.args) {
        if (a.label == item) {
          arg = a.id;
          found = true;
          break;
        }
      }
      if (! found) {
        std::cerr << "Fail: Argument \""<<item<<"\" not found" << std::endl;
        return 0;
      }
      bool justified =  solver -> justify (ar, arg, task == "DS");
      std::cout << (justified ? "YES" : "NO") << std::endl;
    } else {
        std::cerr << "Fail: Unknown task " << task << std::endl;
        return 0;
    }

    if (param_debug_options.size())
      clock.stop("algorithm");

    delete solver;
    delete heuristic;

  }
  return 0;
}
