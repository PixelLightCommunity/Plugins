#ifndef __PLBERKELIUM_SRPWINDOWS_H__
#define __PLBERKELIUM_SRPWINDOWS_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/System/System.h>
#include <PLCore/System/Console.h>
#include <PLCore/Core/MemoryManager.h>
#include <PLCore/Application/CoreApplication.h>
#include <PLCore/Frontend/FrontendApplication.h>
#include <PLCore/Base/Func/FuncGenMemPtr.h>
#include <PLScene/Compositing/SceneRendererPass.h>
#include <PLScene/Compositing/SceneRenderer.h>
#include <PLRenderer/Renderer/Renderer.h>
#include <PLRenderer/Renderer/VertexBuffer.h>
#include <PLRenderer/Renderer/ProgramWrapper.h>
#include <PLRenderer/Renderer/ProgramUniform.h>
#include <PLRenderer/Renderer/ShaderLanguage.h>
#include <PLRenderer/RendererContext.h>
#include <PLGraphics/Image/Image.h>
#include <PLGraphics/Image/ImageBuffer.h>
#include <PLMath/Vector2.h>
#include <PLMath/Vector2i.h>
#include <PLMath/Rectangle.h>
#include <PLMath/Matrix4x4.h>

#include "berkelium/Berkelium.hpp"
#include "berkelium/Context.hpp"
#include "berkelium/WindowDelegate.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/Widget.hpp"
#include "berkelium/ScriptUtil.hpp"

#include "ARGBtoRGBA_GLSL.h"

#include "PLBerkelium.h"


//[-------------------------------------------------------]
//[ Forward declarations                                  ]
//[-------------------------------------------------------]
namespace PLRenderer
{
	class VertexShader;
	class FragmentShader;
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLBerkelium {


//[-------------------------------------------------------]
//[ Defines                                               ]
//[-------------------------------------------------------]
#define DRAGWINDOW "DragWindow"
#define HIDEWINDOW "HideWindow"
#define CLOSEWINDOW "CloseWindow"


//[-------------------------------------------------------]
//[ Structures                                            ]
//[-------------------------------------------------------]
struct sWindowsData
{
	bool bIsVisable;
	PLCore::String sUrl;
	int nFrameWidth;
	int nFrameHeight;
	int nXPos;
	int nYPos;
	bool bTransparent;
	bool bKeyboardEnabled;
	bool bMouseEnabled;
	bool bNeedsFullUpdate;
	bool bLoaded;
};


struct sCallBack
{
	Berkelium::Window *pWindow;
	void *replyMsg;
	Berkelium::URLString origin;
	PLCore::String sFunctionName;
	size_t nNumberOfParameters;
	Berkelium::Script::Variant *pParameters;
};


struct sWidget
{
	PLRenderer::VertexBuffer *pVertexBuffer;		/**< Free the resource if you no longer need it */
	PLRenderer::ProgramWrapper *pProgramWrapper;	/**< Shared, points to SRPWindow::m_pProgramWrapper, do not free the memory */
	PLRenderer::TextureBuffer *pTextureBuffer;		/**< Free the resource if you no longer need it */
	PLGraphics::Image cImage;
	int nWidth;
	int nHeight;
	int nXPos;
	int nYPos;
	bool bNeedsFullUpdate;
};


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class SRPWindow : public PLScene::SceneRendererPass, public Berkelium::WindowDelegate {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(PLBERKELIUM_RTTI_EXPORT, SRPWindow, "PLBerkelium", PLScene::SceneRendererPass, "")
		pl_constructor_1(ParameterConstructor, const PLCore::String&, "", "")
	pl_class_end


	public:
		PLBERKELIUM_API SRPWindow(const PLCore::String &sName);
		PLBERKELIUM_API virtual ~SRPWindow();

		PLBERKELIUM_API void SetRenderer(PLRenderer::Renderer *pRenderer);
		PLBERKELIUM_API bool Initialize(PLRenderer::Renderer *pRenderer, const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		PLBERKELIUM_API void DestroyInstance() const;
		PLBERKELIUM_API Berkelium::Window *GetBerkeliumWindow() const;
		PLBERKELIUM_API void CreateBerkeliumWindow();
		PLBERKELIUM_API sWindowsData *GetData() const;
		PLBERKELIUM_API bool AddSceneRenderPass(PLScene::SceneRenderer *pSceneRenderer);
		PLBERKELIUM_API bool RemoveSceneRenderPass();
		PLBERKELIUM_API void MoveToFront();
		PLBERKELIUM_API PLCore::String GetName() const;
		PLBERKELIUM_API void DestroyBerkeliumWindow();
		PLBERKELIUM_API PLMath::Vector2i GetPosition() const;
		PLBERKELIUM_API PLMath::Vector2i GetSize() const;
		PLBERKELIUM_API PLMath::Vector2i GetRelativeMousePosition(const PLMath::Vector2i &vMousePos) const;
		PLBERKELIUM_API int GetSceneRenderPassIndex();
		PLBERKELIUM_API void MoveWindow(const int &nX, const int &nY);
		PLBERKELIUM_API void SetToolTip(const PLCore::String &sText);
		PLBERKELIUM_API void SetToolTipEnabled(const bool &bEnabled);
		PLBERKELIUM_API void ClearCallBacks() const;
		PLBERKELIUM_API PLCore::uint32 GetNumberOfCallBacks() const;
		PLBERKELIUM_API sCallBack *GetCallBack(const PLCore::String &sKey) const;
		PLBERKELIUM_API bool RemoveCallBack(const PLCore::String &sKey) const;
		PLBERKELIUM_API void ResizeWindow(const int &nWidth, const int &nHeight);
		PLBERKELIUM_API bool AddCallBackFunction(const PLCore::DynFuncPtr pDynFunc, PLCore::String sJSFunctionName = "",  bool bHasReturn = false);
		PLBERKELIUM_API PLCore::HashMap<Berkelium::Widget*, sWidget*> *GetWidgets() const;
		PLBERKELIUM_API PLMath::Vector2i GetRelativeMousePositionWidget(const sWidget *psWidget, const PLMath::Vector2i &vMousePos) const;
		PLBERKELIUM_API void ExecuteJavascript(const PLCore::String &sJavascript) const;
		PLBERKELIUM_API bool IsLoaded() const;

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

		virtual void Draw(PLRenderer::Renderer &cRenderer, const PLScene::SQCull &cCullQuery) override;

		virtual void onPaint(Berkelium::Window *win, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
		virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow, const Berkelium::Rect &initialRect);
		virtual void onLoad(Berkelium::Window *win);
		virtual void onLoadingStateChanged(Berkelium::Window *win, bool isLoading);
		virtual void onCrashedWorker(Berkelium::Window *win);
		virtual void onCrashedPlugin(Berkelium::Window *win, Berkelium::WideString pluginName);
		virtual void onConsoleMessage(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString sourceId, int line_no);
		virtual void onScriptAlert(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString defaultValue, Berkelium::URLString url, int flags, bool &success, Berkelium::WideString &value);
		virtual void onCrashed(Berkelium::Window *win);
		virtual void onUnresponsive(Berkelium::Window *win);
		virtual void onResponsive(Berkelium::Window *win);
		virtual void onAddressBarChanged(Berkelium::Window *win, Berkelium::URLString newURL);
		virtual void onJavascriptCallback(Berkelium::Window *win, void *replyMsg, Berkelium::URLString origin, Berkelium::WideString funcName, Berkelium::Script::Variant *args, size_t numArgs);
		virtual void onTooltipChanged(Berkelium::Window *win, Berkelium::WideString text);
		virtual void onRunFileChooser(Berkelium::Window *win, int mode, Berkelium::WideString title, Berkelium::FileString defaultFile);
		virtual void onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex);
		virtual void onWidgetDestroyed(Berkelium::Window *win, Berkelium::Widget *wid);
		virtual void onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY);
		virtual void onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
		virtual void onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth, int newHeight);

