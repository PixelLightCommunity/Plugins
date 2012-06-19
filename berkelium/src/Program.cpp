//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "Program.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLScene;
using namespace AWESOMIUM;


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(Program)


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
Program::Program(Frontend &cFrontend) : EngineApplication(cFrontend),
	m_pBerkelium(nullptr),
	m_pAwesomium(nullptr),
	m_nBrowserRenderer(1) // 1 = Berkelium | 2 = Awesomium
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
	PLRenderer::FontTexture *pFontTexture = GetRenderer()->GetFontManager().GetFontTexture("SansationRegular.otf", 10U);
	GetRenderer()->GetFontManager().SetDefaultFontTexture(pFontTexture);

	if (m_nBrowserRenderer == 1)
		m_pBerkelium = new SRPBerkelium(*this, *GetRenderer(), 768, 768, "http://google.com", 10, 10);
	else if (m_nBrowserRenderer == 2)
		m_pAwesomium = new SRPAwesomium(*this, *GetRenderer(), 768, 768, "http://google.com", 10, 10);
	else
		DebugToConsole("!!!\tNO RENDERER CHOOSEN\t\t\t!!!\n!!!\t(check m_nBrowserRenderer in Program)\t!!!\n\n");

	SceneRenderer *pSceneRenderer = GetSceneRendererTool().GetSceneRenderer();
	if (pSceneRenderer)
	{
		if (m_pBerkelium) pSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(m_pBerkelium));
		if (m_pAwesomium) pSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(m_pAwesomium));
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