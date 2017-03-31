#include "collector.h"

void ExtensionEnumerator::report_ext_labelling (const labelling_t &labels) {
  std::vector<int> ext;
  for (int i = 0; i<labels.size(); i++) {
    if(labels[i] == labels::IN)
      ext.push_back(i);
  }
  exts.push_back(ext);
  if (max_cnt && exts.size() >= max_cnt)
    stop();
}

void ExtensionEnumerator::report_ext_labelling (const labelling_t &labels, const std::vector<int> &ext) {
  exts.push_back(ext);
  if (max_cnt && exts.size() >= max_cnt)
    stop();
}

void ArgumentJustifier::report_ext_labelling (const labelling_t &labels) {
  if (!is_stopped()) {
    if (sceptical) {
      if (labels[arg] != labels::IN) {
        justified = false;
        stop();
      }
    } else {
      if (labels[arg] == labels::IN) {
        justified = true;
        stop();
      }
    }
  }
}
