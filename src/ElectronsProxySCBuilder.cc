// -*- C++ -*-
//
// Package:     Calo
// Class  :     ElectronsProxySCBuilder
// 
// Implementation:
//     <Notes on implementation>
//
// Original Author:  
//         Created:  Sun Jan  6 23:57:00 EST 2008
// $Id: ElectronsProxySCBuilder.cc,v 1.2 2008/02/13 22:45:50 jmuelmen Exp $
//

// system include files
#include "TEveGeoNode.h"
#include "TEveGeoShapeExtract.h"
#include "TGeoBBox.h"
#include "TGeoArb8.h"
#include "TGeoTube.h"
#include "TEveManager.h"
#include "TH1F.h"
#include "TColor.h"
#include "TROOT.h"

// user include files
#include "Fireworks/Electrons/interface/ElectronsProxy3DBuilder.h"
#include "Fireworks/Electrons/interface/ElectronsProxySCBuilder.h"
#include "Fireworks/Core/interface/FWEventItem.h"
#include "Fireworks/Core/interface/FW3DLegoDataProxyBuilder.h"
#include "Fireworks/Core/interface/DetIdToMatrix.h"
#include "DataFormats/EgammaCandidates/interface/PixelMatchGsfElectron.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"
#include "DataFormats/EcalDetId/interface/EcalSubdetector.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ElectronsProxySCBuilder::ElectronsProxySCBuilder()
{
}

// ElectronsProxySCBuilder::ElectronsProxySCBuilder(const ElectronsProxySCBuilder& rhs)
// {
//    // do actual copying here;
// }

ElectronsProxySCBuilder::~ElectronsProxySCBuilder()
{
}

//
// member functions
//
void 
ElectronsProxySCBuilder::buildRhoPhi(const FWEventItem* iItem,
					    TEveElementList** product)
{
     TEveElementList* tList = *product;

     printf("calling ElectronsProxySCBuilder::buildRhiPhi\n");
     if(0 == tList) {
	  tList =  new TEveElementList(iItem->name().c_str(),"Electron RhoPhi",true);
	  *product = tList;
	  tList->SetMainColor(iItem->defaultDisplayProperties().color());
	  gEve->AddElement(tList);
     } else {
	  tList->DestroyElements();
     }
#if 0
     // get electrons
     using reco::PixelMatchGsfElectronCollection;
     const PixelMatchGsfElectronCollection *electrons = 0;
     printf("getting electrons\n");
     iItem->get(electrons);
     printf("got electrons\n");
     if (electrons == 0) {
	  std::cout <<"failed to get GSF electrons" << std::endl;
	  return;
     }
     printf("%d GSF electrons\n", electrons->size());
     // loop over electrons
     using std::string;
     string name = "superclusters";
     TEveGeoShapeExtract* container = new TEveGeoShapeExtract(name.c_str());
     char index[3] = "00";
     for (PixelMatchGsfElectronCollection::const_iterator i = electrons->begin();
	  i != electrons->end(); ++i, ++index[0]) {
	  assert(i->superCluster().isNonnull());
// 	  const SuperCluster &sc = i->superCluster().product();
	  DetId id = i->superCluster()->seed()->getHitsByDetId()[0];
	  if (id.subdetId() != EcalBarrel) 
	       // skip these for now
	       continue;
#if 0
	  double size = 1;
	  double r = 122;
	  double phi = i->superCluster()->position().phi();
	  double phi_deg = phi * 180 / M_PI;
	  TGeoBBox *sc_box = new TGeoTubeSeg(r - 1, r + 1, 1, 
					     phi_deg - 15, phi_deg + 15);
	  TEveGeoShapeExtract *extract = new TEveGeoShapeExtract((name + index).c_str());
	  TColor* c = gROOT->GetColor(tList->GetMainColor());
	  Float_t rgba[4] = { 1, 0, 0, 1 };
	  if (c) {
	       rgba[0] = c->GetRed();
	       rgba[1] = c->GetGreen();
	       rgba[2] = c->GetBlue();
	  }
	  extract->SetRGBA(rgba);
	  extract->SetRnrSelf(true);
	  extract->SetRnrElements(true);
	  extract->SetShape(sc_box);
	  container->AddElement(extract);
#else
	  std::vector<DetId> detids = i->superCluster()->getHitsByDetId();
	  for (std::vector<DetId>::const_iterator k = detids.begin();
	       k != detids.end(); ++k, ++index[1]) {
// 	       const TGeoHMatrix* matrix = m_item->getGeom()->getMatrix( k->rawId() );
	       TEveGeoShapeExtract* extract = m_item->getGeom()->getExtract( k->rawId() );
	       assert(extract != 0);
	       TVector3 v(extract->GetTrans()[12], 
			  extract->GetTrans()[13], 
			  extract->GetTrans()[14]);
	       TEveElement* shape = TEveGeoShape::ImportShapeExtract(extract,0);
	       shape->SetMainTransparency(50);
	       shape->SetMainColor(tList->GetMainColor());
// 		    tList->AddElement(shape);
	       double size = 1;
	       double r = 122;
	       double phi = v.Phi();
	       double phi_deg_min = (phi - 0.0085) * 180 / M_PI;
	       double phi_deg_max = (phi + 0.0085) * 180 / M_PI;
	       TGeoBBox *sc_box = new TGeoTubeSeg(r - 1, r + 1, 1, 
						  phi_deg_min, phi_deg_max);
	       TEveGeoShapeExtract *extract2 = new TEveGeoShapeExtract((name + index).c_str());
	       TColor* c = gROOT->GetColor(tList->GetMainColor());
	       Float_t rgba[4] = { 1, 0, 0, 1 };
	       if (c) {
		    rgba[0] = c->GetRed();
		    rgba[1] = c->GetGreen();
		    rgba[2] = c->GetBlue();
	       }
	       extract2->SetRGBA(rgba);
	       extract2->SetRnrSelf(true);
	       extract2->SetRnrElements(true);
	       extract2->SetShape(sc_box);
	       container->AddElement(extract2);
	  }
#endif
     }
     tList->AddElement(TEveGeoShape::ImportShapeExtract(container, 0));
#endif
}

