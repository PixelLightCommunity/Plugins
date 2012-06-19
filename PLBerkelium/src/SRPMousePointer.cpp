//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "PLBerkelium/SRPMousePointer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLRenderer;
using namespace PLScene;
using namespace PLMath;

namespace PLBerkelium {


//[-------------------------------------------------------]
//[ RTTI interface                                        ]
//[-------------------------------------------------------]
pl_implement_class(SRPMousePointer)


//[-------------------------------------------------------]
//[ Functions		                                      ]
//[-------------------------------------------------------]
SRPMousePointer::SRPMousePointer(Renderer *pRenderer, SceneRenderer *pSceneRenderer) :
	m_pCurrentSceneRenderer(pSceneRenderer),
	m_pCurrentRenderer(pRenderer),
	m_bVisible(false),
	m_nMouseX(0),
	m_nMouseY(0),
	m_sPointerImagePath("Pointer.png"),
	m_pPointerTexture(nullptr),
	m_cPointerSamplerStates(SamplerStates()),
	m_bInitialized(false)
{
	Initialize();
}


SRPMousePointer::~SRPMousePointer()
{
	DebugToConsole("Terminating..\n");
	if (m_bInitialized)
	{
		m_pCurrentSceneRenderer->Remove(*reinterpret_cast<SceneRendererPass*>(this));
	}
}


void SRPMousePointer::DebugToConsole(const String &sString)
{
	System::GetInstance()->GetConsole().Print("PLBerkelium::SRPMousePointer - " + sString);
}


void SRPMousePointer::Draw(Renderer &cRenderer, const SQCull &cCullQuery)
{
	if (m_bVisible)
	{
		DrawPointer(Vector2(float(m_nMouseX - 4), float(m_nMouseY + 2)));
	}
}


void SRPMousePointer::DrawPointer(const Vector2 &vPos)
{
	if (m_bInitialized)
	{
		if (m_pPointerTexture->GetTextureBuffer())
		{
			m_pCurrentRenderer->SetRenderState(RenderState::BlendEnable, true);
			m_pCurrentRenderer->GetDrawHelpers().Begin2DMode(0.0f, 0.0f, 0.0f, 0.0f);
			m_pCurrentRenderer->GetDrawHelpers().DrawImage(*m_pPointerTexture->GetTextureBuffer(), m_cPointerSamplerStates, vPos);
			m_pCurrentRenderer->GetDrawHelpers().End2DMode();
		}
	}
}


void SRPMousePointer::SetVisible(const bool &bVisible)
{
	m_bVisible = bVisible;
}


bool SRPMousePointer::IsVisible() const
{
	return m_bVisible;
}


void SRPMousePointer::SetPosition(const int &x, const int &y)
{
	m_nMouseX = x;
	m_nMouseY = y;
}


void SRPMousePointer::MoveToFront()
{
	if (m_bInitialized)
	{
		m_pCurrentSceneRenderer->MoveElement(m_pCurrentSceneRenderer->GetIndex(*reinterpret_cast<SceneRendererPass*>(this)), m_pCurrentSceneRenderer->GetNumOfElements() - 1);
	}
}


bool SRPMousePointer::Initialize()
{
	DebugToConsole("Initializing..\n");

	m_pPointerTexture = m_pCurrentRenderer->GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath);
	if (m_pPointerTexture)
	{
		if (m_pCurrentSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(this)))
		{
			m_bInitialized = true;
			DebugToConsole("Initialization complete..\n");
			return true;
		}
	}
	DebugToConsole("Initialization failed..\n");
	return false;
}


bool SRPMousePointer::ReInitialize(const String &sPointerImagePath)
{
	m_sPointerImagePath = sPointerImagePath;
	return Initialize();
}


bool SRPMousePointer::ChangePointerImage(const String &sPointerImagePath, const bool &bVisible)
{
	if (m_bInitialized)
	{
		SetVisible(false);
		m_sPointerImagePath = sPointerImagePath;
		m_pPointerTexture = m_pCurrentRenderer->GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath);
		if (m_pPointerTexture)
		{
			m_bInitialized = true;
			SetVisible(bVisible);
			return true;
		}
		m_bInitialized = false;
	}
	return false;
}


bool SRPMousePointer::IsInitialized() const
{
	return m_bInitialized;
}


void SRPMousePointer::DestroyInstance() const
{
	delete this;
}


Vector2i SRPMousePointer::GetPosition() const
{
	return Vector2i(m_nMouseX, m_nMouseY);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLBerkelium