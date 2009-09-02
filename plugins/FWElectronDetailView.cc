// -*- C++ -*-
//
// Package:     Calo
// Class  :     FWElectronDetailView
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:
//         Created:  Sun Jan  6 23:57:00 EST 2008
// $Id: FWElectronDetailView.cc,v 1.32 2009/08/28 12:22:31 amraktad Exp $
//

#include "TLatex.h"
#include "TEveCalo.h"
#include "TEveStraightLineSet.h"

// user include files
#include "Fireworks/Electrons/plugins/FWElectronDetailView.h"
#include "Fireworks/Electrons/plugins/FWECALDetailViewJohannes.icc"
#include "Fireworks/Electrons/plugins/FWECALDetailViewLothar.icc"
#include "Fireworks/Electrons/plugins/FWECALDetailViewDave.icc"
#include "Fireworks/Core/interface/FWModelId.h"
#include "Fireworks/Core/interface/FWEventItem.h"

#include "DataFormats/EgammaReco/interface/BasicCluster.h"

//
// constructors and destructor
//
FWElectronDetailView::FWElectronDetailView()
{
}

FWElectronDetailView::~FWElectronDetailView()
{
}

//
// member functions
//
void FWElectronDetailView::build(const FWModelId &id, const reco::GsfElectron* iElectron, TEveWindowSlot* base)
{
   if(0==iElectron) return;
   TEveWindowTab* eveWindow = base->MakeTab();
   eveWindow->SetShowTitleBar(kFALSE);


   TEveWindow* ew;
   TEveWindowSlot* slot;
   TEveScene*      scene;
   TEveViewer*     viewer;
   TGVerticalFrame* ediFrame;


   // tab 1
   slot = eveWindow->NewSlot();
   ew = FWDetailViewBase::makePackViewer(slot, ediFrame, viewer, scene);
   ew->SetElementName("View A");
   FWECALDetailViewJohannes<reco::GsfElectron>* viewJohannes =  new  FWECALDetailViewJohannes<reco::GsfElectron>();
   viewJohannes->build(id, iElectron, ediFrame, scene, viewer);

   TEveCaloLego *lego = viewJohannes->getLego();
   addTrackPointsInCaloData( iElectron, viewJohannes->getSubdetId(), lego);
   drawCrossHair(iElectron, viewJohannes->getSubdetId(), lego, scene);
   makeExtraLegend(iElectron, viewJohannes->getTextCanvas()) ;
   scene->Repaint(true);
   viewer->GetGLViewer()->RequestDraw(TGLRnrCtx::kLODHigh);
   gEve->Redraw3D();

   // tab 2
   slot = eveWindow->NewSlot();
   ew = FWDetailViewBase::makePackViewer(slot, ediFrame, viewer, scene);
   ew->SetElementName("View B");
   FWECALDetailViewLothar<reco::GsfElectron>* viewLothar = new  FWECALDetailViewLothar<reco::GsfElectron>();
   viewLothar->build(id, iElectron, ediFrame, scene, viewer);

   // tab 3
   slot = eveWindow->NewSlot();
   ew = FWDetailViewBase::makePackViewer(slot, ediFrame, viewer, scene);
   ew->SetElementName("View C");
   FWECALDetailViewDave<reco::GsfElectron>* viewDave = new  FWECALDetailViewDave<reco::GsfElectron>();
   viewDave->build(id, iElectron, ediFrame, scene, viewer);


   // eveWindow->GetTab()->SetTab(1);
}


math::XYZPoint FWElectronDetailView::trackPositionAtCalo (const reco::GsfElectron &t)
{
   return t.TrackPositionAtCalo();
}

double FWElectronDetailView::deltaEtaSuperClusterTrackAtVtx (const reco::GsfElectron &t)
{
   return t.deltaEtaSuperClusterTrackAtVtx();
}

double FWElectronDetailView::deltaPhiSuperClusterTrackAtVtx (const reco::GsfElectron &t)
{
   return t.deltaPhiSuperClusterTrackAtVtx();
}

