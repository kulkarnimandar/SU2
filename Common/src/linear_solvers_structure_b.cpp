/*!
 * \file linear_solvers_structure_b.cpp
 * \brief Routines for the linear solver used in the reverse sweep of AD.
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

#include "../include/linear_solvers_structure_b.hpp"
#include "../include/linear_solvers_structure.hpp"
#include "../include/vector_structure.hpp"
#include "../include/matrix_structure.hpp"

#ifdef CODI_REVERSE_TYPE
void CSysSolve_b::Solve_b(AD::CheckpointHandler* data){
  /*--- Extract data from the checkpoint handler ---*/

  int *LinSysRes_Indices;
  int *LinSysSol_Indices;

  data->getData(LinSysRes_Indices);
  data->getData(LinSysSol_Indices);

  unsigned long nBlk, nVar, nBlkDomain, size, i;

  data->getData(size);
  data->getData(nBlk);
  data->getData(nVar);
  data->getData(nBlkDomain);

  CSysMatrix* Jacobian;
  data->getData(Jacobian);

  CGeometry* geometry;
  data->getData(geometry);

  CConfig* config;
  data->getData(config);

  CSysVector LinSysRes_b(nBlk, nBlkDomain, nVar, 0.0);
  CSysVector LinSysSol_b(nBlk, nBlkDomain, nVar, 0.0);
  su2double Residual;

  unsigned long MaxIter = config->GetLinear_Solver_Iter();
  su2double SolverTol = config->GetLinear_Solver_Error();

  /*--- Initialize the right-hand side with the gradient of the solution of the primal linear system ---*/

  for (i = 0; i < size; i ++){
    int& index = LinSysSol_Indices[i];
    LinSysRes_b[i] = AD::globalTape.getGradient(index);
    LinSysSol_b[i] = 0.0;
    AD::globalTape.gradient(index) = 0.0;
  }
  /*--- Set up preconditioner and matrix-vector product ---*/

  CPreconditioner* precond  = NULL;

  switch(config->GetKind_DiscAdj_Linear_Prec()){
    case ILU:
      precond = new CILUPreconditioner(*Jacobian, geometry, config);
      break;
    case JACOBI:
      precond = new CJacobiPreconditioner(*Jacobian, geometry, config);
      break;
  }

  CMatrixVectorProduct* mat_vec = new CSysMatrixVectorProductTransposed(*Jacobian, geometry, config);

  CSysSolve *solver = new CSysSolve;

  /*--- Solve the system ---*/

  switch(config->GetKind_DiscAdj_Linear_Solver()){
    case FGMRES:
      solver->FGMRES_LinSolver(LinSysRes_b, LinSysSol_b, *mat_vec, *precond, SolverTol , MaxIter, &Residual, false);
      break;
    case BCGSTAB:
      solver->BCGSTAB_LinSolver(LinSysRes_b, LinSysSol_b, *mat_vec, *precond, SolverTol , MaxIter, &Residual, false);
      break;
  }


  /*--- Update the gradients of the right-hand side of the primal linear system ---*/

  for (i = 0; i < size; i ++){
    int& index = LinSysRes_Indices[i];
    AD::globalTape.gradient(index) += SU2_TYPE::GetValue(LinSysSol_b[i]);
  }

  delete mat_vec;
  delete precond;
  delete solver;
}


void CSysSolve_b::Delete_b(AD::CheckpointHandler* data){

  int *LinSysRes_Indices;
  int *LinSysSol_Indices;

  data->getData(LinSysRes_Indices);
  data->getData(LinSysSol_Indices);

  delete [] LinSysRes_Indices;
  delete [] LinSysSol_Indices;

  unsigned long nBlk, nVar, nBlkDomain, size;

  data->getData(size);
  data->getData(nBlk);
  data->getData(nVar);
  data->getData(nBlkDomain);

  CSysMatrix* Jacobian;
  data->getData(Jacobian);

  CGeometry* geometry;
  data->getData(geometry);

  CConfig* config;
  data->getData(config);
}
#endif
