/*!
 * \file variable_direct_elasticity.cpp
 * \brief Definition of the variables for FEM elastic structural problems.
 * \author R. Sanchez
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

#include "../include/variable_structure.hpp"

CFEM_ElasVariable::CFEM_ElasVariable(void) : CVariable() {

	dynamic_analysis 		= false;
	fsi_analysis 			= false;

	VonMises_Stress 		= 0.0;

	Stress 					= NULL;		// Nodal stress (for output purposes)
	FlowTraction 			= NULL;		// Nodal traction due to the fluid (fsi)
//	Residual_Int 			= NULL;		// Internal component of the residual
	Residual_Ext_Surf 		= NULL;		// Residual component due to external surface forces
	Residual_Ext_Body 		= NULL;		// Residual component due to body forces

	FlowTraction_n			= NULL;		// Nodal traction due to the fluid (fsi) at time n (for gen-alpha methods)
	Residual_Ext_Surf_n		= NULL;		// Residual component due to external surface forces at time n (for gen-alpha methods)

	Solution_time_n			= NULL;		// Solution at the node at the previous subiteration

	Solution_Vel			= NULL;		// Velocity at the node at time t+dt
	Solution_Vel_time_n 	= NULL;		// Velocity at the node at time t

	Solution_Accel			= NULL;		// Acceleration at the node at time t+dt
	Solution_Accel_time_n 	= NULL;		// Acceleration at the node at time t

	Solution_Pred			= NULL;		// Predictor of the solution at the current subiteration
	Solution_Pred_Old		= NULL;		// Predictor of the solution at the previous subiteration

}

CFEM_ElasVariable::CFEM_ElasVariable(su2double *val_fea, unsigned short val_nDim, unsigned short val_nvar, CConfig *config) : CVariable(val_nDim, val_nvar, config) {

	unsigned short iVar;
	bool nonlinear_analysis = (config->GetGeometricConditions() == LARGE_DEFORMATIONS);	// Nonlinear analysis.
	bool body_forces = config->GetDeadLoad();	// Body forces (dead loads).
	bool incremental_load = config->GetIncrementalLoad();
	bool gen_alpha = (config->GetKind_TimeIntScheme_FEA() == GENERALIZED_ALPHA);	// Generalized alpha method requires residual at previous time step.

	VonMises_Stress = 0.0;

	dynamic_analysis = (config->GetDynamic_Analysis() == DYNAMIC);
	fsi_analysis = config->GetFSI_Simulation();

	if (nDim == 2) Stress = new su2double [3];
	else if (nDim == 3) Stress = new su2double [6];

	/*--- Initialization of variables ---*/
	for (iVar = 0; iVar < nVar; iVar++) {
		Solution[iVar] = val_fea[iVar];
	}

	if (dynamic_analysis){
		Solution_time_n			=  new su2double [nVar];
		Solution_Vel 			=  new su2double [nVar];
		Solution_Vel_time_n		=  new su2double [nVar];
		Solution_Accel 			=  new su2double [nVar];
		Solution_Accel_time_n	=  new su2double [nVar];
		for (iVar = 0; iVar < nVar; iVar++){
			Solution_time_n[iVar] 		= val_fea[iVar];
			Solution_Vel[iVar] 			= val_fea[iVar+nVar];
			Solution_Vel_time_n[iVar] 	= val_fea[iVar+nVar];
			Solution_Accel[iVar] 		= val_fea[iVar+2*nVar];
			Solution_Accel_time_n[iVar] = val_fea[iVar+2*nVar];
		}
	}
	else {
		Solution_time_n			=  NULL;
		Solution_Vel 			=  NULL;
		Solution_Vel_time_n		=  NULL;
		Solution_Accel 			=  NULL;
		Solution_Accel_time_n	=  NULL;
	}

	if (fsi_analysis) {
		FlowTraction 			=  new su2double [nVar];
		Solution_Pred 			=  new su2double [nVar];
		Solution_Pred_Old 		=  new su2double [nVar];
		for (iVar = 0; iVar < nVar; iVar++){
			FlowTraction[iVar] = 0.0;
			Solution_Pred[iVar] = val_fea[iVar];
			Solution_Pred_Old[iVar] = val_fea[iVar];
		}
	}
	else {
		FlowTraction 			=  NULL;
		Solution_Pred 			=  NULL;
		Solution_Pred_Old 		=  NULL;
	}

	/*--- If we are going to use incremental analysis, we need a way to store the old solution ---*/
	if (incremental_load && nonlinear_analysis){
		Solution_Old 			=  new su2double [nVar];
	}

	/*--- If we are going to use a generalized alpha integration method, we need a way to store the old residuals ---*/
	if (gen_alpha){
		Residual_Ext_Surf_n		= new su2double [nVar];

		if (fsi_analysis) FlowTraction_n = new su2double [nVar];
		else FlowTraction_n = NULL;

	}

//	if (nonlinear_analysis) Residual_Int = new su2double [nVar];	else Residual_Int = NULL;
	if (body_forces) Residual_Ext_Body = new su2double [nVar];	else Residual_Ext_Body = NULL;
	Residual_Ext_Surf = new su2double [nVar];

	for (iVar = 0; iVar < nVar; iVar++){
		Residual_Ext_Surf[iVar] = 0.0;
		if (body_forces) Residual_Ext_Body[iVar] = 0.0;
	}

}

CFEM_ElasVariable::~CFEM_ElasVariable(void) {

	if (Stress 					!= NULL) delete [] Stress;
	if (FlowTraction 			!= NULL) delete [] FlowTraction;
//	if (Residual_Int 			!= NULL) delete [] Residual_Int;
	if (Residual_Ext_Surf 		!= NULL) delete [] Residual_Ext_Surf;
	if (Residual_Ext_Body 		!= NULL) delete [] Residual_Ext_Body;

	if (FlowTraction_n 			!= NULL) delete [] FlowTraction_n;
	if (Residual_Ext_Surf_n		!= NULL) delete [] Residual_Ext_Surf_n;

	if (Solution_time_n 		!= NULL) delete [] Solution_time_n;

	if (Solution_Vel 			!= NULL) delete [] Solution_Vel;
	if (Solution_Vel_time_n 	!= NULL) delete [] Solution_Vel_time_n;

	if (Solution_Accel 			!= NULL) delete [] Solution_Accel;
	if (Solution_Accel_time_n 	!= NULL) delete [] Solution_Accel_time_n;

	if (Solution_Pred 			!= NULL) delete [] Solution_Pred;
	if (Solution_Pred_Old 		!= NULL) delete [] Solution_Pred_Old;

}
