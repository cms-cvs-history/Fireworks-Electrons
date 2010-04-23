// -*- C++ -*-
//
// Package:     Electrons
// Class  :     FWElectronProxyBuilder
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Chris Jones
//         Created:  Tue Dec  2 14:17:03 EST 2008
// $Id: FWElectronProxyBuilder.cc,v 1.10 2010/04/20 20:49:42 amraktad Exp $
//
#include "TEveCompound.h"
#include "TEveTrack.h"

#include "Fireworks/Core/interface/FWProxyBuilderTemplate.h"
#include "Fireworks/Core/interface/FWEventItem.h"
#include "Fireworks/Core/interface/FWViewType.h"

#include "Fireworks/Candidates/interface/CandidateUtils.h"
#include "Fireworks/Tracks/interface/TrackUtils.h"
#include "Fireworks/Electrons/interface/makeSuperCluster.h" 

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"


////////////////////////////////////////////////////////////////////////////////
//
//   3D and RPZ proxy builder with shared track list
// 
////////////////////////////////////////////////////////////////////////////////

class FWElectronProxyBuilder : public FWProxyBuilderTemplate<reco::GsfElectron> {

public:
   FWElectronProxyBuilder() ;
   virtual ~FWElectronProxyBuilder();

   virtual bool haveSingleProduct() const { return false; }
   virtual void cleanLocal();

   REGISTER_PROXYBUILDER_METHODS();

private:
   FWElectronProxyBuilder( const FWElectronProxyBuilder& ); // stop default
   const FWElectronProxyBuilder& operator=( const FWElectronProxyBuilder& ); // stop default

   virtual void buildViewType( const FWEventItem* iItem, TEveElementList* product, FWViewType::EType type );
   TEveElementList* requestCommon();

   TEveElementList* m_common;
};


FWElectronProxyBuilder::FWElectronProxyBuilder():
   m_common(0)
{
   m_common = new TEveElementList( "common electron scene" );
   m_common->IncDenyDestroy();
}

FWElectronProxyBuilder::~FWElectronProxyBuilder()
{
   m_common->DecDenyDestroy();
}

TEveElementList*
FWElectronProxyBuilder::requestCommon()
{
   if( m_common->HasChildren() == false )
   {
      for (int i = 0; i < static_cast<int>(item()->size()); ++i)
      {
         const reco::GsfElectron &electron = modelData(i);
         TEveTrack* track(0);
         if( electron.gsfTrack().isAvailable() )
            track = fireworks::prepareTrack( *electron.gsfTrack(),
                                             context().getTrackPropagator());
         else
            track = fireworks::prepareCandidate( electron,
                                                 context().getTrackPropagator());
         track->MakeTrack();
         setupElement(track);
         m_common->AddElement( track );
      }
   }
   return m_common;
}

void
FWElectronProxyBuilder::cleanLocal()
{
   m_common->DestroyElements();
}

void
FWElectronProxyBuilder::buildViewType( const FWEventItem* iItem, TEveElementList* product, FWViewType::EType type )
{
   TEveElementList*   tracks = requestCommon();
   TEveElement::List_i trkIt = tracks->BeginChildren();

   for (int i = 0; i < static_cast<int>(iItem->size()); ++i, ++trkIt)
   {
      const reco::GsfElectron &electron = modelData(i);

      TEveCompound* comp = createCompound();
      comp->AddElement( *trkIt );
      if( type == FWViewType::kRhoPhi )
         fireworks::makeRhoPhiSuperCluster( this,
					    electron.superCluster(),
					    electron.phi(),
					    *comp );
      else if( type == FWViewType::kRhoZ )
         fireworks::makeRhoZSuperCluster( this,
					  electron.superCluster(),
					  electron.phi(),
					  *comp );

      setupAddElement(comp, product);
   }
}

REGISTER_FWPROXYBUILDER( FWElectronProxyBuilder, reco::GsfElectron, "Electrons", FWViewType::kAll3DBits | FWViewType::kAllRPZBits );



////////////////////////////////////////////////////////////////////////////////
//
//   GLIMPSE specific proxy builder
// 
////////////////////////////////////////////////////////////////////////////////

#include "Fireworks/Core/interface/FWSimpleProxyBuilderTemplate.h"
#include "Fireworks/Core/interface/FWEveScalableStraightLineSet.h"

class FWElectronGlimpseProxyBuilder : public FWSimpleProxyBuilderTemplate<reco::GsfElectron> {
public:
   FWElectronGlimpseProxyBuilder() {}
   virtual ~FWElectronGlimpseProxyBuilder() {}

   REGISTER_PROXYBUILDER_METHODS();

private:
   FWElectronGlimpseProxyBuilder(const FWElectronGlimpseProxyBuilder&); // stop default

   const FWElectronGlimpseProxyBuilder& operator=(const FWElectronGlimpseProxyBuilder&); // stop default

   virtual void build(const reco::GsfElectron& iData, unsigned int iIndex, TEveElement& oItemHolder);
};

void
FWElectronGlimpseProxyBuilder::build( const reco::GsfElectron& iData, unsigned int iIndex,TEveElement& oItemHolder ) 
{
   FWEveScalableStraightLineSet* marker = new FWEveScalableStraightLineSet("", "");
   marker->SetLineWidth(2);
   fireworks::addStraightLineSegment( marker, &iData, 1.0 );
   setupAddElement(marker, &oItemHolder);
   //add to scaler at end so that it can scale the line after all ends have been added
   // FIXME: It's not a part of a standard FWSimpleProxyBuilderTemplate: the scaler is not set!
//    assert(scaler());
//    scaler()->addElement(marker);
}
REGISTER_FWPROXYBUILDER(FWElectronGlimpseProxyBuilder, reco::GsfElectron, "Electrons", FWViewType::kGlimpseBit);
