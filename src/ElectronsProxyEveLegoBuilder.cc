// -*- C++ -*-
//
// Package:     Electron
// Class  :     ElectronsProxyEveLegoBuilder
// 
// Implementation:
//     <Notes on implementation>
//
// Original Author:  
//         Created:  Sun Jan  6 23:57:00 EST 2008
// $Id: ElectronsProxyEveLegoBuilder.cc,v 1.4 2008/06/09 19:54:03 chrjones Exp $
//

// system include files
#include "TEveElement.h"
#include "TColor.h"
#include "TGeoTube.h"
#include "TEveGeoShapeExtract.h"
#include "TEveTrans.h"
#include "TEveGeoNode.h"
#include "TROOT.h"
#include "TEveStraightLineSet.h"
#include "TEveCompound.h"

// user include files
#include "Fireworks/Electrons/interface/ElectronsProxyEveLegoBuilder.h"
#include "Fireworks/Core/interface/FWEventItem.h"
#include "Fireworks/Core/interface/BuilderUtils.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ElectronsProxyEveLegoBuilder::ElectronsProxyEveLegoBuilder()
{
}

// ElectronsProxyEveLegoBuilder::ElectronsProxyEveLegoBuilder(const ElectronsProxyEveLegoBuilder& rhs)
// {
//    // do actual copying here;
// }

ElectronsProxyEveLegoBuilder::~ElectronsProxyEveLegoBuilder()
{
}

//
// assignment operators
//
// const ElectronsProxyEveLegoBuilder& ElectronsProxyEveLegoBuilder::operator=(const ElectronsProxyEveLegoBuilder& rhs)
// {
//   //An exception safe implementation is
//   ElectronsProxyEveLegoBuilder temp(rhs);
//   swap(rhs);
//
//   return *this;
// }


void
ElectronsProxyEveLegoBuilder::build(const FWEventItem* iItem, TEveElementList** product)
{
   TEveElementList* tList = *product;

   if(0 == tList) {
      tList =  new TEveElementList(iItem->name().c_str(),"ElectronsLego",true);
      *product = tList;
      tList->SetMainColor(iItem->defaultDisplayProperties().color());
   } else {
      tList->DestroyElements();
   }
   
   const reco::GsfElectronCollection* electrons=0;
   iItem->get(electrons);
   if(0==electrons) {
      std::cout <<"Failed to get GsfElectrons"<<std::endl;
      return;
   }
   
   fw::NamedCounter counter("electron");

   for(reco::GsfElectronCollection::const_iterator electron = electrons->begin(); 
       electron != electrons->end(); ++electron, ++counter) {

      TEveCompound* container = new TEveCompound( counter.str().c_str() );
      container->OpenCompound();
      //guarantees that CloseCompound will be called no matter what happens
      boost::shared_ptr<TEveCompound> sentry(container,boost::mem_fn(&TEveCompound::CloseCompound));
      	
      
      TEveStraightLineSet* marker = new TEveStraightLineSet("energy");
      marker->SetLineWidth(1);
      marker->SetLineColor(  iItem->defaultDisplayProperties().color() );
      fw::addStraightLineSegment( marker, &*electron );
      container->AddElement(marker);
      tList->AddElement(container);
      //container->CloseCompound();
   }
}

REGISTER_FW3DLEGODATAPROXYBUILDER(ElectronsProxyEveLegoBuilder,reco::GsfElectronCollection,"Electrons");