void 
ElectronsProxySCBuilder::buildRhoZ(const FWEventItem* iItem,
				   TEveElementList** product)
{
     printf("calling ElectronsProxySCBuilder::buildRhoZ\n");
     TEveElementList* tList = *product;
     if(0 == tList) {
	  tList =  new TEveElementList(iItem->name().c_str(),"Supercluster RhoZ",true);
	  *product = tList;
	  tList->SetMainColor(iItem->defaultDisplayProperties().color());
	  gEve->AddElement(tList);
     } else {
	  tList->DestroyElements();
     }
     // get electrons
#if 0
     using reco::PixelMatchGsfElectronCollection;
     const PixelMatchGsfElectronCollection *electrons = 0;
     printf("getting electrons\n");
     iItem->get(electrons);
     printf("got electrons\n");
     if (electrons == 0) {
	  std::cout <<"failed to get GSF electrons" << std::endl;
	  return;
     }
#else
     using reco::PixelMatchGsfElectronCollection;
     const PixelMatchGsfElectronCollection *electrons = 
	  ElectronsProxy3DBuilder::electrons;
#endif
     printf("%d GSF electrons\n", electrons->size());
     // get rechits
     const EcalRecHitCollection *hits = 0;
     printf("getting rechits\n");
     iItem->get(hits);
     printf("got rechits\n");
     if (hits == 0) {
	  std::cout <<"failed to get Ecal RecHits" << std::endl;
	  return;
     }
     printf("%d RecHits\n", hits->size());
     for (EcalRecHitCollection::const_iterator i = hits->begin();
	  i != hits->end(); ++i) {
	  TEveGeoShapeExtract* extract = m_item->getGeom()->getExtract(i->id().rawId() );
	  printf("extract is 0x%x\n", extract);
	  if(0!=extract) {
	       TEveTrans t = extract->GetTrans();
	       t.MoveLF(3, -i->energy() / 2);
	       TGeoBBox *sc_box = new TGeoBBox(1.1, 1.1, i->energy() / 2, 0);
	       TEveGeoShapeExtract *extract2 = new 
		    TEveGeoShapeExtract("SC");
	       extract2->SetTrans(t.Array());
	       TColor* c = gROOT->GetColor(tList->GetMainColor());
	       Float_t rgba[4] = { 1, 0, 0, 1 };
	       if (c) {
		    rgba[0] = c->GetRed();
		    rgba[1] = c->GetGreen();
		    rgba[2] = c->GetBlue();
	       }
	       extract2->SetRGBA(rgba);
	       extract2->SetRnrSelf(true);
	       extract2->SetRnrElements(true);
	       extract2->SetShape(sc_box);
	       tList->AddElement(TEveGeoShape::ImportShapeExtract(extract2,0));
#if 0
	       TGeoTrap *crystal = dynamic_cast<TGeoTrap *>(extract->GetShape());
	       assert(crystal != 0);
// 	       printf("%d\n", (char *)(&crystal->fH1) - (char *)crystal);
	       double *H1 = (double *)crystal + 30; // this is a kluge
	       printf("%f\n", *H1);
// 	       *H1++ = i->energy() / 10;
// 	       *H1++ = i->energy() / 10;
// 	       *H1++ = i->energy() / 10;
// 	       H1++;
// 	       *H1++ = i->energy() / 10;
// 	       *H1++ = i->energy() / 10;
// 	       *H1++ = i->energy() / 10;
	       TEveElement* shape = TEveGeoShape::ImportShapeExtract(extract,0);
	       shape->SetMainTransparency(50);
	       shape->SetMainColor(Color_t(kBlack + (int)floor(i->energy() + 10))); // tList->GetMainColor());
	       tList->AddElement(shape);
#endif
	  }
     }
}
