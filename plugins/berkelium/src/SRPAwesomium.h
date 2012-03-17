#pragma once


//[-------------------------------------------------------]
//[ Libraries                                             ]
//[-------------------------------------------------------]
#pragma comment(lib, "../../ext/awesomium/lib/Awesomium.lib")


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/System/System.h>
#include <PLCore/System/Console.h>
#include <PLScene/Compositing/SceneRendererPass.h>
#include <PLRenderer/Renderer/Renderer.h>
#include <PLRenderer/Renderer/DrawHelpers.h>
#include <PLRenderer/Renderer/VertexBuffer.h>
#include <PLRenderer/Renderer/VertexShader.h>
#include <PLRenderer/Renderer/FragmentShader.h>
#include <PLRenderer/Renderer/ProgramWrapper.h>
#include <PLRenderer/Renderer/ShaderLanguage.h>
#include <PLRenderer/Texture/TextureManager.h>
#include <PLRenderer/Renderer/TextureBuffer.h>
#include <PLRenderer/RendererContext.h>
#include <PLRenderer/Renderer/SamplerStates.h>
#include <PLMath/Vector2.h>
#include <PLMath/Rectangle.h>
#include <PLGraphics/Color/Color4.h>
#include <PLGraphics/Image/Image.h>
#include <PLGraphics/Image/ImageBuffer.h>
#include <PLInput/Input/InputManager.h>
#include <PLInput/Input/Controller.h>
#include <PLInput/Input/Controls/Button.h>
#include <PLInput/Input/Controls/Axis.h>
#include <PLEngine/Application/EngineApplication.h>

#include <Awesomium/awesomium_capi.h>

#include "ARGBtoRGBA_GLSL.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class SRPAwesomium : public PLScene::SceneRendererPass {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(pl_rtti_export, SRPAwesomium, "", PLScene::SceneRendererPass, "SRPAwesomium class")
		pl_slot_1(ControllerEvents, PLInput::Control&, "Called when a control event has occurred, occurred control as first parameter", "")
	pl_class_end


	public:
		SRPAwesomium(PLEngine::EngineApplication &cEngineApplication, PLRenderer::Renderer &cRenderer, int nWidth = 64, int nHeight = 64, PLCore::String sUrl = "about:blank", int nX = 0, int nY = 0, bool bAllowEvents = true);
		virtual ~SRPAwesomium();

		void UpdateAwesomium();
		bool IsActive();
		bool IsLoaded();
		//Berkelium::Window *GetWindow();
		//PLMath::Vector2 GetWindowSize();
		//PLMath::Vector2 GetWindowPosition();
		void ChangePointerState(bool bDrawPointer);
		void SetPointerImagePath(PLCore::String sPointerImagePath, bool bDrawPointer = true);
		void NavigateTo(PLCore::String sUrl);

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

		virtual void Draw(PLRenderer::Renderer &cRenderer, const PLScene::SQCull &cCullQuery) override;

		PLEngine::EngineApplication &m_cEngineApplication;
		PLRenderer::Renderer &m_cRenderer;
		PLRenderer::VertexBuffer *m_pVertexBuffer;
		PLMath::Vector2 m_vPosition;
		PLMath::Vector2 m_vImageSize;
		float m_fZValue2D;
		PLMath::Vector2 m_vTextureCoordinate;
		PLMath::Vector2 m_vTextureCoordinateSize;
		float m_fTextureCoordinateScaleX;
		float m_fTextureCoordinateScaleY;
		PLRenderer::VertexShader *m_pVertexShader;
		PLRenderer::FragmentShader *m_pFragmentShader;
		PLRenderer::ProgramWrapper *m_pProgram;
		int	m_nFrameWidth;
		int	m_nFrameHeight;
		PLCore::String m_sUrl;
		int m_nXPos;
		int m_nYPos;
		PLGraphics::Image m_cImage;
		PLRenderer::TextureBuffer *m_pTextureBuffer;
		PLCore::uint8 *m_pBufferData;
		bool m_bBufferReady;
		bool m_bAwesomiumCreated;
		//Berkelium::Window *m_pBerkeliumWindow;
		bool m_bNeedsFullUpdate;
		bool m_bIsActive;
		bool m_bLoaded;
		bool m_bLoading;
		bool m_bAllowEvents;
		PLInput::Controller *m_pController;
		int m_nMouseX;
		int m_nMouseY;
		bool m_bDrawPointer;
		PLCore::String m_sPointerImagePath;
		PLRenderer::Texture *m_pPointerTexture;
		PLRenderer::SamplerStates m_cPointerSamplerStates;

		awe_webview *m_pWebView;
		const awe_renderbuffer *m_pAwesomiumRenderBuffer;

		bool CreateVertexBuffer();
		bool InitShader();
		void CreateAwesomium();
		void CreateController();
		void ControllerEvents(PLInput::Control &cControl);
		void MouseEvents(PLInput::Control &cControl);
		void KeyEvents(PLInput::Control &cControl);
		void MouseMove(int nX, int nY);
		//void WindowResize(int nWidth, int nHeight, int nX = 0, int nY = 0, bool bAbsolute = false);
		//void WindowMove(int nX, int nY, bool bAbsolute = false);
		void DrawPointer(const PLMath::Vector2 &vPos);


};