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
	m_pBerkelium(nullptr)
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

	m_pBerkelium = new SRPBerkelium(*this, *GetRenderer(), 768, 768, "http://google.com", 0, 0);
	SceneRenderer *pSceneRenderer = GetSceneRendererTool().GetSceneRenderer();
	if (pSceneRenderer)
		pSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(m_pBerkelium));
}


void Program::OnUpdate()
{
	// Call base implementation
	EngineApplication::OnUpdate();
	// Berkelium update
	if (m_pBerkelium)
		m_pBerkelium->UpdateBerkelium();
}


void Program::OnDrop(const Container<String> &lstFiles)
{
	if (m_pBerkelium)
	{
		m_pBerkelium->GetWindow()->navigateTo(lstFiles[0].GetASCII(), lstFiles[0].GetLength());
	}
}