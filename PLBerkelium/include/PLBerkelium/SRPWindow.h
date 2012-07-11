#ifndef __PLBERKELIUM_SRPWINDOWS_H__
#define __PLBERKELIUM_SRPWINDOWS_H__
#pragma once

// SRPWindow verified [12-juli-2012 at 01:11 by Icefire]


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
#include <PLRenderer/Renderer/VertexShader.h>
#include <PLRenderer/Renderer/FragmentShader.h>
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
		
		/**
		*  @brief
		*    Initializes window and creates needed components
		*
		*  @param[in] PLRenderer::Renderer * pRenderer
		*  @param[in] const PLMath::Vector2 & vPosition
		*  @param[in] const PLMath::Vector2 & vImageSize
		*
		*  @return
		*    'true' if window initialization is successful, else 'false'
		*/
		PLBERKELIUM_API bool Initialize(PLRenderer::Renderer *pRenderer, const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		
		/**
		*  @brief
		*    Destroys this window instance
		*
		*  @remarks
		*    This will also destroy all data and components created by this window.
		*/
		PLBERKELIUM_API void DestroyInstance() const;
		
		/**
		*  @brief
		*    Returns the berkelium window instance
		*
		*  @remarks
		*    You can use this to call berkelium specific methods.
		*
		*  @return
		*    pointer to berkelium window (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API Berkelium::Window *GetBerkeliumWindow() const;
		
		/**
		*  @brief
		*    Creates and sets the berkelium window instance
		*
		*  @remarks
		*    If the berkelium window is already set this will not do anything.
		*
		*  @return
		*    void
		*/
		PLBERKELIUM_API void CreateBerkeliumWindow();
		
		/**
		*  @brief
		*    Returns the data of the window
		*
		*  @remarks
		*    You can use this to get and set window data.
		*
		*  @return
		*    pointer to window data (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API sWindowsData *GetData() const;
		
		/**
		*  @brief
		*    Adds the SceneRenderPass for this window to the SceneRenderer
		*
		*  @param[in] PLScene::SceneRenderer * pSceneRenderer
		*
		*  @return
		*    'true' if the SceneRenderPass was added, else 'false'
		*/
		PLBERKELIUM_API bool AddSceneRenderPass(PLScene::SceneRenderer *pSceneRenderer);
		
		/**
		*  @brief
		*    Removes the SceneRenderPass for this window from the SceneRenderer
		*
		*  @return
		*    'true' if the SceneRenderPass was removed, else 'false'
		*/
		PLBERKELIUM_API bool RemoveSceneRenderPass();
		
		/**
		*  @brief
		*    Moves this window to front
		*/
		PLBERKELIUM_API void MoveToFront();
		
		/**
		*  @brief
		*    Returns the name of this window
		*
		*  @return
		*    window name
		*/
		PLBERKELIUM_API PLCore::String GetName() const;
		
		/**
		*  @brief
		*    Destroys the berkelium window instance
		*
		*  @remarks
		*    If the berkelium window has not been set this will not do anything.
		*/
		PLBERKELIUM_API void DestroyBerkeliumWindow();
		
		/**
		*  @brief
		*    Returns the position of this window
		*
		*  @return
		*    window position
		*/
		PLBERKELIUM_API PLMath::Vector2i GetPosition() const;
		
		/**
		*  @brief
		*    Returns the size of this window
		*
		*  @return
		*    window size
		*/
		PLBERKELIUM_API PLMath::Vector2i GetSize() const;
		
		/**
		*  @brief
		*    Returns the mouse positions relative to this window
		*
		*  @param[in] const PLMath::Vector2i & vMousePos
		*
		*  @return
		*    mouse position relative to window
		*/
		PLBERKELIUM_API PLMath::Vector2i GetRelativeMousePosition(const PLMath::Vector2i &vMousePos) const;
		
		/**
		*  @brief
		*    Returns the SceneRenderPass index for this window
		*
		*  @return
		*    SceneRenderPass index
		*/
		PLBERKELIUM_API int GetSceneRenderPassIndex();
		
		/**
		*  @brief
		*    Moves window to given position
		*
		*  @param[in] const int & nX
		*  @param[in] const int & nY
		*/
		PLBERKELIUM_API void MoveWindow(const int &nX, const int &nY);
		PLBERKELIUM_API void SetToolTip(const PLCore::String &sText);
		PLBERKELIUM_API void SetToolTipEnabled(const bool &bEnabled);
		
		/**
		*  @brief
		*    Removes all javascript callbacks for this window
		*
		*  @remarks
		*    This can be used to make sure default callbacks are not called parallel to one and other.
		*/
		PLBERKELIUM_API void RemoveCallBacks() const;
		
		/**
		*  @brief
		*    Returns the amount of javascript callbacks created by this window
		*
		*  @return
		*    amount of callbacks
		*/
		PLBERKELIUM_API PLCore::uint32 GetNumberOfCallBacks() const;
		
		/**
		*  @brief
		*    Returns a javascript callback by key identifier
		*
		*  @param[in] const PLCore::String & sKey
		*
		*  @return
		*    pointer to callback (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API sCallBack *GetCallBack(const PLCore::String &sKey) const;
		
		/**
		*  @brief
		*    Removes a javascript callback by key identifier
		*
		*  @param[in] const PLCore::String & sKey
		*
		*  @return
		*    'true' if the javascript callback was removed, else 'false'
		*/
		PLBERKELIUM_API bool RemoveCallBack(const PLCore::String &sKey) const;
		
		/**
		*  @brief
		*    Resizes window to given size
		*
		*  @note
		*    Its not advisable to resize a window smaller than 4x4 pixels.
		*
		*  @param[in] const int & nWidth
		*  @param[in] const int & nHeight
		*/
		PLBERKELIUM_API void ResizeWindow(const int &nWidth, const int &nHeight);
		
		/**
		*  @brief
		*    Adds and sets a Javascript callback method for this window
		*
		*  @remarks
		*    You can add c++ methods that javascript can callback to.
		*
		*  @note
		*    The method you want to callback to needs to be defined as a RTTI method.
		*    If you do not define the second parameter (sJSFunctionName), the method name will be used instead.
		*    Set the third parameter to true (bHasReturn) if you want to return anything but make sure the method returns a string, anything else is simply not supported.
		*
		*  @param[in] const PLCore::DynFuncPtr pDynFunc
		*  @param[in] PLCore::String sJSFunctionName
		*  @param[in] bool bHasReturn
		*
		*  @return
		*    'true' if the method was added, else 'false'
		*/
		PLBERKELIUM_API bool AddCallBackFunction(const PLCore::DynFuncPtr pDynFunc, PLCore::String sJSFunctionName = "",  bool bHasReturn = false);
		
		/**
		*  @brief
		*    Returns the widgets
		*
		*  @return
		*    pointer to hashmap containing widgets (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API PLCore::HashMap<Berkelium::Widget*, sWidget*> *GetWidgets() const;
		
		/**
		*  @brief
		*    Returns the mouse position relative to a widget
		*
		*  @param[in] const sWidget * psWidget
		*  @param[in] const PLMath::Vector2i & vMousePos
		*
		*  @return
		*    mouse position relative to widget
		*/
		PLBERKELIUM_API PLMath::Vector2i GetRelativeMousePositionWidget(const sWidget *psWidget, const PLMath::Vector2i &vMousePos) const;
		
		/**
		*  @brief
		*    Executes javascript on this window
		*
		*  @remarks
		*    Its not needed to prefix the string with 'javascript:'.
		*
		*  @param[in] const PLCore::String & sJavascript
		*/
		PLBERKELIUM_API void ExecuteJavascript(const PLCore::String &sJavascript) const;
		
		/**
		*  @brief
		*    Returns whether or not the page of this window finished loading
		*
		*  @return
		*    'true' if the page has loaded, else 'false'
		*/
		PLBERKELIUM_API bool IsLoaded() const;
		
		/**
		*  @brief
		*    Returns the image of this window
		*
		*  @remarks
		*    You can for example use this to save or use the image of this window somewhere else.
		*
		*  @return
		*    image of this window
		*/
		PLBERKELIUM_API PLGraphics::Image GetImage() const;

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

		virtual void Draw(PLRenderer::Renderer &cRenderer, const PLScene::SQCull &cCullQuery) override;

		/** 
			The parameters in the following berkelium specific methods are named to be consistent with berkelium, see http://berkelium.org/class_berkelium_1_1_window_delegate.html
			The description and notes are also copied straight from the berkelium documentation.
		*/

		#pragma region berkelium on event methods

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
		*/
		virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow, const Berkelium::Rect &initialRect);

		/**
		*  @brief
		*    A page has loaded without error.
		*
		*  @param[in] Berkelium::Window * win
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
		*/
		virtual void onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth, int newHeight);

		#pragma endregion berkelium on event methods

		/**
		*  @brief
		*    Sets the renderer for future usage
		*
		*  @param[in] PLRenderer::Renderer * pRenderer
		*/
		void SetRenderer(PLRenderer::Renderer *pRenderer);
		
		/**
		*  @brief
		*    Creates a vertex buffer with given dimensions
		*
		*  @param[in] const PLMath::Vector2 & vPosition
		*  @param[in] const PLMath::Vector2 & vImageSize
		*
		*  @return
		*    pointer to created vertex buffer (can be a null pointer, destroy the returned instance when you no longer need it)
		*/
		PLRenderer::VertexBuffer *CreateVertexBuffer(const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		
		/**
		*  @brief
		*    Creates program wrapper
		*
		*  @return
		*    pointer to created or already existing program wrapper (can be a null pointer, do not destroy the returned instance!)
		*/
		PLRenderer::ProgramWrapper *CreateProgramWrapper();
		
		/**
		*  @brief
		*    Updates given vertex buffer with given dimensions
		*
		*  @param[in] PLRenderer::VertexBuffer * pVertexBuffer
		*  @param[in] const PLMath::Vector2 & vPosition
		*  @param[in] const PLMath::Vector2 & vImageSize
		*
		*  @return
		*    'true' if vertex buffer was updated, else 'false'
		*/
		bool UpdateVertexBuffer(PLRenderer::VertexBuffer *pVertexBuffer, const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		
		/**
		*  @brief
		*    Draws the window on screen
		*/
		void DrawWindow();
		
		/**
		*  @brief
		*    Copies the buffer data from berkelium to the holding image buffer for a full update
		*
		*  @param[in] PLCore::uint8 * pImageBuffer
		*  @param[in] int & nWidth
		*  @param[in] int & nHeight
		*  @param[in] const unsigned char * sourceBuffer
		*  @param[in] const Berkelium::Rect & sourceBufferRect
		*/
		void BufferCopyFull(PLCore::uint8 *pImageBuffer, int &nWidth, int &nHeight, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect);
		
		/**
		*  @brief
		*    Copies the buffer data from berkelium to the holding image buffer for a partial update
		*
		*  @param[in] PLCore::uint8 * pImageBuffer
		*  @param[in] int & nWidth
		*  @param[in] int & nHeight
		*  @param[in] const unsigned char * sourceBuffer
		*  @param[in] const Berkelium::Rect & sourceBufferRect
		*  @param[in] size_t numCopyRects
		*  @param[in] const Berkelium::Rect * copyRects
		*/
		void BufferCopyRects(PLCore::uint8 *pImageBuffer, int &nWidth, int &nHeight, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects);
		
		/**
		*  @brief
		*    Copies the buffer data from berkelium to the holding image buffer for a scrolled update
		*
		*  @param[in] PLCore::uint8 * pImageBuffer
		*  @param[in] int & nWidth
		*  @param[in] int & nHeight
		*  @param[in] const unsigned char * sourceBuffer
		*  @param[in] const Berkelium::Rect & sourceBufferRect
		*  @param[in] size_t numCopyRects
		*  @param[in] const Berkelium::Rect * copyRects
		*  @param[in] int dx
		*  @param[in] int dy
		*  @param[in] const Berkelium::Rect & scrollRect
		*/
		void BufferCopyScroll(PLCore::uint8 *pImageBuffer, int &nWidth, int &nHeight, const unsigned char *sourceBuffer, const Berkelium::Rect &sourceBufferRect, size_t numCopyRects, const Berkelium::Rect *copyRects, int dx, int dy, const Berkelium::Rect &scrollRect);
		
		/**
		*  @brief
		*    Uploads the image buffer data to the GPU
		*/
		void BufferUploadToGPU();
		
		/**
		*  @brief
		*    Destroys this berkelium window and recreates it
		*
		*  @remarks
		*    This is useful for when a window crashes and needs to be recovered.
		*/
		void RecreateWindow();
		
		/**
		*  @brief
		*    Creates a berkelium context needed for the creation of the berkelium window instance
		*
		*  @note
		*    This will at some point be moved back to Gui // [10-07-2012 Icefire]
		*/
		void CreateBerkeliumContext();
		
		/**
		*  @brief
		*    Destroys the berkelium context
		*
		*  @note
		*    This will at some point be moved back to Gui // [10-07-2012 Icefire]
		*/
		void DestroyContext();
		
		/**
		*  @brief
		*    Sets the default windows settings
		*/
		void SetWindowSettings();
		void SetupToolTipWindow();
		void DestroyToolTipWindow();
		
		/**
		*  @brief
		*    Sets the default callback functions for this window
		*/
		void SetDefaultCallBackFunctions();
		
		/**
		*  @brief
		*    Draws a widget by pointer
		*
		*  @param[in] sWidget * psWidget
		*/
		void DrawWidget(sWidget *psWidget);
		
		/**
		*  @brief
		*    Draws widgets on screen
		*/
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
		PLRenderer::TextureBuffer *m_pTextureBufferNew;
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