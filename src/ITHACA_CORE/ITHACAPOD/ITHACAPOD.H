/*---------------------------------------------------------------------------*\
     ██╗████████╗██╗  ██╗ █████╗  ██████╗ █████╗       ███████╗██╗   ██╗
     ██║╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗      ██╔════╝██║   ██║
     ██║   ██║   ███████║███████║██║     ███████║█████╗█████╗  ██║   ██║
     ██║   ██║   ██╔══██║██╔══██║██║     ██╔══██║╚════╝██╔══╝  ╚██╗ ██╔╝
     ██║   ██║   ██║  ██║██║  ██║╚██████╗██║  ██║      ██║      ╚████╔╝
     ╚═╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝  ╚═╝      ╚═╝       ╚═══╝

 * In real Time Highly Advanced Computational Applications for Finite Volumes
 * Copyright (C) 2017 by the ITHACA-FV authors
-------------------------------------------------------------------------------
License
    This file is part of ITHACA-FV
    ITHACA-FV is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    ITHACA-FV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.
    You should have received a copy of the GNU Lesser General Public License
    along with ITHACA-FV. If not, see <http://www.gnu.org/licenses/>.
Class
    ITHACAPOD
Description
    Implementation of a POD decomposition of a general field
SourceFiles
    ITHACAPOD.C
\*---------------------------------------------------------------------------*/

/// \file
/// Header file of the ITHACAPOD class.
/// \dir
/// Directory containing the header and source files for the ITHACAPOD class.

#ifndef ITHACAPOD_H
#define ITHACAPOD_H

#include "fvCFD.H"
#include "ITHACAutilities.H"
#include "ITHACAstream.H"
#include "ITHACAparameters.H"
#include "Foam2Eigen.H"
#include "EigenFunctions.H"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#include <Spectra/GenEigsSolver.h>
#include <Spectra/SymEigsSolver.h>
#include <Eigen/Eigen>
#include <unsupported/Eigen/SparseExtra>
#pragma GCC diagnostic pop

/*---------------------------------------------------------------------------*\
                        Class reductionProblem Declaration
\*---------------------------------------------------------------------------*/

