/*!
 * \file datatype_structure.hpp
 * \brief Headers for generalized datatypes.
 *        The subroutines and functions are in the <i>datatype_structure.cpp</i> file.
 * \author T. Albring
 * \version 4.1.1 "Cardinal"
 *
 * SU2 Lead Developers: Dr. Francisco Palacios (Francisco.D.Palacios@boeing.com).
 *                      Dr. Thomas D. Economon (economon@stanford.edu).
 *
 * SU2 Developers: Prof. Juan J. Alonso's group at Stanford University.
 *                 Prof. Piero Colonna's group at Delft University of Technology.
 *                 Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *                 Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *                 Prof. Rafael Palacios' group at Imperial College London.
 *
 * Copyright (C) 2012-2016 SU2, the open-source CFD code.
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <iostream>
#include <cstdio>

/*--- Depending on the datatype defined during the configuration, include the correct datatype
 * definition. Each file uses a typedef from the specific datatype to su2double and implements
 * the routines defined in the namespace SU2_TYPE below. ---*/

#if defined COMPLEX_TYPE
#include "datatypes/complex_structure.hpp"
#define SPRINTF sprintfOver
#elif defined CODI_REVERSE_TYPE
#define SPRINTF sprintfOver
#include "datatypes/codi_reverse_structure.hpp"
#elif defined CODI_FORWARD_TYPE
#define SPRINTF sprintfOver
#include "datatypes/codi_forward_structure.hpp"
#else
#define SPRINTF sprintf
#include "datatypes/primitive_structure.hpp"
#endif

#include "ad_structure.hpp"

/*--- This type can be used for (rare) compatiblity cases or for computations that are intended to be (always) passive. ---*/

typedef double passivedouble;

/*!
 * \namespace SU2_TYPE
 * \brief Namespace for defining the datatype wrapper routines; this class features as a base class for
 * type interfaces for non-primitive dataypes e.g. used by AD, complex etc.
 * \author T. Albring
 * \version 4.1.1 "Cardinal"
 */
namespace SU2_TYPE{
  /*!
   * \brief Set the (primitive) value of the datatype (needs to be implemented for each new type).
   * \param[in] data - The non-primitive datatype.
   * \param[in] val - The primitive value.
   */
  void SetValue(su2double& data, const double &val);

  /*!
   * \brief Set the secondary value of the datatype (needs to be implemented for each new type).
   * \param[in] data - The non-primitive datatype.
   * \param[in] val - The primitive value.
   */
  void SetSecondary(su2double& data, const double &val);

  /*!
   * \brief Get the (primitive) value of the datatype (needs to be implemented for each new type).
   * \param[in] data - The non-primitive datatype.
   * \return The primitive value.
   */
  double GetValue(const su2double &data);

  /*!
   * \brief Get the secondary value of the datatype (needs to be implemented for each new type).
   * \param[in] data - The non-primitive datatype.
   * \return The primitive value.
   */
  double GetSecondary(const su2double &data);

  /*!
   * \brief Get the derivative value of the datatype (needs to be implemented for each new type).
   * \param[in] data - The non-primitive datatype.
   * \return The derivative value.
   */
  double GetDerivative(const su2double &data);

  /*!
   * \brief Set the derivative value of the datatype (needs to be implemented for each new type).
   * \param[in] data - The non-primitive datatype.
   * \param[in] val - The value of the derivative.
   */
  void SetDerivative(su2double &data, const double &val);

  /*!
   * \brief Casts the primitive value to int (uses GetValue, already implemented for each type).
   * \param[in] data - The non-primitive datatype.
   * \return - The primary value casted to int.
   */
  int Int(const su2double& data);

  /*!
   * \brief Casts the primitive value to short (uses GetValue, already implemented for each type).
   * \param[in] data - The non-primitive datatype.
   * \return - The primary value casted to short.
   */
  short Short(const su2double& data);
}

#include "datatype_structure.inl"


