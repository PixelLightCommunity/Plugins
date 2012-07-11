#ifndef __PLAWESOMIUM_SRPWINDOWS_H__
#define __PLAWESOMIUM_SRPWINDOWS_H__
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
#include <PLRenderer/Renderer/VertexShader.h>
#include <PLRenderer/Renderer/FragmentShader.h>
#include <PLGraphics/Image/Image.h>
#include <PLGraphics/Image/ImageBuffer.h>
#include <PLMath/Vector2.h>
#include <PLMath/Vector2i.h>
#include <PLMath/Rectangle.h>
#include <PLMath/Matrix4x4.h>

#include "Awesomium/WebCore.h"
#include "Awesomium/WebConfig.h"
#include "Awesomium/WebView.h"
#include "Awesomium/STLHelpers.h"
#include "Awesomium/BitmapSurface.h"
#include "Awesomium/WebViewListener.h"

#include "PLAwesomium.h"


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
namespace PLAwesomium {


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
	//Berkelium::Window *pWindow;
	Awesomium::WebView *pWindow;
	void *replyMsg;
	//Berkelium::URLString origin;
	PLCore::String sFunctionName;
	size_t nNumberOfParameters;
	//Berkelium::Script::Variant *pParameters;
};


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class SRPWindows : public PLScene::SceneRendererPass,
	public Awesomium::WebViewListener::View,
	public Awesomium::WebViewListener::Process,
	public Awesomium::WebViewListener::Load,
	public Awesomium::JSMethodHandler {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(PLAWESOMIUM_RTTI_EXPORT, SRPWindows, "PLAwesomium", PLScene::SceneRendererPass, "")
		pl_constructor_1(ParameterConstructor, const PLCore::String&, "", "")
	pl_class_end


	public:
		PLAWESOMIUM_API SRPWindows(const PLCore::String &sName);
		PLAWESOMIUM_API virtual ~SRPWindows();

		PLAWESOMIUM_API void SetRenderer(PLRenderer::Renderer *pRenderer);
		PLAWESOMIUM_API bool Initialize(PLRenderer::Renderer *pRenderer, const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		PLAWESOMIUM_API void DestroyInstance() const;
		PLAWESOMIUM_API Awesomium::WebView *GetAwesomiumWindow() const; /*unused*/
		PLAWESOMIUM_API void CreateAwesomiumWindow();
		PLAWESOMIUM_API sWindowsData *GetData() const;
		PLAWESOMIUM_API bool AddSceneRenderPass(PLScene::SceneRenderer *pSceneRenderer);
		PLAWESOMIUM_API bool RemoveSceneRenderPass();
		PLAWESOMIUM_API void MoveToFront();
		PLAWESOMIUM_API PLCore::String GetName() const; /*unused*/
		PLAWESOMIUM_API void DestroyWindow();
		PLAWESOMIUM_API PLMath::Vector2i GetPosition() const; /*unused*/
		PLAWESOMIUM_API PLMath::Vector2i GetSize() const;
		PLAWESOMIUM_API PLMath::Vector2i GetRelativeMousePosition(const PLMath::Vector2i &vMousePos) const;
		PLAWESOMIUM_API int GetSceneRenderPassIndex();
		PLAWESOMIUM_API void MoveWindow(const int &nX, const int &nY);
		PLAWESOMIUM_API void ClearCallBacks() const;
		PLAWESOMIUM_API PLCore::uint32 GetNumberOfCallBacks() const;
		PLAWESOMIUM_API sCallBack *GetCallBack(const PLCore::String &sKey) const;
		PLAWESOMIUM_API bool RemoveCallBack(const PLCore::String &sKey) const; /*unused*/
		PLAWESOMIUM_API void ResizeWindow(const int &nWidth, const int &nHeight); /*unused*/
		PLAWESOMIUM_API bool AddCallBackFunction(const PLCore::DynFuncPtr pDynFunc, PLCore::String sJSFunctionName = "",  bool bHasReturn = false); /*unused*/
		PLAWESOMIUM_API void ExecuteJavascript(const PLCore::String &sJavascript) const;
		PLAWESOMIUM_API void UpdateCall();
		PLAWESOMIUM_API void SetAwesomiumWebCore(Awesomium::WebCore *pAwesomiumWebCore);
		PLAWESOMIUM_API bool IsLoaded() const;

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);

		virtual void Draw(PLRenderer::Renderer &cRenderer, const PLScene::SQCull &cCullQuery) override;

		virtual void OnChangeTitle(Awesomium::WebView *caller, const Awesomium::WebString &title);
		virtual void OnChangeAddressBar(Awesomium::WebView *caller, const Awesomium::WebURL &url);
		virtual void OnChangeTooltip(Awesomium::WebView *caller, const Awesomium::WebString &tooltip);
		virtual void OnChangeTargetURL(Awesomium::WebView *caller, const Awesomium::WebURL &url);
		virtual void OnChangeCursor(Awesomium::WebView *caller, Awesomium::Cursor cursor);
		virtual void OnChangeFocus(Awesomium::WebView *caller, Awesomium::FocusedElementType focused_type);
		virtual void OnShowCreatedWebView(Awesomium::WebView *caller, Awesomium::WebView *new_view, const Awesomium::WebURL &opener_url, const Awesomium::WebURL &target_url, const Awesomium::Rect &initial_pos, bool is_popup);

		virtual void OnUnresponsive(Awesomium::WebView *caller);
		virtual void OnResponsive(Awesomium::WebView *caller);
		virtual void OnCrashed(Awesomium::WebView *caller, Awesomium::TerminationStatus status);

		virtual void OnBeginLoadingFrame(Awesomium::WebView *caller, PLCore::int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url, bool is_error_page);
		virtual void OnFailLoadingFrame(Awesomium::WebView *caller, PLCore::int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url, int error_code, const Awesomium::WebString &error_desc);
		virtual void OnFinishLoadingFrame(Awesomium::WebView *caller, PLCore::int64 frame_id, bool is_main_frame, const Awesomium::WebURL &url);
		virtual void OnDocumentReady(Awesomium::WebView *caller, const Awesomium::WebURL &url);

		virtual void OnMethodCall(Awesomium::WebView *caller, unsigned int remote_object_id, const Awesomium::WebString &method_name, const Awesomium::JSArray &args);
		virtual Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView *caller, unsigned int remote_object_id, const Awesomium::WebString &method_name, const Awesomium::JSArray &args);

		PLRenderer::VertexBuffer *CreateVertexBuffer(const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		PLRenderer::ProgramWrapper *CreateProgramWrapper();
		bool UpdateVertexBuffer(PLRenderer::VertexBuffer *pVertexBuffer, const PLMath::Vector2 &vPosition, const PLMath::Vector2 &vImageSize);
		void DrawWindow();
		void BufferUploadToGPU();
		void RecreateWindow();
		void SetWindowSettings();
		void SetDefaultCallBackFunctions();
		
		Awesomium::WebCore *m_pCurrentAwesomiumWebCore;
		Awesomium::WebView *m_pWindow;
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
		PLCore::String m_sLastKnownUrl; /*we might need this*/
		PLCore::HashMap<PLCore::String, sCallBack*> *m_pDefaultCallBacks;
		PLCore::HashMap<PLCore::String, PLCore::DynFuncPtr> *m_pCallBackFunctions;
		bool m_bIgnoreBufferUpdate;


};


};


#endif // __PLAWESOMIUM_SRPWINDOWS_H__