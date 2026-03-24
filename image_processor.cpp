#include "bmp/bmp.h"
#include "cli/parser.h"

#include <exception>
#include <iostream>

int main(int argc, char** argv) {
  try {
    ProgramOptions options = ArgumentParser::Parse(argc, argv);
    if (options.show_help) {
      std::cout << ArgumentParser::BuildHelp();
      return 0;
    }

    const auto filters = ArgumentParser::BuildFilters(options.filters);
    Image image = bmp::BmpReader::Read(options.input_path);
    for (const auto& filter : filters) {
      filter->Apply(image);
    }
    bmp::BmpWriter::Write(options.output_path, image);
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << '\n';
    return 1;
  }
}
