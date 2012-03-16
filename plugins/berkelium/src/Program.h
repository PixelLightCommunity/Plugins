#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/System/System.h>
#include <PLCore/System/Console.h>
#include <PLEngine/Application/EngineApplication.h>
#include <PLScene/Compositing/SceneRenderer.h>

#include "SRPBerkelium.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class Program : public PLEngine::EngineApplication {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(pl_rtti_export, Program, "", PLEngine::EngineApplication, "Program class")
		pl_constructor_1(ParameterConstructor,	PLCore::Frontend&,	"Parameter constructor. Frontend this application instance is running in as first parameter.", "")
	pl_class_end


	public:
		Program(PLCore::Frontend &cFrontend);
		virtual ~Program();

	protected:
		virtual void OnInit() override;
		virtual void OnUpdate() override;
		virtual void OnDrop(const PLCore::Container<PLCore::String> &lstFiles);

	private:
		void DebugToConsole(const PLCore::String &sString);

		SRPBerkelium *m_pBerkelium;


};