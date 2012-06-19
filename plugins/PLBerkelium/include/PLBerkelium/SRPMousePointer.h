#ifndef __PLBERKELIUM_SRPMOUSEPOINTER_H__
#define __PLBERKELIUM_SRPMOUSEPOINTER_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/System/System.h>
#include <PLCore/System/Console.h>
#include <PLScene/Compositing/SceneRendererPass.h>
#include <PLScene/Compositing/SceneRenderer.h>
#include <PLRenderer/Texture/TextureManager.h>
#include <PLRenderer/Renderer/SamplerStates.h>
#include <PLRenderer/Renderer/Renderer.h>
#include <PLRenderer/Renderer/DrawHelpers.h>
#include <PLRenderer/RendererContext.h>
#include <PLMath/Vector2.h>
#include <PLMath/Vector2i.h>

#include "PLBerkelium.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLBerkelium {


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class SRPMousePointer : public PLScene::SceneRendererPass {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(PLBERKELIUM_RTTI_EXPORT, SRPMousePointer, "PLBerkelium", PLScene::SceneRendererPass, "")
		pl_constructor_2(ParameterConstructor, PLRenderer::Renderer*, PLScene::SceneRenderer*, "", "")
	pl_class_end


	public:
		PLBERKELIUM_API SRPMousePointer(PLRenderer::Renderer *pRenderer, PLScene::SceneRenderer *pSceneRenderer);
		PLBERKELIUM_API virtual ~SRPMousePointer();

		PLBERKELIUM_API void SetVisible(const bool &bVisible);
		PLBERKELIUM_API bool IsVisible() const;
		PLBERKELIUM_API void SetPosition(const int &x, const int &y);
		PLBERKELIUM_API void DestroyInstance() const;
		PLBERKELIUM_API bool IsInitialized() const;
		PLBERKELIUM_API void MoveToFront();
		PLBERKELIUM_API bool ReInitialize(const PLCore::String &sPointerImagePath);
		PLBERKELIUM_API bool ChangePointerImage(const PLCore::String &sPointerImagePath, const bool &bVisible = true);
		PLBERKELIUM_API PLMath::Vector2i GetPosition() const;

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

		virtual void Draw(PLRenderer::Renderer &cRenderer, const PLScene::SQCull &cCullQuery) override;

		void DrawPointer(const PLMath::Vector2 &vPos);
		bool Initialize();

		PLScene::SceneRenderer *m_pCurrentSceneRenderer;
		PLRenderer::Renderer *m_pCurrentRenderer;
		bool m_bVisible;
		int m_nMouseX;
		int m_nMouseY;
		PLCore::String m_sPointerImagePath;
		PLRenderer::Texture *m_pPointerTexture;
		PLRenderer::SamplerStates m_cPointerSamplerStates;
		bool m_bInitialized;


};


};


#endif // __PLBERKELIUM_SRPMOUSEPOINTER_H__