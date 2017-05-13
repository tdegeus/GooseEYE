
#ifndef IMAGE_BATCH_H
#define IMAGE_BATCH_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include "json/json.hpp"

using json = nlohmann::json;

namespace ImageBatch {

class Files8bit_2D
{

private:

public:

  // variables
  // ---------

  json data;

  // constructors
  // ------------

  Files8bit_2D (const Files8bit_2D &) = default;

  Files8bit_2D& operator=(const Files8bit_2D &) = default;

  Files8bit_2D(){};

  // write to file
  // -------------

  void write ( std::string fname )
  {
    std::ofstream o(fname);
    o << std::setw(2) << data << std::endl;
  }

  // read from file
  // --------------

  void read ( std::string fname )
  {
    data.clear();

    std::ifstream i(fname);
    i >> data;
  }

};

}// namespace ImageBatch

#endif
