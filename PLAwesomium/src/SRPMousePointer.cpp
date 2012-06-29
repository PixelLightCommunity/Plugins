//[-------------------------------------------------------]
//[ Header                                                ]
//[-------------------------------------------------------]
#include "PLAwesomium/SRPMousePointer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
using namespace PLCore;
using namespace PLRenderer;
using namespace PLScene;
using namespace PLMath;

namespace PLAwesomium {


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
	// check if the mouse pointer is initialized
	if (m_bInitialized)
	{
		// remove the scene render pass
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
		// if the pointer is visible only then do we draw it
		DrawPointer(Vector2(float(m_nMouseX - 4), float(m_nMouseY + 2)));
	}
}


void SRPMousePointer::DrawPointer(const Vector2 &vPos)
{
	if (m_bInitialized)
	{
		if (m_pPointerTexture->GetTextureBuffer())
		{
			// set the render state to allow for transparency
			m_pCurrentRenderer->SetRenderState(RenderState::BlendEnable, true);
			// make sure we are in 2D mode
			m_pCurrentRenderer->GetDrawHelpers().Begin2DMode(0.0f, 0.0f, 0.0f, 0.0f);
			// draw the image
			m_pCurrentRenderer->GetDrawHelpers().DrawImage(*m_pPointerTexture->GetTextureBuffer(), m_cPointerSamplerStates, vPos);
			// exit 2D mode
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
		// move the scene render pass to front
		m_pCurrentSceneRenderer->MoveElement(m_pCurrentSceneRenderer->GetIndex(*reinterpret_cast<SceneRendererPass*>(this)), m_pCurrentSceneRenderer->GetNumOfElements() - 1);
	}
}


bool SRPMousePointer::Initialize()
{
	// set the texture
	m_pPointerTexture = m_pCurrentRenderer->GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath);

	if (m_pPointerTexture)
	{
		// we add the scene render pass
		if (m_pCurrentSceneRenderer->Add(*reinterpret_cast<SceneRendererPass*>(this)))
		{
			m_bInitialized = true;
			return true;
		}
	}
	// initialization has failed because the texture could not be created
	return false;
}


bool SRPMousePointer::ReInitialize(const String &sPointerImagePath)
{
	// allow for the user to reinitialize if failed
	m_sPointerImagePath = sPointerImagePath;
	return Initialize();
}


bool SRPMousePointer::ChangePointerImage(const String &sPointerImagePath, const bool &bVisible)
{
	if (m_bInitialized)
	{
		// hide the pointer because we are changing it
		SetVisible(false);
		m_sPointerImagePath = sPointerImagePath;

		// set the new texture
		m_pPointerTexture = m_pCurrentRenderer->GetRendererContext().GetTextureManager().LoadResource(m_sPointerImagePath);
		if (m_pPointerTexture)
		{
			// set the visibility status before we return
			SetVisible(bVisible);
			return true;
		}
		else
		{
			// initialization failed because the texture could not be created
			m_bInitialized = false;
		}
	}
	return false;
}


bool SRPMousePointer::IsInitialized() const
{
	return m_bInitialized;
}


void SRPMousePointer::DestroyInstance() const
{
	// cleanup
	delete this;
}


Vector2i SRPMousePointer::GetPosition() const
{
	return Vector2i(m_nMouseX, m_nMouseY);
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLAwesomium