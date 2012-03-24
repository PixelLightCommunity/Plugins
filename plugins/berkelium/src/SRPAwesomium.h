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
#include <PLScene/Scene/SceneContext.h>
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
		//************************************
		// Method:    SRPAwesomium
		// FullName:  SRPAwesomium::SRPAwesomium
		// Access:    public 
		// Returns:   
		// Qualifier:
		// @param:    PLEngine::EngineApplication & cEngineApplication
		// @param:    PLRenderer::Renderer & cRenderer
		// @param:    int nWidth
		// @param:    int nHeight
		// @param:    PLCore::String sUrl
		// @param:    int nX
		// @param:    int nY
		// @param:    bool bAllowEvents
		//************************************
		SRPAwesomium(PLEngine::EngineApplication &cEngineApplication, PLRenderer::Renderer &cRenderer, int nWidth = 64, int nHeight = 64, const PLCore::String &sUrl = "about:blank", int nX = 0, int nY = 0, bool bAllowEvents = true);
		virtual ~SRPAwesomium();

		// Updates Awesomium to draw and handle callbacks
		void UpdateAwesomium();
		// Is active and running
		bool IsActive();
		//TODO// Is the page loaded
		bool IsLoaded();
		// Gets the window size
		PLMath::Vector2 GetWindowSize();
		// Gets the position of the window
		PLMath::Vector2 GetWindowPosition();
		// Changes the state to draw the pointer
		void ChangePointerState(bool bDrawPointer);
		// Sets and changes the image of the pointer
		void SetPointerImagePath(const PLCore::String &sPointerImagePath, bool bDrawPointer = true);
		// Navigates to a given url
		void NavigateTo(const PLCore::String &sUrl);

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

		// Draws the buffers as part of PLScene::SceneRendererPass
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
		bool m_bBufferReady;
		bool m_bAwesomiumCreated;
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

		// Creates the vertex buffer for rendering
		bool CreateVertexBuffer();
		// Initializes the shader for color swapping
		bool InitShader();
		// Creates Awesomium for usage
		void CreateAwesomium();
		// Creates the controller for handling input
		void CreateController();
		// Handles base controller events
		void ControllerEvents(PLInput::Control &cControl);
		// Handles mouse events
		void MouseEvents(PLInput::Control &cControl);
		// Handles key events
		void KeyEvents(PLInput::Control &cControl);
		// Moves the mouse
		void MouseMove(int nX, int nY);
		// Resizing the window (position optional)
		void WindowResize(int nWidth, int nHeight, int nX = 0, int nY = 0, bool bAbsolute = false);
		// Moves the window to a given location
		void WindowMove(int nX, int nY, bool bAbsolute = false);
		// Draws the pointer on screen at given position
		void DrawPointer(const PLMath::Vector2 &vPos);


};