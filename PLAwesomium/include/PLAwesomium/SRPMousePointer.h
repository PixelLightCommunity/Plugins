#ifndef __PLAWESOMIUM_SRPMOUSEPOINTER_H__
#define __PLAWESOMIUM_SRPMOUSEPOINTER_H__
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

#include "PLAwesomium.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLAwesomium {


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class SRPMousePointer : public PLScene::SceneRendererPass {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(PLAWESOMIUM_RTTI_EXPORT, SRPMousePointer, "PLAwesomium", PLScene::SceneRendererPass, "")
		pl_constructor_2(ParameterConstructor, PLRenderer::Renderer*, PLScene::SceneRenderer*, "", "")
	pl_class_end


	public:
		PLAWESOMIUM_API SRPMousePointer(PLRenderer::Renderer *pRenderer, PLScene::SceneRenderer *pSceneRenderer);
		PLAWESOMIUM_API virtual ~SRPMousePointer();

		PLAWESOMIUM_API void SetVisible(const bool &bVisible);
		PLAWESOMIUM_API bool IsVisible() const;
		PLAWESOMIUM_API void SetPosition(const int &x, const int &y);
		PLAWESOMIUM_API void DestroyInstance() const;
		PLAWESOMIUM_API bool IsInitialized() const;
		PLAWESOMIUM_API void MoveToFront();
		PLAWESOMIUM_API bool ReInitialize(const PLCore::String &sPointerImagePath);
		PLAWESOMIUM_API bool ChangePointerImage(const PLCore::String &sPointerImagePath, const bool &bVisible = true);
		PLAWESOMIUM_API PLMath::Vector2i GetPosition() const;

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


#endif // __PLAWESOMIUM_SRPMOUSEPOINTER_H__