#ifndef SPHLATCH_EOS_IDEALGAS
#define SPHLATCH_EOS_IDEALGAS

/*
 *  eos_idealgas.h
 *
 *
 *  Created by Andreas Reufer on 26.07.08.
 *  Copyright 2008 University of Berne. All rights reserved.
 *
 */

#include "typedefs.h"
#include "eos_generic.h"

namespace sphlatch {

class IdealGas : public EOS<IdealGas> {

public:
void init()
{
  gamma = PartManager.attributes["gamma"];
  gammamone = gamma - 1.;
  uToT = gammamone*PartManager.attributes["KperU"];
};

valueType getPressure(const size_t& _i)
{
  return gammamone * ( u(_i) * rho(_i) );
};

valueType getSpeedOfSound(const size_t& _i)
{
  return sqrt( p(_i)*gamma / rho(_i) );
};

valueType getTemperature(const size_t& _i)
{
  return uToT*u(_i);
};

private:
valueType gamma, gammamone, uToT;

};
}
#endif
