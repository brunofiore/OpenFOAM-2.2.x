/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    interPhaseChangeDyMFoam

Description
    Solver for 2 incompressible, isothermal immiscible fluids with phase-change
    (e.g. cavitation).  Uses a VOF (volume of fluid) phase-fraction based
    interface capturing approach, with optional mesh motion and mesh topology
    changes including adaptive re-meshing.

    The momentum and other fluid properties are of the "mixture" and a
    single momentum equation is solved.

    The set of phase-change models provided are designed to simulate cavitation
    but other mechanisms of phase-change are supported within this solver
    framework.

    Turbulence modelling is generic, i.e. laminar, RAS or LES may be selected.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "dynamicFvMesh.H"
#include "MULES.H"
#include "subCycle.H"
#include "interfaceProperties.H"
#include "phaseChangeTwoPhaseMixture.H"
#include "turbulenceModel.H"
#include "pimpleControl.H"
#include "fvIOoptionList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    #include "createTime.H"
    #include "createDynamicFvMesh.H"
    #include "readGravitationalAcceleration.H"
    #include "initContinuityErrs.H"
    #include "createFields.H"
    #include "readTimeControls.H"

    pimpleControl pimple(mesh);

    surfaceScalarField phiAbs("phiAbs", phi);
    fvc::makeAbsolute(phiAbs, U);

    #include "../interFoam/interDyMFoam/correctPhi.H"
    #include "CourantNo.H"
    #include "setInitialDeltaT.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nStarting time loop\n" << endl;

    while (runTime.run())
    {
        #include "../interFoam/interDyMFoam/readControls.H"
        #include "CourantNo.H"
        #include "setDeltaT.H"

        runTime++;

        Info<< "Time = " << runTime.timeName() << nl << endl;

        scalar timeBeforeMeshUpdate = runTime.elapsedCpuTime();

        {
            // Ensure old-time U exists for mapping
            U.oldTime();

            // Calculate the relative velocity used to map the relative flux phi
            volVectorField Urel("Urel", U);

            if (mesh.moving())
            {
                Urel -= fvc::reconstruct(fvc::meshPhi(U));
            }

            // Do any mesh changes
            mesh.update();
        }

        if (mesh.changing())
        {
            Info<< "Execution time for mesh.update() = "
                << runTime.elapsedCpuTime() - timeBeforeMeshUpdate
                << " s" << endl;

            gh = g & mesh.C();
            ghf = g & mesh.Cf();
        }

        if (mesh.changing() && correctPhi)
        {
            #include "../interFoam/interDyMFoam/correctPhi.H"
        }

        if (mesh.changing() && checkMeshCourantNo)
        {
            #include "meshCourantNo.H"
        }

        twoPhaseProperties->correct();

        #include "alphaEqnSubCycle.H"
        interface.correct();

        // --- Pressure-velocity PIMPLE corrector loop
        while (pimple.loop())
        {
            #include "UEqn.H"

            // --- Pressure corrector loop
            while (pimple.correct())
            {
                #include "pEqn.H"
            }

            if (pimple.turbCorr())
            {
                turbulence->correct();
            }
        }

        runTime.write();

        Info<< "ExecutionTime = " << runTime.elapsedCpuTime() << " s"
            << "  ClockTime = " << runTime.elapsedClockTime() << " s"
            << nl << endl;
    }

    Info<< "End\n" << endl;

    return 0;
}


// ************************************************************************* //
