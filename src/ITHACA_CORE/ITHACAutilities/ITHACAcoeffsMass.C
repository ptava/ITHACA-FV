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
\*---------------------------------------------------------------------------*/
#include "ITHACAcoeffsMass.H"

namespace ITHACAutilities
{

template<class Type, template<class> class PatchField, class GeoMesh>
PtrList<GeometricField<Type, PatchField, GeoMesh >> reconstructFromCoeff(
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes,
    Eigen::MatrixXd& coeff_matrix, label Nmodes)
{
    PtrList<GeometricField<Type, PatchField, GeoMesh >> rec_field;
    rec_field.resize(0);
    for (label k = 0; k < coeff_matrix.cols(); k++)
    {
        for (label i = 0; i < Nmodes; i++)
            if (i == 0)
            {
                rec_field.append(modes[i] * coeff_matrix(i, k));
            }

            else
            {
                rec_field[k] +=  modes[i] * coeff_matrix(i, k);
            }
    }

    return rec_field;
}

template PtrList<GeometricField<scalar, fvPatchField, volMesh >>
reconstructFromCoeff(
    PtrList<GeometricField<scalar, fvPatchField, volMesh >> & modes,
    Eigen::MatrixXd& coeff_matrix, label Nmodes);
template PtrList<GeometricField<vector, fvPatchField, volMesh >>
reconstructFromCoeff(
    PtrList<GeometricField<vector, fvPatchField, volMesh >> & modes,
    Eigen::MatrixXd& coeff_matrix, label Nmodes);
template PtrList<GeometricField<tensor, fvPatchField, volMesh >>
reconstructFromCoeff(
    PtrList<GeometricField<tensor, fvPatchField, volMesh >> & modes,
    Eigen::MatrixXd& coeff_matrix, label Nmodes);

template<class Type, template<class> class PatchField, class GeoMesh >
Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & modes, label Nmodes,
    bool consider_volumes)
{
    label Msize;
    if (Nmodes == 0)
    {
        Msize =  modes.size();
    }

    else
    {
        Msize = Nmodes;
    }
    M_Assert(modes.size() >= Msize,
             "The Number of requested modes is larger then the available quantity.");
    Eigen::MatrixXd F = Foam2Eigen::PtrList2Eigen(modes);
    Eigen::MatrixXd M = Eigen::MatrixXd::Zero(Msize, Msize);

    if (consider_volumes)
    {
        Eigen::VectorXd V = getMassMatrixFV(modes[0]);

        // If V is too big then we need a for loop to avoid RAM overload
        if (V.size() > 1e6)
        {
            for (int i = 0; i < V.size(); i++)
            {
                M += V(i) * F.transpose().topRows(Msize).col(i) * F.leftCols(Msize).row(i);
            }
        }
        else
        {
            M = F.transpose().topRows(Msize) * V.asDiagonal() * F.leftCols(Msize);
        }
    }
    else
    {
        M = F.transpose().topRows(Msize) * F.leftCols(Msize);
    }

    if (Pstream::parRun())
    {
        reduce(M, sumOp<Eigen::MatrixXd>());
    }

    return M;
}

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<scalar, fvPatchField, volMesh >> & modes, label Nmodes,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >>& modes,
    label Nmodes,
    bool consider_volumes = false);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<vector, fvPatchField, volMesh >> & modes, label Nmodes,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<tensor, fvPatchField, volMesh >>& modes, label Nmodes,
    bool consider_volumes);

template<class Type, template<class> class PatchField, class GeoMesh>
Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<Type, PatchField, GeoMesh >> & modes,
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes2, label Nmodes,
    bool consider_volumes)
{
    label Msize, Msize2;

    if (Nmodes == 0)
    {
        Msize =  modes.size();
        Msize2 =  modes2.size();
    }
    else
    {
        Msize = Nmodes;
        Msize2 = Nmodes;
    }

    M_Assert(modes.size() >= Msize,
             "The Number of requested modes is larger then the available quantity.");
    M_Assert(modes2.size() >= Msize2,
             "The Number of requested modes is larger then the available quantity.");
    Eigen::MatrixXd F = Foam2Eigen::PtrList2Eigen(modes);
    Eigen::MatrixXd F2 = Foam2Eigen::PtrList2Eigen(modes2);
    Eigen::MatrixXd M = Eigen::MatrixXd::Zero(Msize, Msize2);

    if (consider_volumes)
    {
        Eigen::VectorXd V = getMassMatrixFV(modes[0]);

        // If V is too big then we need a for loop to avoid RAM overload
        if (V.size() > 1e6)
        {
            for (int i = 0; i < V.size(); i++)
            {
                M += V(i) * F.transpose().topRows(Msize).col(i) * F2.leftCols(Msize2).row(i);
            }
        }
        else
        {
            // Classical M computation
            M = F.transpose().topRows(Msize) * V.asDiagonal() * F2.leftCols(Msize2);
        }
    }
    else
    {
        M = F.transpose().topRows(Msize) * F2.leftCols(Msize2);
    }

    if (Pstream::parRun())
    {
        reduce(M, sumOp<Eigen::MatrixXd>());
    }

    return M;
}

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<scalar, fvPatchField, volMesh >> & modes,
    PtrList<GeometricField<scalar, fvPatchField, volMesh >>& modes2, label Nmodes,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >> & modes,
    PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >>& modes2,
    label Nmodes,
    bool consider_volumes = false);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<vector, fvPatchField, volMesh >> & modes,
    PtrList<GeometricField<vector, fvPatchField, volMesh >>& modes2, label Nmodes,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<tensor, fvPatchField, volMesh >> & modes,
    PtrList<GeometricField<tensor, fvPatchField, volMesh >>& modes2, label Nmodes,
    bool consider_volumes);