		PLRenderer::VertexBuffer *CreateVertexBuffer(const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		PLRenderer::ProgramWrapper *CreateProgramWrapper();	// Don't free the returned instance, pointer stored inside SRPWindow::m_pProgramWrapper
		bool UpdateVertexBuffer(PLRenderer::VertexBuffer *pVertexBuffer, const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		void DrawWindow();
		void BufferCopyFull(PLCore::uint8 *pImageBuffer, int &nWidth, int &nHeight, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect);
		void BufferCopyRects(PLCore::uint8 *pImageBuffer, int &nWidth, int &nHeight, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects);
		void BufferCopyScroll(PLCore::uint8 *pImageBuffer, int &nWidth, int &nHeight, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
		void BufferUploadToGPU();
		void RecreateWindow();
		void CreateBerkeliumContext();
		void DestroyContext();
		void SetWindowSettings();
		void SetupToolTipWindow();
		void DestroyToolTipWindow();
		void SetDefaultCallBackFunctions();
		void DrawWidget(sWidget *psWidget);
		void DrawWidgets();

		Berkelium::Window *m_pBerkeliumWindow;
		const PLCore::String m_sWindowName;
		PLScene::SceneRenderer *m_pCurrentSceneRenderer;
		PLRenderer::Renderer *m_pCurrentRenderer;
		PLRenderer::VertexBuffer *m_pVertexBuffer;
		PLRenderer::VertexShader *m_pVertexShader;
		PLRenderer::FragmentShader *m_pFragmentShader;
		PLRenderer::ProgramWrapper *m_pProgramWrapper;
		PLRenderer::TextureBuffer *m_pTextureBuffer;
		PLGraphics::Image m_cImage;
		sWindowsData *m_psWindowsData;
		bool m_bInitialized;
		bool m_bReadyToDraw;
		PLCore::String m_sLastKnownUrl;
		Berkelium::Context *m_pBerkeliumContext;
		SRPWindow *m_pToolTip;
		bool m_bToolTipEnabled;
		PLCore::HashMap<PLCore::String, sCallBack*> *m_pDefaultCallBacks;
		PLCore::HashMap<PLCore::String, PLCore::DynFuncPtr> *m_pCallBackFunctions;
		bool m_bIgnoreBufferUpdate;
		PLCore::HashMap<Berkelium::Widget*, sWidget*> *m_pWidgets;


};


};


#endif // __PLBERKELIUM_SRPWINDOWS_H__