#ifndef OOSPHERAZER_H
#define OOSPHERAZER_H

#include <cstdlib>
#include <iostream>
#include <boost/mpl/at.hpp>
#include <boost/mpl/for_each.hpp>

#include "manager.h"

namespace oosph
{

namespace mpl = boost::mpl;

/**
 * \brief Helper Structor for Deleting Values
 * 
 * \author Pascal Bauer pbauer@phim.unibe.ch
 */
template <typename SimTrait>
struct Eraze
{

    typedef Manager<SimTrait>  manager_type;
    typedef Manager<SimTrait>& manager_reference;
    typedef Manager<SimTrait>* manager_pointer;

    typedef typename manager_type::mem_manager_type mem_manager_type;
    typedef typename manager_type::mem_manager_reference mem_manager_reference;
    typedef typename manager_type::mem_manager_pointer mem_manager_pointer;
    
    typedef typename SimTrait::vector_type vector_type;
    typedef typename SimTrait::zero_vector zero_vector;
    typedef typename SimTrait::matrix_column matrix_column;

    template <typename  N>
    inline void operator()(N) const
    {        
        matrix_column(Mem.Data, N::value) = zero_vector(Mem.Data.size1() );        
        matrix_column(Mem.GData, N::value) = zero_vector(Mem.GData.size1() );
        
    }
//     static manager_reference Manager;
    static mem_manager_reference Mem;
};

// template <typename SimTrait>
// typename Eraze<SimTrait>::manager_reference Eraze<SimTrait>::Manager(manager_type::Instance());

template <typename SimTrait>
typename Eraze<SimTrait>::mem_manager_reference Eraze<SimTrait>::Mem(mem_manager_type::Instance());

/**
 * \brief Class for Selective Deletion of Values in the Particles
 * 
 * \author Pascal Bauer pabuer@phim.unibe.ch
 */

template <typename Indices, typename SimTrait>
class Erazer
{
public:
    typedef Erazer  self_type;
    typedef Erazer& self_reference;
    typedef Erazer* self_pointer;

    typedef Eraze<SimTrait> eraze;

    static self_reference Instance(void);

    void operator()(void);

protected:

    Erazer(void);
    ~Erazer(void);

private:

    static self_pointer _instance;
};

template <typename Indices, typename SimTrait>
typename Erazer<Indices, SimTrait>::self_pointer Erazer<Indices, SimTrait>::_instance = NULL;

template <typename Indices, typename SimTrait>
typename Erazer<Indices, SimTrait>::self_reference Erazer<Indices, SimTrait>::Instance(void)

{
    if (_instance != NULL)
        return *_instance;
    else
    {
        _instance = new self_type;
        return *_instance;
    }
}

template <typename Indices, typename SimTrait>
Erazer<Indices, SimTrait>::Erazer(void)
{
#ifdef XML
    std::cout << "<Erazer>" << std::endl;
    std::cout << this << std::endl;
#endif
};

template <typename Indices, typename SimTrait>
Erazer<Indices, SimTrait>::~Erazer(void)
{
#ifdef XML
    std::cout << "</Erazer>" << std::endl;
#endif
};

template <typename Indices, typename SimTrait>
void Erazer<Indices, SimTrait>::operator()(void)
{
    mpl::for_each<Indices>(eraze());
};

}

#endif
