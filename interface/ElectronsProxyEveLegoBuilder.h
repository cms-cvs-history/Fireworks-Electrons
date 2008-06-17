#ifndef Fireworks_Electron_ElectronsProxyEveLegoBuilder_h
#define Fireworks_Electron_ElectronsProxyEveLegoBuilder_h
// -*- C++ -*-
//
// Package:     Calo
// Class  :     ElectronsProxyEveLegoBuilder
// 
/**\class ElectronsProxyEveLegoBuilder ElectronsProxyEveLegoBuilder.h Fireworks/Calo/interface/ElectronsProxyEveLegoBuilder.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  
//         Created:  Sun Jan  6 23:42:33 EST 2008
// $Id: ElectronsProxyEveLegoBuilder.h,v 1.2 2008/06/09 19:54:03 chrjones Exp $
//

// system include files

// user include files
#include "Fireworks/Core/interface/FW3DLegoDataProxyBuilder.h"

// forward declarations
class ElectronsProxyEveLegoBuilder : public FW3DLegoDataProxyBuilder
{

   public:
      ElectronsProxyEveLegoBuilder();
      virtual ~ElectronsProxyEveLegoBuilder();

      // ---------- const member functions ---------------------
      REGISTER_PROXYBUILDER_METHODS();
   
      // ---------- static member functions --------------------
   private:
      virtual void build(const FWEventItem* iItem, 
			 TEveElementList** product);

      ElectronsProxyEveLegoBuilder(const ElectronsProxyEveLegoBuilder&); // stop default

      const ElectronsProxyEveLegoBuilder& operator=(const ElectronsProxyEveLegoBuilder&); // stop default

      // ---------- member data --------------------------------
};


#endif