void
FWElectronDetailView::makeExtraLegend(const reco::GsfElectron *electron, TCanvas* textCanvas)
{
   textCanvas->cd();
   TLatex* latex = new TLatex(0.02, 0.970, "");
   latex->SetTextSize(0.06);

   float_t x = 0.02;
   float_t x2 = 0.52;
   float   y = 0.6;
   float fontsize = latex->GetTextSize()*0.6;

   latex->DrawLatex(x, y, "Electron specific Info:");
   y -= fontsize;

   latex->DrawLatex(x, y, "#color[2]{+} track outer helix extrapolation");
   y -= fontsize;
   latex->DrawLatex(x, y, "#color[4]{+} track inner helix extrapolation");
   y -= fontsize;

   // summary
   if (electron->charge() > 0)
      latex->DrawLatex(x, y, "charge = +1");
   else latex->DrawLatex(x, y, "charge = -1");
   y -= fontsize;


   // delta phi/eta in
   char din[128];
   sprintf(din, "#Delta#eta_{in} = %.3f",
           electron->deltaEtaSuperClusterTrackAtVtx());
   latex->DrawLatex(x, y, din);
   sprintf(din, "#Delta#varphi_{in} = %.3f",
           electron->deltaPhiSuperClusterTrackAtVtx());
   latex->DrawLatex(x2, y, din);
   y -= fontsize;

   // delta phi/eta out
   char dout[128];
   sprintf(dout, "#Delta#eta_{out} = %.3f",
           electron->deltaEtaSeedClusterTrackAtCalo());
   latex->DrawLatex(x, y, dout);
   sprintf(dout, "#Delta#varphi_{out} = %.3f",
           electron->deltaPhiSeedClusterTrackAtCalo());
   latex->DrawLatex(x2, y, dout);
   y -= 2*fontsize;
}

void
FWElectronDetailView::drawCrossHair (const reco::GsfElectron* i, int subdetId, TEveCaloLego *lego, TEveElementList* tList)
{
   double ymax = lego->GetPhiMax();
   double ymin = lego->GetPhiMin();
   double xmax = lego->GetEtaMax();
   double xmin = lego->GetEtaMin();

   // draw crosshairs for track intersections
   //
   {
      const double eta = i->superCluster()->seed()->position().eta() -
         i->deltaEtaSeedClusterTrackAtCalo();
      const double phi = i->superCluster()->seed()->position().phi() -
         i->deltaPhiSeedClusterTrackAtCalo();

      TEveStraightLineSet *trackpositionAtCalo = new TEveStraightLineSet("sc trackpositionAtCalo");
      if (subdetId == EcalBarrel)
      {
         trackpositionAtCalo->AddLine(eta, ymin, 0, eta, ymax, 0);
         trackpositionAtCalo->AddLine(xmin, phi, 0, xmax, phi, 0);
      }
      else if (subdetId == EcalEndcap)
      {
         TVector3 pos;
         pos.SetPtEtaPhi(i->superCluster()->seed()->position().rho(), eta, phi);
         trackpositionAtCalo->AddLine(pos.X(), ymin, 0, pos.X(), ymax, 0);
         trackpositionAtCalo->AddLine(xmin, pos.Y(), 0, xmax,pos.Y(),0);
      }
      trackpositionAtCalo->SetDepthTest(kFALSE);
      trackpositionAtCalo->SetPickable(kFALSE);
      trackpositionAtCalo->SetLineColor(kBlue);
      tList->AddElement(trackpositionAtCalo);
   }
   //
   // pin position
   //
   {
      TEveStraightLineSet *pinposition = new TEveStraightLineSet("pin position");
      Double_t eta = i->caloPosition().eta() - deltaEtaSuperClusterTrackAtVtx(*i);
      Double_t phi = i->caloPosition().phi() - deltaPhiSuperClusterTrackAtVtx(*i);

      if (subdetId == EcalBarrel)
      {
         pinposition->AddLine(eta, ymax, 0, eta, ymin, 0);
         pinposition->AddLine(xmin, phi, 0, xmax, phi, 0);
      }
      else if (subdetId == EcalEndcap)
      {
         TVector3 pos;
         pos.SetPtEtaPhi(i->caloPosition().rho(), eta, phi);
         pinposition->AddLine(pos.X(),ymin, 0, pos.X(), ymax, 0);
         pinposition->AddLine(xmin, pos.Y(), 0, xmax, pos.Y(), 0);
      }
      pinposition->SetDepthTest(kFALSE);
      pinposition->SetPickable(kFALSE);
      pinposition->SetLineColor(kRed);
      tList->AddElement(pinposition);
   }

   printf("TrackPositionAtCalo: %f %f\n",
          trackPositionAtCalo(*i).eta(), trackPositionAtCalo(*i).phi());
   printf("TrackPositionAtCalo: %f %f\n",
          i->superCluster()->seed()->position().eta() -
          i->deltaEtaSeedClusterTrackAtCalo(),
          i->superCluster()->seed()->position().phi() -
          i->deltaPhiSeedClusterTrackAtCalo());
   printf("TrackPositionInner: %f %f\n",
          i->caloPosition().eta() - deltaEtaSuperClusterTrackAtVtx(*i),
          i->caloPosition().phi() - deltaPhiSuperClusterTrackAtVtx(*i));
   printf("calo position %f, deltaEta %f, track position %f\n",
          i->caloPosition().eta(),
          deltaEtaSuperClusterTrackAtVtx(*i),
          trackPositionAtCalo(*i).eta());
}