template<class Type, template<class> class PatchField, class GeoMesh>
Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<Type, PatchField, GeoMesh >>
    & modes,
    PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes2,
    Eigen::VectorXd weights,
    label Nmodes,
    bool consider_volumes)
{
    label Msize, Msize2;

    if (Nmodes == 0)
    {
        Msize =  modes.size();
        Msize2 =  modes2.size();
    }
    else
    {
        Msize = Nmodes;
        Msize2 = Nmodes;
    }

    M_Assert(modes.size() >= Msize,
             "The Number of requested modes is larger then the available quantity.");
    M_Assert(modes2.size() >= Msize2,
             "The Number of requested modes is larger then the available quantity.");
    Eigen::MatrixXd F = Foam2Eigen::PtrList2Eigen(modes);
    Eigen::MatrixXd F2 = Foam2Eigen::PtrList2Eigen(modes2);
    Eigen::MatrixXd M = Eigen::MatrixXd::Zero(Msize, Msize2);

    if (consider_volumes)
    {
        Eigen::VectorXd V = getMassMatrixFV(modes[0]);

        // If V is too big then we need a for loop to avoid RAM overload
        if (V.size() > 1e6)
        {
            for (int i = 0; i < V.size(); i++)
            {
                M += (V(i) * weights(i)) * F.transpose().topRows(Msize).col(i) * F2.leftCols(
                         Msize2).row(i);
            }
        }
        else
        {
            // Classical M computation
            M = F.transpose().topRows(Msize) * V.asDiagonal() * ( weights.asDiagonal()
                * F2.leftCols(Msize2) );
        }
    }
    else
    {
        M = F.transpose().topRows(Msize) * weights.asDiagonal() * F2.leftCols(Msize2);
    }

    if (Pstream::parRun())
    {
        reduce(M, sumOp<Eigen::MatrixXd>());
    }

    return M;
}

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<scalar, fvPatchField, volMesh >> & modes,
    PtrList<GeometricField<scalar, fvPatchField, volMesh >>& modes2,
    Eigen::VectorXd weights,
    label Nmodes = 0,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >> & modes,
    PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >>& modes2,
    Eigen::VectorXd weights,
    label Nmodes = 0,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<vector, fvPatchField, volMesh >> & modes,
    PtrList<GeometricField<vector, fvPatchField, volMesh >>& modes2,
    Eigen::VectorXd weights,
    label Nmodes = 0,
    bool consider_volumes);

template Eigen::MatrixXd getMassMatrix(
    PtrList<GeometricField<tensor, fvPatchField, volMesh >> & modes,
    PtrList<GeometricField<tensor, fvPatchField, volMesh >>& modes2,
    Eigen::VectorXd weights,
    label Nmodes = 0,
    bool consider_volumes);


template<class Type, template<class> class PatchField, class GeoMesh>
Eigen::VectorXd getMassMatrixFV(
    GeometricField<Type, PatchField, GeoMesh>& snapshot)
{
    Eigen::MatrixXd snapEigen = Foam2Eigen::field2Eigen(snapshot);
    label dim = std::nearbyint(snapEigen.rows() / (snapshot.mesh().V()).size());
    Eigen::VectorXd volumes = Foam2Eigen::field2Eigen(snapshot.mesh().V());
    Eigen::VectorXd vol3 = volumes.replicate(dim, 1);
    return vol3;
}

template Eigen::VectorXd getMassMatrixFV(
    GeometricField<scalar, fvPatchField, volMesh>& snapshot);
template Eigen::VectorXd getMassMatrixFV(
    GeometricField<vector, fvPatchField, volMesh>& snapshot);
template Eigen::VectorXd getMassMatrixFV(
    GeometricField<tensor, fvPatchField, volMesh>& snapshot);

