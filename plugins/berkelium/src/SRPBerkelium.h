#pragma once


//[-------------------------------------------------------]
//[ Libraries                                             ]
//[-------------------------------------------------------]
#pragma comment(lib, "../../ext/berkelium-11-win32/lib/berkelium.lib")


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/System/System.h>
#include <PLCore/System/Console.h>
#include <PLCore/Core/MemoryManager.h>
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

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/Widget.hpp"
#include "berkelium/WindowDelegate.hpp"
#include "berkelium/Context.hpp"
#include "berkelium/Rect.hpp"
#include "berkelium/ScriptUtil.hpp"

#include "ARGBtoRGBA_GLSL.h"


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class SRPBerkelium : public PLScene::SceneRendererPass, public Berkelium::WindowDelegate {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(pl_rtti_export, SRPBerkelium, "", PLScene::SceneRendererPass, "SRPBerkelium class")
		pl_slot_1(ControllerEvents, PLInput::Control&, "Called when a control event has occurred, occurred control as first parameter", "")
	pl_class_end


	public:
		//************************************
		// Method:    SRPBerkelium
		// FullName:  SRPBerkelium::SRPBerkelium
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
		SRPBerkelium(PLEngine::EngineApplication &cEngineApplication, PLRenderer::Renderer &cRenderer, int nWidth = 64, int nHeight = 64, const PLCore::String &sUrl = "about:blank", int nX = 0, int nY = 0, bool bAllowEvents = true);
		virtual ~SRPBerkelium();

		// Updates Berkelium to draw and handle callbacks
		void UpdateBerkelium();
		// Is active and running
		bool IsActive();
		// Is the page loaded
		bool IsLoaded();
		// Gets the window
		Berkelium::Window *GetWindow();
		// Gets the window size
		PLMath::Vector2 GetWindowSize();
		// Gets the position of the window
		PLMath::Vector2 GetWindowPosition();
		// Changes the state to draw the pointer
		void ChangePointerState(bool bDrawPointer);
		// Sets and changes the image of the pointer
		void SetPointerImagePath(const PLCore::String &sPointerImagePath, bool bDrawPointer = true);

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString, bool bTitle = true);

		// Draws the buffers as part of PLScene::SceneRendererPass
		virtual void Draw(PLRenderer::Renderer &cRenderer, const PLScene::SQCull &cCullQuery) override;

		virtual void onPaint(Berkelium::Window* wini, const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect, size_t num_copy_rects, const Berkelium::Rect* copy_rects, int dx, int dy, const Berkelium::Rect& scroll_rect);
		virtual void onStartLoading(Berkelium::Window *win, Berkelium::URLString newURL);
		virtual void onNavigationRequested(Berkelium::Window *win, Berkelium::URLString newUrl, Berkelium::URLString referrer, bool isNewWindow, bool &cancelDefaultAction);
		virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow, const Berkelium::Rect &initialRect);
		virtual void onLoad(Berkelium::Window *win);
		virtual void onLoadingStateChanged(Berkelium::Window *win, bool isLoading);
		virtual void onUnresponsive(Berkelium::Window *win);
		virtual void onResponsive(Berkelium::Window *win);
		virtual void onJavascriptCallback(Berkelium::Window *win, void* replyMsg, Berkelium::URLString origin, Berkelium::WideString funcName, Berkelium::Script::Variant *args, size_t numArgs);
		virtual	void onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
		virtual void onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex);
		virtual void onTooltipChanged(Berkelium::Window *win, Berkelium::WideString text);
		virtual void onConsoleMessage(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString sourceId, int line_no);

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
		bool m_bBerkeliumCreated;
		Berkelium::Window *m_pBerkeliumWindow;
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

		// Creates the vertex buffer for rendering
		bool CreateVertexBuffer();
		// Initializes the shader for color swapping
		bool InitShader();
		// Creates Berkelium for usage
		void CreateBerkelium();
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