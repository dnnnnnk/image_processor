#pragma once

#include "filters/base.h"

#include <memory>
#include <string>
#include <vector>

struct ParsedFilter {
  std::string name;
  std::vector<std::string> params;
};

struct ProgramOptions {
  std::string input_path;
  std::string output_path;
  std::vector<ParsedFilter> filters;
  bool show_help = false;
};

class ArgumentParser {
 public:
  static ProgramOptions Parse(int argc, char* argv[]);
  static std::vector<std::unique_ptr<Filter>> BuildFilters(const std::vector<ParsedFilter>& parsed_filters);
  static std::string BuildHelp();

 private:
  static std::string NextToken(int argc, char* argv[], int& position);
  static bool IsBmpPath(const std::string& path);
};