/// namespace for the computation of the POD, it exploits bot the method of snapshots and SVD.
namespace ITHACAPOD
{

// Public Members

//------------------------------------------------------------------------------
/// Computes the bases or reads them for a field
///
/// @param[in]  snapshots   List of snapshots.
/// @param[out] modes       A PtrList where modes are stored (it must be passed
///                         empty).
/// @param[in]  fieldName   The field name
/// @param[in]  podex       If 1, the functions read the stored mode. If 0, the
///                         function computes the modes and stores them.
/// @param[in]  supex       If 1, the functions read the stored supremizer mode.
///                         If 0, the function computes and stores them.
/// @param[in]  sup         If 1 it computes the supremizer modes.
/// @param[in]  nmodes      Number of modes to be stored. If 0, the maximum
///                         number of modes will computed.
///
/// @tparam     Type        vector or scalar.
/// @tparam     PatchField  fvPatchField or fvsPatchField.
/// @tparam     GeoMesh     volMesh or surfaceMesh.
///
template<class Type, template<class> class PatchField, class GeoMesh>
void getModes(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & snapshots,
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes,
    word fieldName, bool podex, bool supex = 0, bool sup = 0,
    label nmodes = 0, bool correctBC = true);

//------------------------------------------------------------------------------
/// @brief      Gets the bases for a scalar field using SVD instead of the
///             method of snapshots
///
/// @param[in]  snapshots   List of snapshots.
/// @param[out] modes       A PtrList where modes are stored (it must be passed
///                         empty).
/// @param[in]  fieldName   The file name
/// @param[in]  podex       If 1, the functions read the stored mode. If 0, the
///                         function computes the modes and stores them.
/// @param[in]  supex       If 1, the functions read the stored supremizer mode.
///                         If 0, the function computes and stores them.
/// @param[in]  sup         If 1 it computes the supremizer modes.
/// @param[in]  nmodes      Number of modes to be stored. If 0, the maximum
///                         number of modes will computed.
///
/// @tparam     Type        vector or scalar.
/// @tparam     PatchField  fvPatchField or fvsPatchField.
/// @tparam     GeoMesh     volMesh or surfaceMesh.
///
template<class Type, template<class> class PatchField, class GeoMesh>
void getModesSVD(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> &
    snapshots, PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes,
    word fieldName, bool podex, bool supex = 0, bool sup = 0,
    label nmodes = 0, bool correctBC = true);


//------------------------------------------------------------------------------
/// Nested-POD approach. Computes the nested snapshot matrix and weighted bases
/// for a vector field
///
/// @param[in]  snapshots   List of snapshots.
/// @param      modes       The modes
/// @param[in]  fieldName   The field name
/// @param[in]  Npar        Number of parameters
/// @param[in]  NnestedOut  Number of weighted modes to be stored for the
///                         assembly of the nested matrix
/// @param[out] UModesGlobal  List of snapshots where modes are stored (it must be
///                           passed empty).
///
/// @tparam     Type        vector or scalar.
/// @tparam     PatchField  fvPatchField or fvsPatchField.
/// @tparam     GeoMesh     volMesh or surfaceMesh.
///
template<class Type, template<class> class PatchField, class GeoMesh>
void getNestedSnapshotMatrix(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & snapshots,
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes,
    word fieldName, label Npar,
    label NnestedOut);


//------------------------------------------------------------------------------
/// Computes the weighted bases (using the nested-pod approach) or read them for
/// a vector field
///
/// @param[in]  snapshots   List of snapshots.
/// @param[out] modes       A PtrList where modes are stored (it must be passed
///                         empty).
/// @param[in]  fieldName   The field name
/// @param[in]  podex       If 1, the functions read the stored mode. If 0, the
///                         function computes the modes and stores them.
/// @param[in]  supex       If 1, the functions read the stored supremizer mode.
///                         If 0, the function computes and stores them.
/// @param[in]  sup         If 1, it computes the supremizer modes.
/// @param[in]  nmodes      Number of modes to be stored. If 0, the maximum
///                         number of modes will computed.
///
/// @tparam     Type        vector or scalar.
/// @tparam     PatchField  fvPatchField or fvsPatchField.
/// @tparam     GeoMesh     volMesh or surfaceMesh.
///
template<class Type, template<class> class PatchField, class GeoMesh>
void getWeightedModes(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & snapshots,
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes,
    word fieldName, bool podex, bool supex = 0, bool sup = 0,
    label nmodes = 0, bool correctBC = true);

//------------------------------------------------------------------------------
/// @brief      Performs GrammSchmidt orthonormalization on an Eigen Matrix
///
/// @param[out] Matrix  The matrix
///
void GrammSchmidt(Eigen::MatrixXd& Matrix);

//------------------------------------------------------------------------------
/// Computes the correlation matrix given a vector field snapshot Matrix using
/// different norms depending on the input snapshots
///
/// @param[in]  snapshots   List of snapshots.
///
/// @tparam     Field_type  Type of the input list you are passing, it can be:
///                         PtrList<volVectorField>, PtrList<volScalarField>,
///                         List<Eigen::SparseMatrix<double>>,
///                         List<Eigen::VectorXd>.
///
/// @return     the Eigen::MatrixXd correlation matrix.
///
template<class Field_type>
Eigen::MatrixXd corMatrix(Field_type& snapshots);


//------------------------------------------------------------------------------
/// Exports the basis for an OpenFOAM GeometricField into the ITHACAOutput/POD
/// or ITHACAOutput/supremizer
///
/// @param      snapshots   The snapshots
/// @param[in]  bases       a PtrList of GeometricField where the bases are
///                         stored.
/// @param[in]  fieldName   The field name
/// @param[in]  sup         a boolean variable 1 if you want to export the
///                         supremizer bases (in ITHACAOutput/supremizer) 0
///                         elsewhere (Default is 0).
/// @param[in]  s     a PtrList of GeometricField where the snapshots associated
///                   with the bases are stored.
///
/// @tparam     Type        vector or scalar.
/// @tparam     PatchField  fvPatchField or fvsPatchField.
/// @tparam     GeoMesh     volMesh or surfaceMesh.
///
template<class Type, template<class> class PatchField, class GeoMesh>
void exportBases(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & snapshots,
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& bases,
    word fieldName,
    bool sup = 0);

//------------------------------------------------------------------------------
/// Exports the eigenvalues as a txt file
///
/// @param[in]  Eigenvalues  a scalarField of eigenvalues.
/// @param[in]  name         a fileName to indicate the name of the txt file.
/// @param[in]  sup          If 1, it exports the eigenValues in
///                          "ITHACAOutput/supremizer". If 0, no actions
///                          (Default is 0).
///
void exportEigenvalues(scalarField Eigenvalues, fileName name,
                       bool sup = 0);

//------------------------------------------------------------------------------
/// Export the cumulative eigenvalues as a txt file
///
/// @param[in]  cumEigenvalues  a scalarField of cumEigenvalues.
/// @param[in]  name            a fileName to indicate the name of the txt file.
/// @param[in]  sup             If 1, it exports the cumEigenValues in
///                             "ITHACAOutput/supremizer". If 0, no actions
///                             (Default is 0).
///
void exportcumEigenvalues(scalarField cumEigenvalues,  fileName name,
                          bool sup = 0);

//------------------------------------------------------------------------------
/// @brief      Get the DEIM modes for a generic non linear function
///
/// @param[in]  SnapShotsMatrix  The snapshots matrix
/// @param[in]  nmodes           The number of modes
/// @param[in]  FunctionName     The function name
///
/// @tparam     Type             templated object for the snapshots matrix type
///
/// @return     Type             the POD modes
///
template<class Type, template<class> class PatchField, class GeoMesh>
PtrList<GeometricField<Type, PatchField, GeoMesh >> DEIMmodes(
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& SnapShotsMatrix,
    label nmodes, word FunctionName, word FieldName);

//------------------------------------------------------------------------------
/// @brief      Get the DEIM modes for a generic non-parametrized matrix coming
///             from a differential operator function
///
/// @param[in]  MatrixList   The matrix list as a list of pointers
/// @param[in]  nmodesA      The number of modes for A
/// @param[in]  nmodesB      The number of modes for B
/// @param[in]  MatrixName   The matrix name, used to save the matrix
///
/// @tparam     type_matrix  The   type of matrix, can be fvScalarMatrix or
///                          fvVectorMatrix
///
/// @return     It returns a tuple containing the list of modes for the matrix
///             and the list of modes for the source term
///
template<typename type_matrix>
std::tuple<List<Eigen::SparseMatrix<double >>, List<Eigen::VectorXd >>
DEIMmodes(PtrList<type_matrix> & MatrixList, label nmodesA, label nmodesB,
          word MatrixName = "Matrix");

//------------------------------------------------------------------------------
/// @brief      Get the DEIM modes for a generic non a parametrized matrix
///             coming from a differential operator function
///
/// @param      A           List of matrices for the lhs
/// @param      b           List of vectors for the rhs
/// @param[in]  nmodesA     The number of modes
/// @param[in]  nmodesB     The nmodes b
/// @param[in]  MatrixName  The matrix name, used to save the matrix
/// @param[in]  MatrixList  The matrix list as a list of pointers
///
/// @tparam     The   type of matrix, can be fvScalarMatrix or fvVectorMatrix
///
/// @return     It returns a tuple containing the list of modes for the matrix
///             and the list of modes for the source term
///
std::tuple<List<Eigen::SparseMatrix<double >>, List<Eigen::VectorXd >>
DEIMmodes(List<Eigen::SparseMatrix<double >> & A, List<Eigen::VectorXd>& b,
          label nmodesA, label nmodesB, word MatrixName = "Matrix");


//------------------------------------------------------------------------------
/// @brief      Gets the modes.
///
/// @param[in]  snapshots   List of snapshots.
/// @param[out] modes       A PtrList where modes are stored (it must be passed
///                         empty).
/// @param      Volumes     The volumes
/// @param[in]  fieldName   The field name
/// @param[in]  podex       If 1, the functions read the stored mode. If 0, the
///                         function computes the modes and stores them.
/// @param[in]  supex       If 1, the functions read the stored supremizer mode.
///                         If 0, the function computes and stores them.
/// @param[in]  sup         If 1 it computes the supremizer modes.
/// @param[in]  nmodes      Number of modes to be stored. If 0, the maximum
///                         number of modes will computed.
///
/// @tparam     Type        vector or scalar.
/// @tparam     PatchField  fvPatchField or fvsPatchField.
/// @tparam     GeoMesh     volMesh or surfaceMesh.
///
template<class Type, template<class> class PatchField, class GeoMesh>
void getModes(PtrList<GeometricField<Type, PatchField, GeoMesh >> & snapshots,
              PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes,
              PtrList<volScalarField>& Volumes, word fieldName, bool podex, bool supex = 0,
              bool sup = 0,
              label nmodes = 0);

//------------------------------------------------------------------------------
/// @brief      Gets Modes based on eigenvalue decomposition of another Field
///
/// @param      snapshots     The snapshots of which we want to create the modes
/// @param      modes         The modes of the snapshots
/// @param      fields2       The fields that we want to use for the eigenvalue
///                           decomposition
/// @param[in]  fieldName     The field name
/// @param[in]  podex         boolean variable 1 if the modes have been already
///                           computed and stored (in this case the function is
///                           reading them) 0 elsewhere.
/// @param[in]  supex         boolean variable 1 if the supremizer modes have
///                           been already computed and stored (in this case the
///                           function is reading them) 0 elsewhere.
/// @param[in]  sup           boolean variable 1 if you want to compute the
///                           supremizer modes 0 elsewhere.
/// @param[in]  nmodes        label variable to set the number of modes to be
///                           stored, if set to 0 the maximum number of modes
///                           will computed.
///
/// @tparam     Field_type    Type of field snapshots
/// @tparam     Field_type_2  Type of the other fields
///
template<class Field_type, class Field_type_2>
void getModes(PtrList<Field_type>& snapshots, PtrList<Field_type>& modes,
              PtrList<Field_type_2>& fields2, word fieldName, bool podex, bool supex = 0,
              bool sup = 0,
              label nmodes = 0);

//------------------------------------------------------------------------------
/// @brief      Gets the modes in a memory-efficient manner
///
/// @param[in]  templateField  The template field
/// @param[in]  snapshotsPath  The path to the snapshots
/// @param[out] modes         The modes
/// @param[in]  fieldName      The field name
/// @param[in]  podex          If 1, the functions read the stored mode. If 0, the
///                            function computes the modes and stores them.
/// @param[in]  supex         If 1, the functions read the stored supremizer mode.
///                            If 0, the function computes and stores them.
/// @param[in]  sup            If 1, it computes the supremizer modes.
/// @param[in]  nmodes         Number of modes to be stored. If 0, the maximum
///                            number of modes will computed.
/// @param[in]  correctBC      If true, correct the boundary conditions.
///
/// @tparam     Type           The type of the field
/// @tparam     PatchField     The patch field type
/// @tparam     GeoMesh        The mesh type
///
template<class Type, template<class> class PatchField, class GeoMesh>
void getModesMemoryEfficient(
    GeometricField<Type, PatchField, GeoMesh>& templateField,
    word snapshotsPath,
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & modes,
    word fieldName,
    bool podex = false,
    bool supex = false,
    bool sup = false,
    label nmodes = 0,
    bool correctBC = true,
    autoPtr<GeometricField<Type, PatchField, GeoMesh >> meanField = NULL);

//------------------------------------------------------------------------------
/// @brief      Gets the mean field in a memory-efficient manner
template<class Type, template<class> class PatchField, class GeoMesh>
void getMeanMemoryEfficient(
    GeometricField<Type, PatchField, GeoMesh>& templateField,
    word snapshotsPath,
    autoPtr<GeometricField<Type, PatchField, GeoMesh >> & meanField,
    bool meanex = false);

//------------------------------------------------------------------------------
/// @brief      Helper function to compute inner product value for different field types
///
/// @param[in]  field1  First field
/// @param[in]  field2  Second field
///
/// @return     Inner product value as scalar
///
template<class Type, template<class> class PatchField, class GeoMesh >
scalar computeInnerProduct(
    const GeometricField<Type, PatchField, GeoMesh>& field1,
    const GeometricField<Type, PatchField, GeoMesh>& field2);

//------------------------------------------------------------------------------
/// @brief      Helper function to compute Frobenius norm inner product
///
/// @param[in]  field1  First field
/// @param[in]  field2  Second field
///
/// @return     Inner product value as scalar
///
template<class Type, template<class> class PatchField, class GeoMesh>
scalar computeFrobeniusInnerProduct(
    const GeometricField<Type, PatchField, GeoMesh>& field1,
    const GeometricField<Type, PatchField, GeoMesh>& field2);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif



