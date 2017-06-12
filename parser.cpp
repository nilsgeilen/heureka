#include <unordered_map>
#include <iostream>

#include "parser.h"

void parseTGF(AAF&aaf, std::istream &file) {
  std::unordered_map<std::string, Argument> argids;

  int id = -1;
  while ( ! file.eof()) {
    std::string line;
    file >> line;
      if(line.find("#") != std::string::npos) {
        break;
      } else {
        Argument arg = {++id, line};
        argids[line] = arg;
        aaf.args.push_back(arg);
      }
  }

  while ( ! file.eof()) {
      std::string a,b;
      file >> a >> b;
      if(a==""||b=="")
        break;
      Attack att = {argids[a], argids[b]};
      aaf.atts.push_back(att);
  }

}

void parseAPX(AAF&aaf, std::istream &file) {
  std::unordered_map<std::string, Argument> argids;

  int id = -1;
  while ( ! file.eof()) {
    std::string line;
    std::getline(file, line);
    auto parenpos1 =  line.find("("),
      parenpos2 = line.find(")");
    std::string type = line.substr(0, parenpos1);
    if (type == "arg") {
      std::string label = line.substr(parenpos1+1, parenpos2-parenpos1-1);
      Argument arg = {++id, label};
      argids[label] = arg;
      aaf.args.push_back(arg);
    } else if (type == "att") {
      auto commapos = line.find(",");
      std::string atter = line.substr(parenpos1+1, commapos-parenpos1-1),
         atted = line.substr(commapos+1, parenpos2-commapos-1);;
      Attack att = {argids[atter], argids[atted]};
      aaf.atts.push_back(att);
    }
  }
}
