
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix3.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const Matrix3 Matrix3::ZERO(0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f);
const Matrix3 Matrix3::IDENTITY(1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f);
const float Matrix3::EPSILON				= 1e-06f;
const float Matrix3::ms_fSvdEpsilon		= 1e-04f;
const int Matrix3::ms_iSvdMaxIterations	= 32;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::Bidiagonalize (Matrix3& kA, Matrix3& kL, Matrix3& kR)
{
    float afV[3], afW[3];
    float fLength, fSign, fT1, fInvT1, fT2;
    bool bIdentity;

    // map first column to (*,0,0)
    fLength = sqrt(kA[0][0]*kA[0][0] + kA[1][0]*kA[1][0] +
        kA[2][0]*kA[2][0]);
    if ( fLength > 0.0f )
    {
        fSign = (kA[0][0] > 0.0f ? 1.0f : -1.0f);
        fT1 = kA[0][0] + fSign*fLength;
        fInvT1 = 1.0f/fT1;
        afV[1] = kA[1][0]*fInvT1;
        afV[2] = kA[2][0]*fInvT1;

        fT2 = -2.0f/(1.0f+afV[1]*afV[1]+afV[2]*afV[2]);
        afW[0] = fT2*(kA[0][0]+kA[1][0]*afV[1]+kA[2][0]*afV[2]);
        afW[1] = fT2*(kA[0][1]+kA[1][1]*afV[1]+kA[2][1]*afV[2]);
        afW[2] = fT2*(kA[0][2]+kA[1][2]*afV[1]+kA[2][2]*afV[2]);
        kA[0][0] += afW[0];
        kA[0][1] += afW[1];
        kA[0][2] += afW[2];
        kA[1][1] += afV[1]*afW[1];
        kA[1][2] += afV[1]*afW[2];
        kA[2][1] += afV[2]*afW[1];
        kA[2][2] += afV[2]*afW[2];

        kL[0][0] = 1.0f+fT2;
        kL[0][1] = kL[1][0] = fT2*afV[1];
        kL[0][2] = kL[2][0] = fT2*afV[2];
        kL[1][1] = 1.0f+fT2*afV[1]*afV[1];
        kL[1][2] = kL[2][1] = fT2*afV[1]*afV[2];
        kL[2][2] = 1.0f+fT2*afV[2]*afV[2];
        bIdentity = false;
    }
    else
    {
        kL = Matrix3::IDENTITY;
        bIdentity = true;
    }

    // map first row to (*,*,0)
    fLength = sqrt(kA[0][1]*kA[0][1]+kA[0][2]*kA[0][2]);
    if ( fLength > 0.0f )
    {
        fSign = (kA[0][1] > 0.0f ? 1.0f : -1.0f);
        fT1 = kA[0][1] + fSign*fLength;
        afV[2] = kA[0][2]/fT1;

        fT2 = -2.0f/(1.0f+afV[2]*afV[2]);
        afW[0] = fT2*(kA[0][1]+kA[0][2]*afV[2]);
        afW[1] = fT2*(kA[1][1]+kA[1][2]*afV[2]);
        afW[2] = fT2*(kA[2][1]+kA[2][2]*afV[2]);
        kA[0][1] += afW[0];
        kA[1][1] += afW[1];
        kA[1][2] += afW[1]*afV[2];
        kA[2][1] += afW[2];
        kA[2][2] += afW[2]*afV[2];
        
        kR[0][0] = 1.0f;
        kR[0][1] = kR[1][0] = 0.0f;
        kR[0][2] = kR[2][0] = 0.0f;
        kR[1][1] = 1.0f+fT2;
        kR[1][2] = kR[2][1] = fT2*afV[2];
        kR[2][2] = 1.0f+fT2*afV[2]*afV[2];
    }
    else
    {
        kR = Matrix3::IDENTITY;
    }

    // map second column to (*,*,0)
    fLength = sqrt(kA[1][1]*kA[1][1]+kA[2][1]*kA[2][1]);
    if ( fLength > 0.0f )
    {
        fSign = (kA[1][1] > 0.0f ? 1.0f : -1.0f);
        fT1 = kA[1][1] + fSign*fLength;
        afV[2] = kA[2][1]/fT1;

        fT2 = -2.0f/(1.0f+afV[2]*afV[2]);
        afW[1] = fT2*(kA[1][1]+kA[2][1]*afV[2]);
        afW[2] = fT2*(kA[1][2]+kA[2][2]*afV[2]);
        kA[1][1] += afW[1];
        kA[1][2] += afW[2];
        kA[2][2] += afV[2]*afW[2];

        float fA = 1.0f+fT2;
        float fB = fT2*afV[2];
        float fC = 1.0f+fB*afV[2];

        if ( bIdentity )
        {
            kL[0][0] = 1.0f;
            kL[0][1] = kL[1][0] = 0.0f;
            kL[0][2] = kL[2][0] = 0.0f;
            kL[1][1] = fA;
            kL[1][2] = kL[2][1] = fB;
            kL[2][2] = fC;
        }
        else
        {
            for (int iRow = 0; iRow < 3; iRow++)
            {
                float fTmp0 = kL[iRow][1];
                float fTmp1 = kL[iRow][2];
                kL[iRow][1] = fA*fTmp0+fB*fTmp1;
                kL[iRow][2] = fB*fTmp0+fC*fTmp1;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::GolubKahanStep (Matrix3& kA, Matrix3& kL, Matrix3& kR)
{
    float fT11 = kA[0][1]*kA[0][1]+kA[1][1]*kA[1][1];
    float fT22 = kA[1][2]*kA[1][2]+kA[2][2]*kA[2][2];
    float fT12 = kA[1][1]*kA[1][2];
    float fTrace = fT11+fT22;
    float fDiff = fT11-fT22;
    float fDiscr = sqrt(fDiff*fDiff+4.0f*fT12*fT12);
    float fRoot1 = 0.5f*(fTrace+fDiscr);
    float fRoot2 = 0.5f*(fTrace-fDiscr);

    // adjust right
    float fY = kA[0][0] - (fabs(fRoot1-fT22) <=
        fabs(fRoot2-fT22) ? fRoot1 : fRoot2);
    float fZ = kA[0][1];
    float fInvLength = 1.0f/sqrt(fY*fY+fZ*fZ);
    float fsin = fZ*fInvLength;
    float fcos = -fY*fInvLength;

    float fTmp0 = kA[0][0];
    float fTmp1 = kA[0][1];
    kA[0][0] = fcos*fTmp0-fsin*fTmp1;
    kA[0][1] = fsin*fTmp0+fcos*fTmp1;
    kA[1][0] = -fsin*kA[1][1];
    kA[1][1] *= fcos;

    int iRow;
    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = kR[0][iRow];
        fTmp1 = kR[1][iRow];
        kR[0][iRow] = fcos*fTmp0-fsin*fTmp1;
        kR[1][iRow] = fsin*fTmp0+fcos*fTmp1;
    }

    // adjust left
    fY = kA[0][0];
    fZ = kA[1][0];
    fInvLength = 1.0f/sqrt(fY*fY+fZ*fZ);
    fsin = fZ*fInvLength;
    fcos = -fY*fInvLength;

    kA[0][0] = fcos*kA[0][0]-fsin*kA[1][0];
    fTmp0 = kA[0][1];
    fTmp1 = kA[1][1];
    kA[0][1] = fcos*fTmp0-fsin*fTmp1;
    kA[1][1] = fsin*fTmp0+fcos*fTmp1;
    kA[0][2] = -fsin*kA[1][2];
    kA[1][2] *= fcos;

    int iCol;
    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = kL[iCol][0];
        fTmp1 = kL[iCol][1];
        kL[iCol][0] = fcos*fTmp0-fsin*fTmp1;
        kL[iCol][1] = fsin*fTmp0+fcos*fTmp1;
    }

    // adjust right
    fY = kA[0][1];
    fZ = kA[0][2];
    fInvLength = 1.0f/sqrt(fY*fY+fZ*fZ);
    fsin = fZ*fInvLength;
    fcos = -fY*fInvLength;

    kA[0][1] = fcos*kA[0][1]-fsin*kA[0][2];
    fTmp0 = kA[1][1];
    fTmp1 = kA[1][2];
    kA[1][1] = fcos*fTmp0-fsin*fTmp1;
    kA[1][2] = fsin*fTmp0+fcos*fTmp1;
    kA[2][1] = -fsin*kA[2][2];
    kA[2][2] *= fcos;

    for (iRow = 0; iRow < 3; iRow++)
    {
        fTmp0 = kR[1][iRow];
        fTmp1 = kR[2][iRow];
        kR[1][iRow] = fcos*fTmp0-fsin*fTmp1;
        kR[2][iRow] = fsin*fTmp0+fcos*fTmp1;
    }

    // adjust left
    fY = kA[1][1];
    fZ = kA[2][1];
    fInvLength = 1.0f/sqrt(fY*fY+fZ*fZ);
    fsin = fZ*fInvLength;
    fcos = -fY*fInvLength;

    kA[1][1] = fcos*kA[1][1]-fsin*kA[2][1];
    fTmp0 = kA[1][2];
    fTmp1 = kA[2][2];
    kA[1][2] = fcos*fTmp0-fsin*fTmp1;
    kA[2][2] = fsin*fTmp0+fcos*fTmp1;

    for (iCol = 0; iCol < 3; iCol++)
    {
        fTmp0 = kL[iCol][1];
        fTmp1 = kL[iCol][2];
        kL[iCol][1] = fcos*fTmp0-fsin*fTmp1;
        kL[iCol][2] = fsin*fTmp0+fcos*fTmp1;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::SingularValueDecomposition (Matrix3& kL, Vector3& kS, Matrix3& kR) const
{
    int iRow, iCol;

    Matrix3 kA = *this;
    Bidiagonalize(kA,kL,kR);

    for (int i = 0; i < ms_iSvdMaxIterations; i++)
    {
        float fTmp, fTmp0, fTmp1;
        float fsin0, fcos0, fTan0;
        float fsin1, fcos1, fTan1;

        bool bTest1 = (fabs(kA[0][1]) <=
            ms_fSvdEpsilon*(fabs(kA[0][0])+fabs(kA[1][1])));
        bool bTest2 = (fabs(kA[1][2]) <=
            ms_fSvdEpsilon*(fabs(kA[1][1])+fabs(kA[2][2])));
        if ( bTest1 )
        {
            if ( bTest2 )
            {
                kS[0] = kA[0][0];
                kS[1] = kA[1][1];
                kS[2] = kA[2][2];
                break;
            }
            else
            {
                // 2x2 closed form factorization
                fTmp = (kA[1][1]*kA[1][1] - kA[2][2]*kA[2][2] +
                    kA[1][2]*kA[1][2])/(kA[1][2]*kA[2][2]);
                fTan0 = 0.5f*(fTmp+sqrt(fTmp*fTmp + 4.0f));
                fcos0 = 1.0f/sqrt(1.0f+fTan0*fTan0);
                fsin0 = fTan0*fcos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = kL[iCol][1];
                    fTmp1 = kL[iCol][2];
                    kL[iCol][1] = fcos0*fTmp0-fsin0*fTmp1;
                    kL[iCol][2] = fsin0*fTmp0+fcos0*fTmp1;
                }
                
                fTan1 = (kA[1][2]-kA[2][2]*fTan0)/kA[1][1];
                fcos1 = 1.0f/sqrt(1.0f+fTan1*fTan1);
                fsin1 = -fTan1*fcos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = kR[1][iRow];
                    fTmp1 = kR[2][iRow];
                    kR[1][iRow] = fcos1*fTmp0-fsin1*fTmp1;
                    kR[2][iRow] = fsin1*fTmp0+fcos1*fTmp1;
                }

                kS[0] = kA[0][0];
                kS[1] = fcos0*fcos1*kA[1][1] -
                    fsin1*(fcos0*kA[1][2]-fsin0*kA[2][2]);
                kS[2] = fsin0*fsin1*kA[1][1] +
                    fcos1*(fsin0*kA[1][2]+fcos0*kA[2][2]);
                break;
            }
        }
        else 
        {
            if ( bTest2 )
            {
                // 2x2 closed form factorization 
                fTmp = (kA[0][0]*kA[0][0] + kA[1][1]*kA[1][1] -
                    kA[0][1]*kA[0][1])/(kA[0][1]*kA[1][1]);
                fTan0 = 0.5f*(-fTmp+sqrt(fTmp*fTmp + 4.0f));
                fcos0 = 1.0f/sqrt(1.0f+fTan0*fTan0);
                fsin0 = fTan0*fcos0;

                for (iCol = 0; iCol < 3; iCol++)
                {
                    fTmp0 = kL[iCol][0];
                    fTmp1 = kL[iCol][1];
                    kL[iCol][0] = fcos0*fTmp0-fsin0*fTmp1;
                    kL[iCol][1] = fsin0*fTmp0+fcos0*fTmp1;
                }
                
                fTan1 = (kA[0][1]-kA[1][1]*fTan0)/kA[0][0];
                fcos1 = 1.0f/sqrt(1.0f+fTan1*fTan1);
                fsin1 = -fTan1*fcos1;

                for (iRow = 0; iRow < 3; iRow++)
                {
                    fTmp0 = kR[0][iRow];
                    fTmp1 = kR[1][iRow];
                    kR[0][iRow] = fcos1*fTmp0-fsin1*fTmp1;
                    kR[1][iRow] = fsin1*fTmp0+fcos1*fTmp1;
                }

                kS[0] = fcos0*fcos1*kA[0][0] -
                    fsin1*(fcos0*kA[0][1]-fsin0*kA[1][1]);
                kS[1] = fsin0*fsin1*kA[0][0] +
                    fcos1*(fsin0*kA[0][1]+fcos0*kA[1][1]);
                kS[2] = kA[2][2];
                break;
            }
            else
            {
                GolubKahanStep(kA,kL,kR);
            }
        }
    }

    // positize diagonal
    for (iRow = 0; iRow < 3; iRow++)
    {
        if ( kS[iRow] < 0.0f )
        {
            kS[iRow] = -kS[iRow];
            for (iCol = 0; iCol < 3; iCol++)
                kR[iRow][iCol] = -kR[iRow][iCol];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::SingularValueComposition (const Matrix3& kL, const Vector3& kS, const Matrix3& kR)
{
    int iRow, iCol;
    Matrix3 kTmp;

    // product S*R
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kTmp[iRow][iCol] = kS[iRow]*kR[iRow][iCol];
    }

    // product L*S*R
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            m[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 3; iMid++)
                m[iRow][iCol] += kL[iRow][iMid]*kTmp[iMid][iCol];
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    float fInvLength = 1.0f/sqrt(m[0][0]*m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0]);

    m[0][0] *= fInvLength;
    m[1][0] *= fInvLength;
    m[2][0] *= fInvLength;

    // compute q1
    float fDot0 = m[0][0]*m[0][1] + m[1][0]*m[1][1] + m[2][0]*m[2][1];

    m[0][1] -= fDot0*m[0][0];
    m[1][1] -= fDot0*m[1][0];
    m[2][1] -= fDot0*m[2][0];

    fInvLength = 1.0f/sqrt(m[0][1]*m[0][1] + m[1][1]*m[1][1] + m[2][1]*m[2][1]);

    m[0][1] *= fInvLength;
    m[1][1] *= fInvLength;
    m[2][1] *= fInvLength;

    // compute q2
    float fDot1 = m[0][1]*m[0][2] + m[1][1]*m[1][2] + m[2][1]*m[2][2];

    fDot0 = m[0][0]*m[0][2] + m[1][0]*m[1][2] + m[2][0]*m[2][2];

    m[0][2] -= fDot0*m[0][0] + fDot1*m[0][1];
    m[1][2] -= fDot0*m[1][0] + fDot1*m[1][1];
    m[2][2] -= fDot0*m[2][0] + fDot1*m[2][1];

    fInvLength = 1.0f/sqrt(m[0][2]*m[0][2] + m[1][2]*m[1][2] + m[2][2]*m[2][2]);

    m[0][2] *= fInvLength;
    m[1][2] *= fInvLength;
    m[2][2] *= fInvLength;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::QDUDecomposition (Matrix3& kQ, Vector3& kD, Vector3& kU) const
{
    // Factor M = QR = QDU where Q is orthogonal, D is diagonal, 
    // and U is upper triangular with ones on its diagonal.  Algorithm uses
    // Gram-Schmidt orthogonalization (the QR algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    //
    // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
    // u02 = r02/r00, and u12 = r12/r11.

    // Q = rotation
    // D = scaling
    // U = shear

    // D stores the three diagonal entries r00, r11, r22
    // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

    // build orthogonal matrix Q
    float fInvLength = 1.0f/sqrt(m[0][0]*m[0][0] + m[1][0]*m[1][0] + m[2][0]*m[2][0]);
    kQ[0][0] = m[0][0]*fInvLength;
    kQ[1][0] = m[1][0]*fInvLength;
    kQ[2][0] = m[2][0]*fInvLength;

    float fDot = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
 
    kQ[0][1] = m[0][1]-fDot*kQ[0][0];
    kQ[1][1] = m[1][1]-fDot*kQ[1][0];
    kQ[2][1] = m[2][1]-fDot*kQ[2][0];

    fInvLength = 1.0f/sqrt(kQ[0][1]*kQ[0][1] + kQ[1][1]*kQ[1][1] + kQ[2][1]*kQ[2][1]);

    kQ[0][1] *= fInvLength;
    kQ[1][1] *= fInvLength;
    kQ[2][1] *= fInvLength;

    fDot = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];

    kQ[0][2] = m[0][2]-fDot*kQ[0][0];
    kQ[1][2] = m[1][2]-fDot*kQ[1][0];
    kQ[2][2] = m[2][2]-fDot*kQ[2][0];

    fDot = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];

    kQ[0][2] -= fDot*kQ[0][1];
    kQ[1][2] -= fDot*kQ[1][1];
    kQ[2][2] -= fDot*kQ[2][1];

    fInvLength = 1.0f/sqrt(kQ[0][2]*kQ[0][2] + kQ[1][2]*kQ[1][2] + kQ[2][2]*kQ[2][2]);

    kQ[0][2] *= fInvLength;
    kQ[1][2] *= fInvLength;
    kQ[2][2] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    float fDet = kQ[0][0]*kQ[1][1]*kQ[2][2] + kQ[0][1]*kQ[1][2]*kQ[2][0] +
				kQ[0][2]*kQ[1][0]*kQ[2][1] - kQ[0][2]*kQ[1][1]*kQ[2][0] -
				kQ[0][1]*kQ[1][0]*kQ[2][2] - kQ[0][0]*kQ[1][2]*kQ[2][1];

    if ( fDet < 0.0f )
    {
        for (int iRow = 0; iRow < 3; iRow++)
        {
            for (int iCol = 0; iCol < 3; iCol++)
                kQ[iRow][iCol] = -kQ[iRow][iCol];
        }
    }

    // build "right" matrix R
    Matrix3 kR;
    kR[0][0] = kQ[0][0]*m[0][0] + kQ[1][0]*m[1][0] + kQ[2][0]*m[2][0];
    kR[0][1] = kQ[0][0]*m[0][1] + kQ[1][0]*m[1][1] + kQ[2][0]*m[2][1];
    kR[1][1] = kQ[0][1]*m[0][1] + kQ[1][1]*m[1][1] + kQ[2][1]*m[2][1];
    kR[0][2] = kQ[0][0]*m[0][2] + kQ[1][0]*m[1][2] + kQ[2][0]*m[2][2];
    kR[1][2] = kQ[0][1]*m[0][2] + kQ[1][1]*m[1][2] + kQ[2][1]*m[2][2];
    kR[2][2] = kQ[0][2]*m[0][2] + kQ[1][2]*m[1][2] + kQ[2][2]*m[2][2];

    // the scaling component
    kD[0] = kR[0][0];
    kD[1] = kR[1][1];
    kD[2] = kR[2][2];

    // the shear component
    float fInvD0 = 1.0f/kD[0];
    kU[0] = kR[0][1]*fInvD0;
    kU[1] = kR[0][2]*fInvD0;
    kU[2] = kR[1][2]/kD[1];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float Matrix3::MaxCubicRoot (float afCoeff[3])
{
    // Spectral norm is for A^T*A, so characteristic polynomial
    // P(x) = c[0]+c[1]*x+c[2]*x^2+x^3 has three positive float roots.
    // This yields the assertions c[0] < 0 and c[2]*c[2] >= 3*c[1].

    // quick out for uniform scale (triple root)
    const float fOneThird = 1.0f/3.0f;
    const float fEpsilon = 1e-06f;
    float fDiscr = afCoeff[2]*afCoeff[2] - 3.0f*afCoeff[1];
    if ( fDiscr <= fEpsilon )
        return -fOneThird*afCoeff[2];

    // Compute an upper bound on roots of P(x).  This assumes that A^T*A
    // has been scaled by its largest entry.
    float fX = 1.0f;
    float fPoly = afCoeff[0]+fX*(afCoeff[1]+fX*(afCoeff[2]+fX));
    if ( fPoly < 0.0f )
    {
        // uses a matrix norm to find an upper bound on maximum root
        fX = fabs(afCoeff[0]);
        float fTmp = 1.0f+fabs(afCoeff[1]);
        if ( fTmp > fX )
            fX = fTmp;
        fTmp = 1.0f+fabs(afCoeff[2]);
        if ( fTmp > fX )
            fX = fTmp;
    }

    // Newton's method to find root
    float fTwoC2 = 2.0f*afCoeff[2];
    for (int i = 0; i < 16; i++)
    {
        fPoly = afCoeff[0]+fX*(afCoeff[1]+fX*(afCoeff[2]+fX));
        if ( fabs(fPoly) <= fEpsilon )
            return fX;

        float fDeriv = afCoeff[1]+fX*(fTwoC2+3.0f*fX);
        fX -= fPoly/fDeriv;
    }

    return fX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

float Matrix3::SpectralNorm () const
{
    Matrix3 kP;
    int iRow, iCol;
    float fPmax = 0.0f;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            kP[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 3; iMid++)
            {
                kP[iRow][iCol] +=
                    m[iMid][iRow]*m[iMid][iCol];
            }
            if ( kP[iRow][iCol] > fPmax )
                fPmax = kP[iRow][iCol];
        }
    }

    float fInvPmax = 1.0f/fPmax;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kP[iRow][iCol] *= fInvPmax;
    }

    float afCoeff[3];
    afCoeff[0] = -(kP[0][0]*(kP[1][1]*kP[2][2]-kP[1][2]*kP[2][1]) +
        kP[0][1]*(kP[2][0]*kP[1][2]-kP[1][0]*kP[2][2]) +
        kP[0][2]*(kP[1][0]*kP[2][1]-kP[2][0]*kP[1][1]));
    afCoeff[1] = kP[0][0]*kP[1][1]-kP[0][1]*kP[1][0] +
        kP[0][0]*kP[2][2]-kP[0][2]*kP[2][0] +
        kP[1][1]*kP[2][2]-kP[1][2]*kP[2][1];
    afCoeff[2] = -(kP[0][0]+kP[1][1]+kP[2][2]);

    float fRoot = MaxCubicRoot(afCoeff);
    float fNorm = sqrt(fPmax*fRoot);
    return fNorm;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::ToAxisAngle (Vector3& rkAxis, float& rfRadians) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    float fTrace = m[0][0] + m[1][1] + m[2][2];
    float fcos = 0.5f*(fTrace-1.0f);
    rfRadians = acos(fcos);  // in [0,PI]

    if ( rfRadians > 0.0f )
    {
        if ( rfRadians < Math::PI )
        {
            rkAxis.x = m[2][1]-m[1][2];
            rkAxis.y = m[0][2]-m[2][0];
            rkAxis.z = m[1][0]-m[0][1];
            rkAxis.unitize();
        }
        else
        {
            // angle is PI
            float fHalfInverse;
            if ( m[0][0] >= m[1][1] )
            {
                // r00 >= r11
                if ( m[0][0] >= m[2][2] )
                {
                    // r00 is maximum diagonal term
                    rkAxis.x = 0.5f*sqrt(m[0][0] -
                        m[1][1] - m[2][2] + 1.0f);
                    fHalfInverse = 0.5f/rkAxis.x;
                    rkAxis.y = fHalfInverse*m[0][1];
                    rkAxis.z = fHalfInverse*m[0][2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis.z = 0.5f*sqrt(m[2][2] -
                        m[0][0] - m[1][1] + 1.0f);
                    fHalfInverse = 0.5f/rkAxis.z;
                    rkAxis.x = fHalfInverse*m[0][2];
                    rkAxis.y = fHalfInverse*m[1][2];
                }
            }
            else
            {
                // r11 > r00
                if ( m[1][1] >= m[2][2] )
                {
                    // r11 is maximum diagonal term
                    rkAxis.y = 0.5f*sqrt(m[1][1] -
                        m[0][0] - m[2][2] + 1.0f);
                    fHalfInverse  = 0.5f/rkAxis.y;
                    rkAxis.x = fHalfInverse*m[0][1];
                    rkAxis.z = fHalfInverse*m[1][2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis.z = 0.5f*sqrt(m[2][2] -
                        m[0][0] - m[1][1] + 1.0f);
                    fHalfInverse = 0.5f/rkAxis.z;
                    rkAxis.x = fHalfInverse*m[0][2];
                    rkAxis.y = fHalfInverse*m[1][2];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        rkAxis.x = 1.0f;
        rkAxis.y = 0.0f;
        rkAxis.z = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromAxisAngle (const Vector3& rkAxis, float fRadians)
{
    float fcos = cos(fRadians);
    float fsin = sin(fRadians);
    float fOneMinuscos = 1.0f-fcos;
    float fX2 = rkAxis.x*rkAxis.x;
    float fY2 = rkAxis.y*rkAxis.y;
    float fZ2 = rkAxis.z*rkAxis.z;
    float fXYM = rkAxis.x*rkAxis.y*fOneMinuscos;
    float fXZM = rkAxis.x*rkAxis.z*fOneMinuscos;
    float fYZM = rkAxis.y*rkAxis.z*fOneMinuscos;
    float fXsin = rkAxis.x*fsin;
    float fYsin = rkAxis.y*fsin;
    float fZsin = rkAxis.z*fsin;
    
    m[0][0] = fX2*fOneMinuscos+fcos;
    m[0][1] = fXYM-fZsin;
    m[0][2] = fXZM+fYsin;
    m[1][0] = fXYM+fZsin;
    m[1][1] = fY2*fOneMinuscos+fcos;
    m[1][2] = fYZM-fXsin;
    m[2][0] = fXZM-fYsin;
    m[2][1] = fYZM+fXsin;
    m[2][2] = fZ2*fOneMinuscos+fcos;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::ToEulerAnglesXYZ (float& rfXAngle, float& rfYAngle, float& rfZAngle) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m[0][2] < 1.0f )
    {
        if ( m[0][2] > -1.0f )
        {
            rfXAngle = atan2(-m[1][2],m[2][2]);
            rfYAngle = (float)asin(m[0][2]);
            rfZAngle = atan2(-m[0][1],m[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -atan2(m[1][0],m[1][1]);
            rfYAngle = -Math::HALF_PI;
            rfZAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = atan2(m[1][0],m[1][1]);
        rfYAngle = Math::HALF_PI;
        rfZAngle = 0.0f;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::ToEulerAnglesXZY (float& rfXAngle, float& rfZAngle, float& rfYAngle) const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if ( m[0][1] < 1.0f )
    {
        if ( m[0][1] > -1.0f )
        {
            rfXAngle = atan2(m[2][1],m[1][1]);
            rfZAngle = (float)asin(-m[0][1]);
            rfYAngle = atan2(m[0][2],m[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = atan2(m[2][0],m[2][2]);
            rfZAngle = Math::HALF_PI;
            rfYAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = atan2(-m[2][0],m[2][2]);
        rfZAngle = -Math::HALF_PI;
        rfYAngle = 0.0f;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::ToEulerAnglesYXZ (float& rfYAngle, float& rfXAngle, float& rfZAngle) const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if ( m[1][2] < 1.0f )
    {
        if ( m[1][2] > -1.0f )
        {
            rfYAngle = atan2(m[0][2],m[2][2]);
            rfXAngle = (float)asin(-m[1][2]);
            rfZAngle = atan2(m[1][0],m[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = atan2(m[0][1],m[0][0]);
            rfXAngle = Math::HALF_PI;
            rfZAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = atan2(-m[0][1],m[0][0]);
        rfXAngle = -Math::HALF_PI;
        rfZAngle = 0.0f;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::ToEulerAnglesYZX (float& rfYAngle, float& rfZAngle, float& rfXAngle) const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if ( m[1][0] < 1.0f )
    {
        if ( m[1][0] > -1.0f )
        {
            rfYAngle = atan2(-m[2][0],m[0][0]);
            rfZAngle = (float)asin(m[1][0]);
            rfXAngle = atan2(-m[1][2],m[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = -atan2(m[2][1],m[2][2]);
            rfZAngle = -Math::HALF_PI;
            rfXAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = atan2(m[2][1],m[2][2]);
        rfZAngle = Math::HALF_PI;
        rfXAngle = 0.0f;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::ToEulerAnglesZXY (float& rfZAngle, float& rfXAngle, float& rfYAngle) const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if ( m[2][1] < 1.0f )
    {
        if ( m[2][1] > -1.0f )
        {
            rfZAngle = atan2(-m[0][1],m[1][1]);
            rfXAngle = (float)asin(m[2][1]);
            rfYAngle = atan2(-m[2][0],m[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = -atan2(m[0][2],m[0][0]);
            rfXAngle = -Math::HALF_PI;
            rfYAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = atan2(m[0][2],m[0][0]);
        rfXAngle = Math::HALF_PI;
        rfYAngle = 0.0f;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::ToEulerAnglesZYX (float& rfZAngle, float& rfYAngle, float& rfXAngle) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( m[2][0] < 1.0f )
    {
        if ( m[2][0] > -1.0f )
        {
            rfZAngle = atan2(m[1][0],m[0][0]);
            rfYAngle = (float)asin(-m[2][0]);
            rfXAngle = atan2(m[2][1],m[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -atan2(m[0][1],m[0][2]);
            rfYAngle = Math::HALF_PI;
            rfXAngle = 0.0f;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = atan2(-m[0][1],-m[0][2]);
        rfYAngle = -Math::HALF_PI;
        rfXAngle = 0.0f;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromEulerAnglesXYZ (float fYAngle, float fPAngle, float fRAngle)
{
    float fcos, fsin;

    fcos = cos(fYAngle);
    fsin = sin(fYAngle);
    Matrix3 kXMat(1.0f,0.0f,0.0f,0.0f,fcos,-fsin,0.0f,fsin,fcos);

    fcos = cos(fPAngle);
    fsin = sin(fPAngle);
    Matrix3 kYMat(fcos,0.0f,fsin,0.0f,1.0f,0.0f,-fsin,0.0f,fcos);

    fcos = cos(fRAngle);
    fsin = sin(fRAngle);
    Matrix3 kZMat(fcos,-fsin,0.0f,fsin,fcos,0.0f,0.0f,0.0f,1.0f);

    *this = kXMat*(kYMat*kZMat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromEulerAnglesXZY (float fYAngle, float fPAngle, float fRAngle)
{
    float fcos, fsin;

    fcos = cos(fYAngle);
    fsin = sin(fYAngle);
    Matrix3 kXMat(1.0f,0.0f,0.0f,0.0f,fcos,-fsin,0.0f,fsin,fcos);

    fcos = cos(fPAngle);
    fsin = sin(fPAngle);
    Matrix3 kZMat(fcos,-fsin,0.0f,fsin,fcos,0.0f,0.0f,0.0f,1.0f);

    fcos = cos(fRAngle);
    fsin = sin(fRAngle);
    Matrix3 kYMat(fcos,0.0f,fsin,0.0f,1.0f,0.0f,-fsin,0.0f,fcos);

    *this = kXMat*(kZMat*kYMat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromEulerAnglesYXZ (float fYAngle, float fPAngle, float fRAngle)
{
    float fcos, fsin;

    fcos = cos(fYAngle);
    fsin = sin(fYAngle);
    Matrix3 kYMat(fcos,0.0f,fsin,0.0f,1.0f,0.0f,-fsin,0.0f,fcos);

    fcos = cos(fPAngle);
    fsin = sin(fPAngle);
    Matrix3 kXMat(1.0f,0.0f,0.0f,0.0f,fcos,-fsin,0.0f,fsin,fcos);

    fcos = cos(fRAngle);
    fsin = sin(fRAngle);
    Matrix3 kZMat(fcos,-fsin,0.0f,fsin,fcos,0.0f,0.0f,0.0f,1.0f);

    *this = kYMat*(kXMat*kZMat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromEulerAnglesYZX (float fYAngle, float fPAngle, float fRAngle)
{
    float fcos, fsin;

    fcos = cos(fYAngle);
    fsin = sin(fYAngle);
    Matrix3 kYMat(fcos,0.0f,fsin,0.0f,1.0f,0.0f,-fsin,0.0f,fcos);

    fcos = cos(fPAngle);
    fsin = sin(fPAngle);
    Matrix3 kZMat(fcos,-fsin,0.0f,fsin,fcos,0.0f,0.0f,0.0f,1.0f);

    fcos = cos(fRAngle);
    fsin = sin(fRAngle);
    Matrix3 kXMat(1.0f,0.0f,0.0f,0.0f,fcos,-fsin,0.0f,fsin,fcos);

    *this = kYMat*(kZMat*kXMat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromEulerAnglesZXY (float fYAngle, float fPAngle, float fRAngle)
{
    float fcos, fsin;

    fcos = cos(fYAngle);
    fsin = sin(fYAngle);
    Matrix3 kZMat(fcos,-fsin,0.0f,fsin,fcos,0.0f,0.0f,0.0f,1.0f);

    fcos = cos(fPAngle);
    fsin = sin(fPAngle);
    Matrix3 kXMat(1.0f,0.0f,0.0f,0.0f,fcos,-fsin,0.0f,fsin,fcos);

    fcos = cos(fRAngle);
    fsin = sin(fRAngle);
    Matrix3 kYMat(fcos,0.0f,fsin,0.0f,1.0f,0.0f,-fsin,0.0f,fcos);

    *this = kZMat*(kXMat*kYMat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::FromEulerAnglesZYX (float fYAngle, float fPAngle, float fRAngle)
{
    float fcos, fsin;

    fcos = cos(fYAngle);
    fsin = sin(fYAngle);
    Matrix3 kZMat(fcos,-fsin,0.0f,fsin,fcos,0.0f,0.0f,0.0f,1.0f);

    fcos = cos(fPAngle);
    fsin = sin(fPAngle);
    Matrix3 kYMat(fcos,0.0f,fsin,0.0f,1.0f,0.0f,-fsin,0.0f,fcos);

    fcos = cos(fRAngle);
    fsin = sin(fRAngle);
    Matrix3 kXMat(1.0f,0.0f,0.0f,0.0f,fcos,-fsin,0.0f,fsin,fcos);

    *this = kZMat*(kYMat*kXMat);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::Tridiagonal (float afDiag[3], float afSubDiag[3])
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    float fA = m[0][0];
    float fB = m[0][1];
    float fC = m[0][2];
    float fD = m[1][1];
    float fE = m[1][2];
    float fF = m[2][2];

    afDiag[0] = fA;
    afSubDiag[2] = 0.0f;
    if ( fabs(fC) >= EPSILON )
    {
        float fLength = sqrt(fB*fB+fC*fC);
        float fInvLength = 1.0f/fLength;
        fB *= fInvLength;
        fC *= fInvLength;
        float fQ = 2.0f*fB*fE+fC*(fF-fD);
        afDiag[1] = fD+fC*fQ;
        afDiag[2] = fF-fC*fQ;
        afSubDiag[0] = fLength;
        afSubDiag[1] = fE-fB*fQ;
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = fB;
        m[1][2] = fC;
        m[2][0] = 0.0f;
        m[2][1] = fC;
        m[2][2] = -fB;
    }
    else
    {
        afDiag[1] = fD;
        afDiag[2] = fF;
        afSubDiag[0] = fB;
        afSubDiag[1] = fE;
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[1][0] = 0.0f;
        m[1][1] = 1.0f;
        m[1][2] = 0.0f;
        m[2][0] = 0.0f;
        m[2][1] = 0.0f;
        m[2][2] = 1.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool Matrix3::QLAlgorithm (float afDiag[3], float afSubDiag[3])
{
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal

    for (int i0 = 0; i0 < 3; i0++)
    {
        const int iMaxIter = 32;
        int iIter;
        for (iIter = 0; iIter < iMaxIter; iIter++)
        {
            int i1;
            for (i1 = i0; i1 <= 1; i1++)
            {
                float fSum = fabs(afDiag[i1]) +
                    fabs(afDiag[i1+1]);
                if ( fabs(afSubDiag[i1]) + fSum == fSum )
                    break;
            }
            if ( i1 == i0 )
                break;

            float fTmp0 = (afDiag[i0+1]-afDiag[i0])/(2.0f*afSubDiag[i0]);
            float fTmp1 = sqrt(fTmp0*fTmp0+1.0f);
            if ( fTmp0 < 0.0f )
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
            else
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
            float fsin = 1.0f;
            float fcos = 1.0f;
            float fTmp2 = 0.0f;
            for (int i2 = i1-1; i2 >= i0; i2--)
            {
                float fTmp3 = fsin*afSubDiag[i2];
                float fTmp4 = fcos*afSubDiag[i2];
                if ( fabs(fTmp3) >= fabs(fTmp0) )
                {
                    fcos = fTmp0/fTmp3;
                    fTmp1 = sqrt(fcos*fcos+1.0f);
                    afSubDiag[i2+1] = fTmp3*fTmp1;
                    fsin = 1.0f/fTmp1;
                    fcos *= fsin;
                }
                else
                {
                    fsin = fTmp3/fTmp0;
                    fTmp1 = sqrt(fsin*fsin+1.0f);
                    afSubDiag[i2+1] = fTmp0*fTmp1;
                    fcos = 1.0f/fTmp1;
                    fsin *= fcos;
                }
                fTmp0 = afDiag[i2+1]-fTmp2;
                fTmp1 = (afDiag[i2]-fTmp0)*fsin+2.0f*fTmp4*fcos;
                fTmp2 = fsin*fTmp1;
                afDiag[i2+1] = fTmp0+fTmp2;
                fTmp0 = fcos*fTmp1-fTmp4;

                for (int iRow = 0; iRow < 3; iRow++)
                {
                    fTmp3 = m[iRow][i2+1];
                    m[iRow][i2+1] = fsin*m[iRow][i2] +
                        fcos*fTmp3;
                    m[iRow][i2] = fcos*m[iRow][i2] -
                        fsin*fTmp3;
                }
            }
            afDiag[i0] -= fTmp2;
            afSubDiag[i0] = fTmp0;
            afSubDiag[i1] = 0.0f;
        }

        if ( iIter == iMaxIter )
        {
            // should not get here under normal circumstances
            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::EigenSolveSymmetric (float afEigenvalue[3], Vector3 akEigenvector[3]) const
{
    Matrix3 kMatrix = *this;
    float afSubDiag[3];
    kMatrix.Tridiagonal(afEigenvalue,afSubDiag);
    kMatrix.QLAlgorithm(afEigenvalue,afSubDiag);

    for (int i = 0; i < 3; i++)
    {
        akEigenvector[i][0] = kMatrix[0][i];
        akEigenvector[i][1] = kMatrix[1][i];
        akEigenvector[i][2] = kMatrix[2][i];
    }

    // make eigenvectors form a right--handed system
    Vector3 kCross = akEigenvector[1].cross(akEigenvector[2]);
    float fDet = akEigenvector[0].dot(kCross);
    if ( fDet < 0.0f )
    {
        akEigenvector[2][0] = - akEigenvector[2][0];
        akEigenvector[2][1] = - akEigenvector[2][1];
        akEigenvector[2][2] = - akEigenvector[2][2];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Matrix3::TensorProduct (const Vector3& rkU, const Vector3& rkV, Matrix3& rkProduct)
{
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
