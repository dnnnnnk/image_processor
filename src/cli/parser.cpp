#include "cli/parser.h"

#include "filters/crop.h"
#include "filters/edge_detection.h"
#include "filters/gaussian_blur.h"
#include "filters/grayscale.h"
#include "filters/negative.h"
#include "filters/pixelate.h"
#include "filters/sharpening.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace {

using FilterFactory = std::function<std::unique_ptr<Filter>(const std::vector<std::string>&)>;

const std::unordered_map<std::string, size_t> kFilterParamCounts = {
    {"crop", 2},
    {"gs", 0},
    {"neg", 0},
    {"sharp", 0},
    {"edge", 1},
    {"blur", 1},
    {"pixelate", 1},
};

void EnsureParamCount(const std::string& filter_name, size_t expected, size_t actual) {
  if (expected != actual) {
    throw std::invalid_argument("Filter -" + filter_name + " expects " + std::to_string(expected) +
                                " parameter(s), got " + std::to_string(actual));
  }
}

int ParsePositiveInt(const std::string& token, const std::string& parameter_name) {
  size_t parsed = 0;
  int value = 0;

  try {
    value = std::stoi(token, &parsed);
  } catch (const std::exception&) {
    throw std::invalid_argument("Invalid integer for " + parameter_name + ": " + token);
  }

  if (parsed != token.size() || value <= 0) {
    throw std::invalid_argument("Invalid positive integer for " + parameter_name + ": " + token);
  }

  return value;
}

double ParseFiniteDouble(const std::string& token, const std::string& parameter_name) {
  size_t parsed = 0;
  double value = 0.0;

  try {
    value = std::stod(token, &parsed);
  } catch (const std::exception&) {
    throw std::invalid_argument("Invalid number for " + parameter_name + ": " + token);
  }

  if (parsed != token.size() || !std::isfinite(value)) {
    throw std::invalid_argument("Invalid finite number for " + parameter_name + ": " + token);
  }

  return value;
}

const std::unordered_map<std::string, FilterFactory>& GetFilterFactories() {
  static const std::unordered_map<std::string, FilterFactory> FACTORIES = {
      {"crop",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("crop", 2, params.size());
         const int width = ParsePositiveInt(params[0], "crop width");
         const int height = ParsePositiveInt(params[1], "crop height");
         return std::make_unique<CropFilter>(width, height);
       }},
      {"gs",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("gs", 0, params.size());
         return std::make_unique<GrayscaleFilter>();
       }},
      {"neg",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("neg", 0, params.size());
         return std::make_unique<NegativeFilter>();
       }},
      {"sharp",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("sharp", 0, params.size());
         return std::make_unique<SharpeningFilter>();
       }},
      {"edge",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("edge", 1, params.size());
         const double threshold = ParseFiniteDouble(params[0], "edge threshold");
         return std::make_unique<EdgeDetectionFilter>(threshold);
       }},
      {"blur",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("blur", 1, params.size());
         const double sigma = ParseFiniteDouble(params[0], "blur sigma");
         return std::make_unique<GaussianBlurFilter>(sigma);
       }},
      {"pixelate",
       [](const std::vector<std::string>& params) {
         EnsureParamCount("pixelate", 1, params.size());
         const int block_size = ParsePositiveInt(params[0], "pixelate block size");
         return std::make_unique<PixelateFilter>(block_size);
       }},
  };
  return FACTORIES;
}

}

ProgramOptions ArgumentParser::Parse(int argc, char* argv[]) {
  ProgramOptions options;

  if (argc == 1) {
    options.show_help = true;
    return options;
  }

  if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
    options.show_help = true;
    return options;
  }

  if (argc < 3) {
    throw std::invalid_argument("Expected input and output file paths");
  }

  options.input_path = argv[1];
  options.output_path = argv[2];

  if (!IsBmpPath(options.input_path) || !IsBmpPath(options.output_path)) {
    throw std::invalid_argument("Input and output files must have .bmp extension");
  }

  int position = 3;
  while (position < argc) {
    const std::string token = argv[position];
    if (token.empty() || token[0] != '-') {
      throw std::invalid_argument("Expected filter name, got: " + token);
    }

    const std::string filter_name = token.substr(1);
    ++position;

    const auto it = kFilterParamCounts.find(filter_name);
    if (it == kFilterParamCounts.end()) {
      throw std::invalid_argument("Unknown filter: -" + filter_name);
    }

    ParsedFilter filter;
    filter.name = filter_name;
    filter.params.reserve(it->second);

    for (size_t i = 0; i < it->second; ++i) {
      filter.params.push_back(NextToken(argc, argv, position));
    }

    options.filters.push_back(std::move(filter));
  }

  return options;
}

std::vector<std::unique_ptr<Filter>> ArgumentParser::BuildFilters(
    const std::vector<ParsedFilter>& parsed_filters) {
  std::vector<std::unique_ptr<Filter>> filters;
  filters.reserve(parsed_filters.size());

  const auto& factories = GetFilterFactories();
  for (const ParsedFilter& parsed_filter : parsed_filters) {
    const auto it = factories.find(parsed_filter.name);
    if (it == factories.end()) {
      throw std::invalid_argument("Unknown filter: -" + parsed_filter.name);
    }
    filters.push_back(it->second(parsed_filter.params));
  }

  return filters;
}

std::string ArgumentParser::BuildHelp() {
  return "Usage:\n"
         "  image_processor <input.bmp> <output.bmp> [filters]\n"
         "  image_processor --help\n\n"
         "Filters:\n"
         "  -crop <width> <height>\n"
         "  -gs\n"
         "  -neg\n"
         "  -sharp\n"
         "  -edge <threshold>\n"
         "  -blur <sigma>\n"
         "  -pixelate <block_size>\n\n"
         "Both input and output files must use .bmp extension.\n";
}

std::string ArgumentParser::NextToken(int argc, char* argv[], int& position) {
  if (position >= argc) {
    throw std::invalid_argument("Missing filter parameter");
  }

  const std::string token = argv[position];
  ++position;
  return token;
}

bool ArgumentParser::IsBmpPath(const std::string& path) {
  if (path.size() < 4) {
    return false;
  }

  std::string lower_path = path;
  std::transform(lower_path.begin(), lower_path.end(), lower_path.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
  return lower_path.rfind(".bmp") == lower_path.size() - 4;
}
