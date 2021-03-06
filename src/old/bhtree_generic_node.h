#ifndef BHTREE_GENERIC_NODE_H
#define BHTREE_GENERIC_NODE_H

/*
 *  bhtree_generic_node.h
 *
 *
 *  Created by Andreas Reufer on 06.02.08.
 *  Copyright 2008 University of Berne. All rights reserved.
 *
 */

#include "typedefs.h"

namespace sphlatch {
/// \brief tree node struct with an
///        arbitrary payload
///

struct genericNode {
  ///
  /// pointer to parent
  ///
  genericNode* parent;

  ///
  /// the root node has depth = 0, its
  /// children have depth = 1 and so on
  ///
  size_t depth;

  ///
  /// the cell locally unique identifier:
  /// - for particles the rowIndex
  /// - for cells a negative number
  ///
  identType ident;

  ///
  /// isParticle:   is a particle
  ///
  bool isParticle : 1;

  ///
  /// isEmpty:	    a cell node whose subtrees do not contain particles
  ///               on any costzone domain.
  ///
  bool isEmpty    : 1;

  ///
  /// isLocal:       determines whether a particle is local or non-
  ///                local (ghost).
  ///
  bool isLocal    : 1;

  ///
  /// pointer operator
  ///
  genericNode*  operator*() {
    return this;
  }
};
};

#endif

