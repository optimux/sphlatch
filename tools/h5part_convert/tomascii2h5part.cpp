// some defs

// uncomment for single-precision calculation
#define SPHLATCH_SINGLEPREC

#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <boost/program_options/option.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/std/vector.hpp>

namespace po = boost::program_options;

#include "typedefs.h"
typedef sphlatch::fType fType;

#include "particle_manager.h"
typedef sphlatch::ParticleManager part_type;

#include "io_manager.h"
typedef sphlatch::IOManager io_type;

using namespace boost::assign;

int main(int argc, char* argv[])
{
  po::options_description Options("Global Options");
  Options.add_options()
  ("help,h", "Produces this Help")
  ("input-file,i" , po::value<std::string>(), "input  file")
  ("output-file,o", po::value<std::string>(), "output file")
  ("grav-eps,e" ,   po::value<fType>(),   "gravitational smoothing");

  po::variables_map VMap;
  po::store(po::command_line_parser(argc, argv).options(Options).run(), VMap);
  po::notify(VMap);

  if (VMap.count("help"))
    {
      std::cerr << Options << std::endl;
      return EXIT_FAILURE;
    }

  if (!VMap.count("output-file") &&
      !VMap.count("input-file") &&
      !VMap.count("grav-eps"))
    {
      std::cerr << Options << std::endl;
      return EXIT_FAILURE;
    }
  
  part_type& PartManager(part_type::instance());
  io_type&        IOManager(io_type::instance());

  std::string outputFilename = VMap["output-file"].as<std::string>();
  std::string  inputFilename = VMap["input-file"].as<std::string>();
  fType gravEps          = VMap["grav-eps"].as<fType>();

  std::cout << inputFilename << " -> " << outputFilename << "\n";

  std::fstream fin;
  fin.open(inputFilename.c_str(), std::ios::in);

  if (!fin)
  {
    std::cerr << "Error opening " << inputFilename << "\n";
  }

  size_t noParts = 0;
  fType gravConst = 0;
  std::string readLine;

  // ugly header parsing
  for (size_t i = 0; i < 25; i++)
  {
    fin >> readLine;
    switch (i)
    {
      case 1:
        noParts = boost::lexical_cast<size_t>(readLine);
        break;

      case 5:
        gravConst = boost::lexical_cast<fType>(readLine);
        break;
    }
  }

  PartManager.attributes["gravConst"] = gravConst;
  PartManager.useGravity();
  PartManager.setNoParts(noParts, 0);
  PartManager.resizeAll();

  std::cout << noParts << " particles \n";
  std::cout << "using G = " << gravConst
            << " and gravitational smoothing of "
            << gravEps << "\n";

  sphlatch::idvectRefType id( PartManager.id );
  
  sphlatch::matrixRefType pos( PartManager.pos );
  sphlatch::matrixRefType vel( PartManager.vel );
  
  sphlatch::valvectRefType m( PartManager.m );
  sphlatch::valvectRefType eps( PartManager.eps );
  
  sphlatch::quantsType saveQuants;
  saveQuants.vects += &pos, &vel;
  saveQuants.scalars += &m, &eps;
  saveQuants.ints  += &id;

  PartManager.attributes["time"] = 0.0;
  PartManager.step = 0;
  
  for (size_t i = 0; i < noParts; i++)
  {
    fin >> readLine;
    m(i) = boost::lexical_cast<fType>(readLine);

    for (size_t j = 0; j < 3; j++)
    {
      fin >> readLine;
      pos(i, j) = boost::lexical_cast<fType>(readLine);
    }
    
    for (size_t j = 0; j < 3; j++)
    {
      fin >> readLine;
      vel(i, j) = boost::lexical_cast<fType>(readLine);
    }

    id(i) = i + 1;
    eps(i) = gravEps;
  }

  fin.close();

  //IOManager.setSinglePrecOut();
  //IOManager.setDoublePrecOut();
  IOManager.saveDump( outputFilename, saveQuants );

  return EXIT_SUCCESS;
}


