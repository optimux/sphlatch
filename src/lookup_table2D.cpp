#ifndef SPHLATCH_LOOKUP_TABLE2D
#define SPHLATCH_LOOKUP_TABLE2D

/*
 *  lookup_table2D.h
 *
 *  this class generates a 2D look-up table usable like a mathe-
 *  matical function from a tables f(x,y), x and y
 *
 *  x and y need to be monotonous ascending
 *
 *  Created by Andreas Reufer on 13.08.08.
 *  Copyright 2008 University of Berne. All rights reserved.
 *
 */

#include "typedefs.h"
#include "err_handler.cpp"
namespace sphlatch {
template<class T_leaftype>
class LookupTable2D {
public:
LookupTable2D(valvectType _x, valvectType _y, matrixType _f)
{
  nx = _x.size();
  ny = _y.size();

  assert(_f.size1() == nx);
  assert(_f.size2() == ny);

  f.resize(nx, ny);
  f = _f;

  x.resize(nx);
  x = _x;
  xMin = x(0);
  xMax = x(nx-1);

  y.resize(ny);
  y = _y;
  yMin = y(0);
  yMax = y(ny-1);
};

~LookupTable2D()
{
};

public:
fType operator()(const fType _x, const fType _y);

fType operator()(const fType _x, const fType _y,
                     const size_t _ixl, const size_t _iyl);

private:
T_leaftype& asLeaf()
{
  return static_cast<T_leaftype&>(*this);
};

public:
size_t ixl, ixh, iyl, iyh;

protected:
valvectType x, y;
fType xMin, xMax, yMin, yMax;
matrixType f;
size_t nx, ny;

private:
class OutsideRange : public GenericError
{
  public:
OutsideRange(const fType _x,
             const fType _y,
             const fType xMin,
             const fType xMax,
             const fType yMin,
             const fType yMax)
{
#ifdef SPHLATCH_LOGGER
  Logger.stream
#else
  std::cerr
#endif
    << "Lookup table 2D: argument ["
    << _x << "," << _y << "] out of range ["
    << xMin << "..." << xMax << ","
    << yMin << "..." << yMax << "]";
#ifdef SPHLATCH_LOGGER
  Logger.flushStream();
  Logger.destroy();
#else
  std::cerr << "\n";
#endif
}
~OutsideRange() {};
};
};

template<class T_leaftype>
fType LookupTable2D<T_leaftype>::operator()(const fType _x,
                                                const fType _y)
{
  ///
  /// throw an exception outside the
  /// interpolation range
  ///
  if (_x < xMin || _x > xMax ||
      _y < yMin || _y > yMax )
     throw OutsideRange( _x, _y, xMin, xMax, yMin, yMax );

  ///
  /// bracket the _x value
  ///
  ixl = 0;
  ixh = nx - 1;
  while ((ixh - ixl) > 1)
    {
      const size_t ixm = (ixh + ixl) / 2;

      if (x(ixm) < _x)
        ixl = ixm;
      else
        ixh = ixm;
    }

  ///
  /// bracket the _y value
  ///
  iyl = 0;
  iyh = ny - 1;
  while ((iyh - iyl) > 1)
    {
      const size_t iym = (iyh + iyl) / 2;

      if (y(iym) < _y)
        iyl = iym;
      else
        iyh = iym;
    }

  ///
  /// interpolate
  ///
  return(asLeaf().interpolate(_x, _y, ixl, iyl));
};


///
/// directly return interpolation in case
/// the indices are already known
///
/// no check is performed, whether the arguments
/// are still inside the range
///
template<class T_leaftype>
fType LookupTable2D<T_leaftype>::operator()(
  const fType _x, const fType _y,
  const size_t _ixl, const size_t _iyl)
{
  return(asLeaf().interpolate(_x, _y, _ixl, _iyl));
}

///
/// this class provides bilinear interpolation
/// for the look-up table
///
class InterpolateBilinear : public LookupTable2D<InterpolateBilinear> {
public:
fType interpolate(const fType _x, const fType _y,
                      const size_t _ixl, const size_t _iyl)
{
  const fType xl = x(_ixl);
  const fType xh = x(_ixl + 1);

  const fType yl = y(_iyl);
  const fType yh = y(_iyl + 1);

  const fType t = (_x - xl) / (xh - xl);
  const fType u = (_y - yl) / (yh - yl);

  return(f(_ixl, _iyl) * (1. - t) * (1. - u) +
         f(_ixl + 1, _iyl) * t * (1. - u) +
         f(_ixl, _iyl + 1) * (1. - t) * u +
         f(_ixl + 1, _iyl + 1) * t * u);
};
};
}
#endif
