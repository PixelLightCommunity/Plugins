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
		SRPBerkelium(PLEngine::EngineApplication &cEngineApplication, PLRenderer::Renderer &cRenderer, int nWidth = 64, int nHeight = 64, PLCore::String sUrl = "about:blank", int nX = 0, int nY = 0, bool bAllowEvents = true);
		virtual ~SRPBerkelium();

		void UpdateBerkelium();
		bool IsActive();
		bool IsLoaded();
		Berkelium::Window *GetWindow();
		PLMath::Vector2 GetWindowSize();
		PLMath::Vector2 GetWindowPosition();

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

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

		bool CreateVertexBuffer();
		bool InitShader();
		void CreateBerkelium();
		void CreateController();
		void ControllerEvents(PLInput::Control &cControl);
		void MouseEvents(PLInput::Control &cControl);
		void MouseMove(int nX, int nY);
		void DrawDebugBox(int nX, int nY, int nWidth, int nHeight);
		void WindowResize(int nWidth, int nHeight, int nX = 0, int nY = 0, bool bAbsolute = false);
		void WindowMove(int nX, int nY, bool bAbsolute = false);


};