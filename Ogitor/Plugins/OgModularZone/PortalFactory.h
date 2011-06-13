#pragma once

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "NodeEditor.h"

using namespace Ogitors;

namespace MZP
{
	class PortalFactory : public CNodeEditorFactory
	{
	public:
		PortalFactory(OgitorsView *view = 0);
		virtual ~PortalFactory(void){};

		/** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        
		/** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbMarker.mesh";};

		virtual CBaseEditorFactory* duplicate(OgitorsView *view);
	};
}
