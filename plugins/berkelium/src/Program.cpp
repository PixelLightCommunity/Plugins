//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "Program.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLScene;


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(Program)


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
Program::Program(Frontend &cFrontend) : EngineApplication(cFrontend),
	m_pBerkelium(nullptr),
	m_pAwesomium(nullptr)
{
	DebugToConsole("Program initialized..\n\n");
}


Program::~Program()
{
}


void Program::DebugToConsole(const String &sString)
{
	#ifdef _CONSOLE
		System::GetInstance()->GetConsole().Print(sString);
	#endif
}


void Program::OnInit()
{
	// Call base implementation
	EngineApplication::OnInit();
	//EngineApplication::LoadScene("");

	m_pBerkelium = new SRPBerkelium(*this, *GetRenderer(), 512, 512, "http://google.com", 10, 10);
	m_pAwesomium = new SRPAwesomium(*this, *GetRenderer(), 512, 512, "http://google.com", 512 + 20, 10);

	SceneRenderer *pSceneRenderer = GetSceneRendererTool().GetSceneRenderer();
	if (pSceneRenderer)
	{
		pSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(m_pBerkelium));
		pSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(m_pAwesomium));
	}
}


void Program::OnUpdate()
{
	// Call base implementation
	EngineApplication::OnUpdate();
	// Berkelium update
 	if (m_pBerkelium)
 		m_pBerkelium->UpdateBerkelium();
	// Awesomium update
	if (m_pAwesomium)
		m_pAwesomium->UpdateAwesomium();
}


void Program::OnDrop(const Container<String> &lstFiles)
{
 	if (m_pBerkelium)
 	{
 		m_pBerkelium->GetWindow()->navigateTo(lstFiles[0].GetASCII(), lstFiles[0].GetLength());
 	}
	if (m_pAwesomium)
	{
		m_pAwesomium->NavigateTo(lstFiles[0]);
	}
}