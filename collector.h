#include <vector>

namespace {
  /**
   * Defines the structure of labellings
   */
  typedef unsigned char label_t;
  typedef std::vector<label_t> labelling_t;

  namespace labels {
  constexpr label_t
      BLANK = 0x1,
      IN=0x2,
      UNDEC=BLANK,
      OUT=0x4
    ;
  }
}

/**
 * A container which controls an algorithm and collects its results
 */
class ExtensionCollector {
  bool stopped = false;
public:
  /**
   * Indicate that the collector is satisfied
   */
  inline void stop() {stopped=true;}
  /**
   * Indicate whether the container is satisfied
   * @return true iff satisfied
   */
  inline bool is_stopped() const {return stopped;};
  /**
   * Process an extension
   * @param labels an extension labelling
   */
  virtual void report_ext_labelling (const labelling_t &labels) = 0;
  /**
   * Process an extension
   * @param labels an extension labelling L
   * @param ext    the extension IN(L)
   */
  virtual void report_ext_labelling (const labelling_t &labels, const std::vector<int> &ext) {
    report_ext_labelling(labels);
  };
};

/**
 * A collector which enumerates all or a limited number of extension
 */
class ExtensionEnumerator : public ExtensionCollector {
  const int max_cnt;
public:
  std::vector<std::vector<int>> exts;
  /**
   * Constructs a new collector
   * @param max_cnt the number of extensions needed
   */
  ExtensionEnumerator (int max_cnt = 0) : max_cnt(max_cnt) {}
  void report_ext_labelling (const labelling_t &labels) override ;
  void report_ext_labelling (const labelling_t &labels, const std::vector<int> &ext) override ;
};

/**
 * A collector which determines whether an argument is justified
 */
class ArgumentJustifier : public ExtensionCollector {
  const bool sceptical;
  const int arg;
  bool justified;
public:
  /**
   * Creates a new collector
   * @param arg       the argument to be justified
   * @param sceptical the argument has to be sceptically justified iff true and creduously elsewise
   */
  ArgumentJustifier (int arg, bool sceptical)
    : arg(arg), sceptical(sceptical), justified(sceptical) {}
  /**
    * Retruns the result
    * @return true iff the argument is justified
  */
  inline bool is_justified() const {return justified;}
  void report_ext_labelling (const labelling_t &labels) override ;
};
