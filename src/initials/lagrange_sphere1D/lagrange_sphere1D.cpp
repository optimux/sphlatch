// uncomment for single-precision calculation
//#define SPHLATCH_SINGLEPREC

// enable parallel version
//#define SPHLATCH_PARALLEL

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
typedef sphlatch::valueType valueType;
typedef sphlatch::identType identType;
typedef sphlatch::valvectType valvectType;

typedef sphlatch::valvectRefType valvectRefType;
typedef sphlatch::idvectRefType idvectRefType;
typedef sphlatch::matrixRefType matrixRefType;

#include "io_manager.h"
typedef sphlatch::IOManager io_type;

#include "lagrange_sphere1D_solver.h"
typedef sphlatch::LagrangeSphere1DSolver lg1D_solver_type;

using namespace boost::assign;
using namespace sphlatch::vectindices;

int main(int argc, char* argv[])
{
  //MPI::Init(argc, argv);

  po::options_description Options("Global Options");
  Options.add_options()
  ("help,h", "Produces this Help")
  ("output-file,o", po::value<std::string>(), "output file")
  ("input-file,i", po::value<std::string>(), "input  file");

  po::variables_map VMap;
  po::store(po::command_line_parser(argc, argv).options(Options).run(), VMap);
  po::notify(VMap);

  if (VMap.count("help"))
    {
      std::cerr << Options << std::endl;
      return EXIT_FAILURE;
    }

  if (!VMap.count("output-file"))
    {
      std::cerr << Options << std::endl;
      return EXIT_FAILURE;
    }

  io_type&        IOManager(io_type::instance());

  const valueType mTot = 5.3014e27;
  const valueType mCore   = 0.30 * mTot;

  const identType matCore = 5;
  const identType matMantle = 4;

  ///
  /// guess for the density
  ///
  const valueType rhoCore = 6.;
  const valueType rhoMantle = 3.;

  const size_t noCells = 1000;
  const size_t noEdges = noCells + 1;

  const valueType dm = mTot / static_cast<valueType>(noCells);

  const size_t noCoreCells = lrint(noCells * (mCore / mTot));

  lg1D_solver_type Solver(noCells);

  ///
  /// set the shell edges
  ///
  Solver.r(0) = 0.;
  Solver.v(0) = 0.;
  for (size_t i = 1; i < noEdges; i++)
    {
      valueType rhoCur = 0.;
      if (i <= noCoreCells)
        rhoCur = rhoCore;
      else
        rhoCur = rhoMantle;
      Solver.r(i) = pow( pow(Solver.r(i-1),3.) +
                         (3./(4*M_PI))*(dm / rhoCur), 1./3.);
      Solver.v(i) = 0.;
    }

  ///
  /// set the shell centered values
  ///
  for (size_t i = 0; i < noCells; i++)
    {
      Solver.m(i) = dm;

      if (i < noCoreCells)
        {
          Solver.mat(i) = matCore;
        }
      else
        {
          Solver.mat(i) = matMantle;
        }

      Solver.u(i) = 5.e10;
    }
  Solver.m(noCells-1) = 0.;

  ///
  /// set some constants
  ///
  Solver.uMin = 1.e4;
  Solver.friction = 1.e-2;

  ///
  /// integrate for a certain physical time
  ///
  std::cerr << " start 1D Lagrange solver\n";
  Solver.integrateTo(5.e3); /// replace by option
  std::cerr << " ... finished\n";
  
  ///
  /// the last cell contains vacuum, which will assign
  /// far outside zero mass. circumvent this, by setting
  /// the same density and temperature like on the second
  /// to last cell
  ///
  Solver.rho(noCells-1) = Solver.rho(noCells-2);
  Solver.u(noCells-1) = Solver.u(noCells-2);

  ///
  /// calculate the position of the cell centers
  ///
  valvectType rCen;
  rCen.resize(noCells);

  for (size_t i = 0; i < noCells; i++)
    {
      rCen(i) = 0.5 * (Solver.r(i) + Solver.r(i + 1));
    }

  std::string dumpfilename = VMap["output-file"].as<std::string>();
  IOManager.savePrimitive(rCen, "r", dumpfilename);
  IOManager.savePrimitive(Solver.v, "v", dumpfilename);
  IOManager.savePrimitive(Solver.m, "m", dumpfilename);
  IOManager.savePrimitive(Solver.u, "u", dumpfilename);
  IOManager.savePrimitive(Solver.p, "p", dumpfilename);
  IOManager.savePrimitive(Solver.mat, "mat", dumpfilename);
  IOManager.savePrimitive(Solver.rho, "rho", dumpfilename);

  IOManager.saveAttribute(Solver.gravConst, "gravconst", dumpfilename);
  IOManager.saveAttribute(Solver.uMin, "umin", dumpfilename);

  //MPI::Finalize();
  return EXIT_SUCCESS;
}