template<class Type, template<class> class PatchField, class GeoMesh>
Eigen::VectorXd getCoeffs(GeometricField<Type, PatchField, GeoMesh>&
                          snapshot,
                          PtrList<GeometricField<Type, PatchField, GeoMesh >> & modes, label Nmodes,
                          bool consider_volumes)
{
    label Msize;
    if (Nmodes == 0)
    {
        Msize =  modes.size();
    }

    else
    {
        Msize = Nmodes;
    }
    M_Assert(modes.size() >= Msize,
             "The Number of requested modes is larger then the available quantity.");
    Eigen::MatrixXd F = Foam2Eigen::PtrList2Eigen(modes);
    Eigen::MatrixXd M_matrix = getMassMatrix(modes, Nmodes, consider_volumes);
    Eigen::MatrixXd snapEigen = Foam2Eigen::field2Eigen(snapshot);
    Eigen::VectorXd a(Msize);
    Eigen::VectorXd b(Msize);

    if (consider_volumes)
    {
        Eigen::VectorXd V = getMassMatrixFV(modes[0]);
        b = F.transpose().topRows(Msize) * V.asDiagonal() * snapEigen;
    }
    else
    {
        b = F.transpose().topRows(Msize) * snapEigen;
    }

    if (Pstream::parRun())
    {
        reduce(b, sumOp<Eigen::VectorXd>());
    }

    a = M_matrix.fullPivLu().solve(b);
    return a;
}

template Eigen::VectorXd getCoeffs(
    GeometricField<scalar, fvPatchField, volMesh>&
    snapshot, PtrList<GeometricField<scalar, fvPatchField, volMesh >> & modes,
    label Nmodes,
    bool consider_volumes);

template Eigen::VectorXd getCoeffs(
    GeometricField<scalar, fvsPatchField, surfaceMesh>&
    snapshot, PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >> & modes,
    label Nmodes,
    bool consider_volumes = false);

template Eigen::VectorXd getCoeffs(
    GeometricField<vector, fvPatchField, volMesh>&
    snapshot, PtrList<GeometricField<vector, fvPatchField, volMesh >> & modes,
    label Nmodes,
    bool consider_volumes);

template<class Type, template<class> class PatchField, class GeoMesh >
Eigen::MatrixXd getCoeffs(PtrList<GeometricField<Type, PatchField, GeoMesh >> &
                          snapshots,
                          PtrList<GeometricField<Type, PatchField, GeoMesh >>& modes, label Nmodes,
                          bool consider_volumes)
{
    label Msize;

    if (Nmodes == 0)
    {
        Msize =  modes.size();
    }
    else
    {
        Msize = Nmodes;
    }

    M_Assert(modes.size() >= Msize,
             "The Number of requested modes is larger then the available quantity.");
    Eigen::MatrixXd coeff(Msize, snapshots.size());

    for (auto i = 0; i < snapshots.size(); i++)
    {
        coeff.col(i) = getCoeffs(snapshots[i], modes, Nmodes, consider_volumes);
    }

    return coeff;
}

template Eigen::MatrixXd getCoeffs(
    PtrList<GeometricField<scalar, fvPatchField, volMesh >> &
    snapshot, PtrList<GeometricField<scalar, fvPatchField, volMesh >>& modes,
    label Nmodes,
    bool consider_volumes);

template Eigen::MatrixXd getCoeffs(
    PtrList<GeometricField<vector, fvPatchField, volMesh >> &
    snapshot, PtrList<GeometricField<vector, fvPatchField, volMesh >>& modes,
    label Nmodes,
    bool consider_volumes);

template Eigen::MatrixXd getCoeffs(
    PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >> &
    snapshot, PtrList<GeometricField<scalar, fvsPatchField, surfaceMesh >>& modes,
    label Nmodes,
    bool consider_volumes);

Eigen::MatrixXd parTimeCombMat(List<Eigen::VectorXd>
                               acquiredSnapshotsTimes,
                               Eigen::MatrixXd parameters)
{
    label parsNum = parameters.cols();
    label parsSamplesNum = parameters.rows();
    M_Assert(parsSamplesNum == acquiredSnapshotsTimes.size(),
             "The list of time instants does not have the same number of vectors as the number of parameters samples");
    Eigen::MatrixXd comb;
    label totalSnapshotsNum = 0;

    for (label k = 0; k < acquiredSnapshotsTimes.size(); k++)
    {
        totalSnapshotsNum += acquiredSnapshotsTimes[k].size();
    }

    comb.resize(totalSnapshotsNum, parsNum + 1);
    label i = 0;

    for (label j = 0; j < acquiredSnapshotsTimes.size(); j++)
    {
        for (label k = 0; k < acquiredSnapshotsTimes[j].size(); k++)
        {
            comb(i, parsNum) = (acquiredSnapshotsTimes[j])(k, 0);
            comb.block(i, 0, 1, parsNum) = parameters.row(j);
            i = i + 1;
        }
    }

    return comb;
}

}
