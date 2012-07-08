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
#define RESIZEWINDOW "ResizeWindow"


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
	Berkelium::Window *pWindow;						/**< Shared, points to SRPWindow::m_pProgramWrapper, do not free the memory */
	void *pReplyMsg;								/**< Free the resource if you no longer need it */
	Berkelium::URLString OriginUrl;
	PLCore::String sFunctionName;
	size_t nNumberOfParameters;
	Berkelium::Script::Variant *pParameters;		/**< Free the resource if you no longer need it */
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

		/** 
			The parameters in the following berkelium specific methods are named to be consistent with berkelium, see http://berkelium.org/class_berkelium_1_1_window_delegate.html
			The description and notes are also copied straight from the berkelium documentation.
		*/

		/**
		*  @brief
		*    The window is being painted.
		*
		*  @note
		*    You need to synchronously copy the buffer into application (video) memory before returning.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] const unsigned char * sourceBuffer
		*  @param[in] const Berkelium::Rect & sourceBufferRect
		*  @param[in] size_t numCopyRects
		*  @param[in] const Berkelium::Rect * copyRects
		*  @param[in] int dx
		*  @param[in] int dy
		*  @param[in] const Berkelium::Rect & scrollRect
		*
		*  @return
		*    void
		*/
		virtual void onPaint(Berkelium::Window *win, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);

		/**
		*  @brief
		*    A new window has already been created.
		*
		*  @note
		*    You are now expected to hold onto the instance and be responsible for deleting it.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::Window * newWindow
		*  @param[in] const Berkelium::Rect & initialRect
		*
		*  @return
		*    void
		*/
		virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow, const Berkelium::Rect &initialRect);

		/**
		*  @brief
		*    A page has loaded without error.
		*
		*  @param[in] Berkelium::Window * win
		*
		*  @return
		*    void
		*/
		virtual void onLoad(Berkelium::Window *win);

		/**
		*  @brief
		*    Notifies if a page is loading.
		*
		*  @note
		*    This callback is only useful for deciding whether to show a loading indicator,
		*    for example a spinning widget or maybe a capital letter being bombarded by meteors.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] bool isLoading
		*
		*  @return
		*    void
		*/
		virtual void onLoadingStateChanged(Berkelium::Window *win, bool isLoading);

		/**
		*  @brief
		*    A worker has crashed.
		*
		*  @remarks
		*    No info is provided yet to the callback.
		*
		*  @param[in] Berkelium::Window * win
		*
		*  @return
		*    void
		*/
		virtual void onCrashedWorker(Berkelium::Window *win);
		
		/**
		*  @brief
		*    Plugin with a given name has crashed.
		*
		*  @note
		*    There's currently no way to restart it, aside from maybe reloading the page that uses it.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::WideString pluginName
		*
		*  @return
		*    void
		*/
		virtual void onCrashedPlugin(Berkelium::Window *win, Berkelium::WideString pluginName);
		
		/**
		*  @brief
		*    Display a javascript message (Maybe an error?) or console.log.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::WideString message
		*  @param[in] Berkelium::WideString sourceId
		*  @param[in] int line_no
		*
		*  @return
		*    void
		*/
		virtual void onConsoleMessage(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString sourceId, int line_no);
		
		/**
		*  @brief
		*    Synchronously display an alert box (OK), confirm (OK, CANCEL), or a prompt (Input).
		*
		*  @note
		*    If prompt, optionally return a string in value.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::WideString message
		*  @param[in] Berkelium::WideString defaultValue
		*  @param[in] Berkelium::URLString url
		*  @param[in] int flags
		*  @param[in] bool & success
		*  @param[in] Berkelium::WideString & value
		*
		*  @return
		*    void
		*/
		virtual void onScriptAlert(Berkelium::Window *win, Berkelium::WideString message, Berkelium::WideString defaultValue, Berkelium::URLString url, int flags, bool &success, Berkelium::WideString &value);
		
		/**
		*  @brief
		*    A renderer instance crashed.
		*
		*  @note
		*    Applications can't currently do anything to handle this aside from recreating the Window object.
		*
		*  @param[in] Berkelium::Window * win
		*
		*  @return
		*    void
		*/
		virtual void onCrashed(Berkelium::Window *win);
		
		/**
		*  @brief
		*    A renderer instance is hung.
		*
		*  @note
		*    You can use this to display the Window in a grayed out state, and offer the user a choice to kill the Window.
		*
		*  @param[in] Berkelium::Window * win
		*
		*  @return
		*    void
		*/
		virtual void onUnresponsive(Berkelium::Window *win);
		
		/**
		*  @brief
		*    Renderer instance is back to normal.
		*
		*  @note
		*    Reset the color and close dialogs.
		*
		*  @param[in] Berkelium::Window * win
		*
		*  @return
		*    void
		*/
		virtual void onResponsive(Berkelium::Window *win);
		
		/**
		*  @brief
		*    The URL associated with this page has changed.
		*
		*  @note
		*    This is similar to onStartLoading, but may also be called if the hash part of the url changes.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::URLString newURL
		*
		*  @return
		*    void
		*/
		virtual void onAddressBarChanged(Berkelium::Window *win, Berkelium::URLString newURL);
		
		/**
		*  @brief
		*    Javascript has called a bound function on this Window.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] void * replyMsg
		*  @param[in] Berkelium::URLString origin
		*  @param[in] Berkelium::WideString funcName
		*  @param[in] Berkelium::Script::Variant * args
		*  @param[in] size_t numArgs
		*
		*  @return
		*    void
		*/
		virtual void onJavascriptCallback(Berkelium::Window *win, void *replyMsg, Berkelium::URLString origin, Berkelium::WideString funcName, Berkelium::Script::Variant *args, size_t numArgs);
		
		/**
		*  @brief
		*    Display a tooltip at the mouse cursor position.
		*
		*  @note
		*    There's no notification for destroying the tooltip afaik
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::WideString text
		*
		*  @return
		*    void
		*/
		virtual void onTooltipChanged(Berkelium::Window *win, Berkelium::WideString text);
		
		/**
		*  @brief
		*    Display a file chooser dialog, if necessary.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] int mode
		*  @param[in] Berkelium::WideString title
		*  @param[in] Berkelium::FileString defaultFile
		*
		*  @return
		*    void
		*/
		virtual void onRunFileChooser(Berkelium::Window *win, int mode, Berkelium::WideString title, Berkelium::FileString defaultFile);
		
		/**
		*  @brief
		*    A widget is a rectangle to display on top of the page, e.g.
		*
		*  @remarks
		*    a context menu or a dropdown.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::Widget * newWidget
		*  @param[in] int zIndex
		*
		*  @return
		*    void
		*/
		virtual void onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex);
		
		/**
		*  @brief
		*    Called by Widget::destroy(), in its destructor.
		*
		*  @note
		*    This widget will be removed from the Window's list of widgets when this function returns.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::Widget * wid
		*
		*  @return
		*    void
		*/
		virtual void onWidgetDestroyed(Berkelium::Window *win, Berkelium::Widget *wid);
		
		/**
		*  @brief
		*    Widget has moved, Usually only happens once after creating.
		*
		*  @note
		*    While these coordinates may be ignored, the position usually corresponds to some click event or the mouse coordinates.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::Widget * wid
		*  @param[in] int newX
		*  @param[in] int newY
		*
		*  @return
		*    void
		*/
		virtual void onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY);
		
		/**
		*  @brief
		*    A widget overlay has been painted.
		*
		*  @note
		*    You need to synchronously copy the buffer into application (video) memory before returning.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::Widget * wid
		*  @param[in] const unsigned char * sourceBuffer
		*  @param[in] const Berkelium::Rect & sourceBufferRect
		*  @param[in] size_t numCopyRects
		*  @param[in] const Berkelium::Rect * copyRects
		*  @param[in] int dx
		*  @param[in] int dy
		*  @param[in] const Berkelium::Rect & scrollRect
		*
		*  @return
		*    void
		*/
		virtual void onWidgetPaint(Berkelium::Window *win, Berkelium::Widget *wid, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
		
		/**
		*  @brief
		*    Widget has changed size, Usually only happens once after creating.
		*
		*  @param[in] Berkelium::Window * win
		*  @param[in] Berkelium::Widget * wid
		*  @param[in] int newWidth
		*  @param[in] int newHeight
		*
		*  @return
		*    void
		*/
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
		PLCore::HashMap<PLCore::String, sCallBack*> *m_pmapDefaultCallBacks;
		PLCore::HashMap<PLCore::String, PLCore::DynFuncPtr> *m_pmapCallBackFunctions;
		bool m_bIgnoreBufferUpdate;
		PLCore::HashMap<Berkelium::Widget*, sWidget*> *m_pmapWidgets;


};


};


#endif // __PLBERKELIUM_SRPWINDOWS_H__