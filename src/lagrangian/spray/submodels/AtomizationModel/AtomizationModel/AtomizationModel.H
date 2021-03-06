/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
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

Class
    Foam::AtomizationModel

Description
    Templated atomization model class

SourceFiles
    AtomizationModel.C
    AtomizationModelNew.C

\*---------------------------------------------------------------------------*/

#ifndef AtomizationModel_H
#define AtomizationModel_H

#include "IOdictionary.H"
#include "autoPtr.H"
#include "runTimeSelectionTables.H"
#include "SubModelBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class AtomizationModel Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class AtomizationModel
:
    public SubModelBase<CloudType>
{
public:

    //- Runtime type information
    TypeName("atomizationModel");

    //- Declare runtime constructor selection table
    declareRunTimeSelectionTable
    (
        autoPtr,
        AtomizationModel,
        dictionary,
        (
            const dictionary& dict,
            CloudType& owner
        ),
        (dict, owner)
    );


    // Constructors

        //- Construct null from owner
        AtomizationModel(CloudType& owner);

        //- Construct from dictionary
        AtomizationModel
        (
            const dictionary& dict,
            CloudType& owner,
            const word& type
        );

        //- Construct copy
        AtomizationModel(const AtomizationModel<CloudType>& am);

        //- Construct and return a clone
        virtual autoPtr<AtomizationModel<CloudType> > clone() const
        {
            return autoPtr<AtomizationModel<CloudType> >
            (
                new AtomizationModel<CloudType>(*this)
            );
        }


    //- Destructor
    virtual ~AtomizationModel();


    //- Selector
    static autoPtr<AtomizationModel<CloudType> > New
    (
        const dictionary& dict,
        CloudType& owner
    );


    // Member Functions

        //- initial value of liquidCore
        virtual scalar initLiquidCore() const;

        //- Average temperature calculation
        scalar Taverage(const scalar& Tliq, const scalar& Tc) const;

        //- flag to indicate if chi needs to be calculated
        virtual bool calcChi() const;

        virtual void update
        (
            const scalar dt,
            scalar& d,
            scalar& liquidCore,
            scalar& tc,
            const scalar rho,
            const scalar mu,
            const scalar sigma,
            const scalar volFlowRate,
            const scalar rhoAv,
            const scalar Urel,
            const vector& pos,
            const vector& injectionPos,
            const scalar pAmbient,
            const scalar chi,
            cachedRandom& rndGen
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#define makeAtomizationModel(CloudType)                                       \
                                                                              \
    typedef CloudType::sprayCloudType sprayCloudType;                         \
    defineNamedTemplateTypeNameAndDebug                                       \
    (                                                                         \
        AtomizationModel<sprayCloudType>,                                     \
        0                                                                     \
    );                                                                        \
    defineTemplateRunTimeSelectionTable                                       \
    (                                                                         \
        AtomizationModel<sprayCloudType>,                                     \
        dictionary                                                            \
    );


#define makeAtomizationModelType(SS, CloudType)                               \
                                                                              \
    typedef CloudType::sprayCloudType sprayCloudType;                         \
    defineNamedTemplateTypeNameAndDebug(SS<sprayCloudType>, 0);               \
                                                                              \
    AtomizationModel<sprayCloudType>::                                        \
        adddictionaryConstructorToTable<SS<sprayCloudType> >                  \
            add##SS##CloudType##sprayCloudType##ConstructorToTable_;


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "AtomizationModel.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