Bool_t FWElectronDetailView::checkRange(Double_t &em, Double_t& eM, Double_t &pm, Double_t& pM,
                                       Double_t eta, Double_t phi)
{
   Bool_t changed = kFALSE;

   //check eta
   if (eta < em)
   {
      em = eta;
      changed = kTRUE;
   }
   else if (eta > eM)
   {
      eM = eta;
      changed = kTRUE;
   }

   // check phi
   if (phi < pm)
   {
      pm = phi;
      changed = kTRUE;
   }
   else if (phi > pM)
   {
      pM = phi;
      changed = kTRUE;
   }
   return changed;
}

void
FWElectronDetailView::addTrackPointsInCaloData (const reco::GsfElectron *i, int subdetId, TEveCaloLego* lego)
{
   TEveCaloDataVec* data = (TEveCaloDataVec*)lego->GetData();
   Double_t em, eM, pm, pM;
   data->GetEtaLimits(em, eM);
   data->GetPhiLimits(pm, pM);

   Bool_t changed = kFALSE;
   // add cells in third layer if necessary

   //   trackpositionAtCalo
   {
      double eta = i->superCluster()->seed()->position().eta() -
         i->deltaEtaSeedClusterTrackAtCalo();
      double phi = i->superCluster()->seed()->position().phi() -
         i->deltaPhiSeedClusterTrackAtCalo();

      if (subdetId == EcalBarrel)
      {
         if (checkRange(em, eM, pm, pM, eta, phi))
            changed = kTRUE;
      }
      else if (subdetId == EcalEndcap) {
         TVector3 pos;
         pos.SetPtEtaPhi(i->superCluster()->seed()->position().rho(),eta, phi);
         if (checkRange(em, eM, pm, pM, pos.X(), pos.Y()))
            changed = kTRUE;

      }
   }
   // pinposition
   {
      double eta = i->caloPosition().eta() - deltaEtaSuperClusterTrackAtVtx(*i);
      double phi = i->caloPosition().phi() - deltaPhiSuperClusterTrackAtVtx(*i);
      if (subdetId == EcalBarrel)
      {
         if (checkRange(em, eM, pm, pM, eta, phi))
            changed = kTRUE;
      }
      else if (subdetId == EcalEndcap) {
         TVector3 pos;
         pos.SetPtEtaPhi(i->caloPosition().rho(), eta, phi);
         if (checkRange(em, eM, pm, pM, pos.X(), pos.Y()))
            changed = kTRUE;
      }
   }
   if (changed)
   {
      data->AddTower(em, eM, pm, pM);
      data->FillSlice(2, 0);   data->DataChanged();

      lego->ComputeBBox();
      Double_t legoScale = ((eM - em) < (pM - pm)) ? (eM - em) : (pM - pm);
      lego->InitMainTrans();
      lego->RefMainTrans().SetScale(legoScale, legoScale, legoScale*0.5);
      lego->RefMainTrans().SetPos((eM+em)*0.5, (pM+pm)*0.5, 0);
      lego->ElementChanged(true);
   }
}

REGISTER_FWDETAILVIEW(FWElectronDetailView);
