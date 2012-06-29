#ifndef __PLAWESOMIUM_GUI_H__
#define __PLAWESOMIUM_GUI_H__
#pragma once


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <PLCore/System/System.h>
#include <PLCore/System/Console.h>
#include <PLCore/Base/Object.h>
#include <PLCore/Container/List.h>
#include <PLCore/Tools/Timing.h>
#include <PLCore/Application/CoreApplication.h>
#include <PLCore/Frontend/FrontendApplication.h>
#include <PLScene/Compositing/SceneRenderer.h>
#include <PLScene/Scene/SceneContext.h>
#include <PLRenderer/RendererContext.h>
#include <PLRenderer/Renderer/Renderer.h>
#include <PLMath/Vector2.h>
#include <PLMath/Vector2i.h>
#include <PLInput/Input/Controller.h>
#include <PLInput/Input/Controls/Control.h>
#include <PLInput/Input/Controls/Button.h>
#include <PLInput/Input/InputManager.h>
#include <PLInput/Input/Controls/Button.h>
#include <PLInput/Input/Controls/Control.h>
#include <PLInput/Input/Controls/Axis.h>
#include <PLEngine/Controller/SNMEgoLookController.h>
#include <PLEngine/Application/EngineApplication.h>

#include "Awesomium/WebCore.h"
#include "Awesomium/WebConfig.h"
#include "Awesomium/WebView.h"
#include "Awesomium/STLHelpers.h"
#include "Awesomium/BitmapSurface.h"

#include "PLAwesomium.h"
#include "SRPWindows.h"
#include "SRPMousePointer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLAwesomium {


//[-------------------------------------------------------]
//[ Defines                                               ]
//[-------------------------------------------------------]
#define DUMMYWINDOW "awesomiumdummywindow"


//[-------------------------------------------------------]
//[ Structures                                            ]
//[-------------------------------------------------------]
struct sButton
{
	PLCore::String sKey;
	char nKey;
	bool bValid;
	bool bMod;
};


//[-------------------------------------------------------]
//[ Classes                                               ]
//[-------------------------------------------------------]
class Gui : public PLCore::Object {


	//[-------------------------------------------------------]
	//[ RTTI interface                                        ]
	//[-------------------------------------------------------]
	pl_class(PLAWESOMIUM_RTTI_EXPORT, Gui, "PLAwesomium", PLCore::Object, "")
		pl_constructor_0(DefaultConstructor, "Default constructor", "")
		pl_slot_0(OnUpdate, "", "")
		pl_slot_1(OnControl, PLInput::Control&, "", "")
	pl_class_end


	public:
		PLAWESOMIUM_API Gui();
		PLAWESOMIUM_API virtual ~Gui();

		PLAWESOMIUM_API bool IsAwesomiumInitialized() const;
		PLAWESOMIUM_API bool AddWindow(const PLCore::String &sName,
			const bool &pVisible = true,
			const PLCore::String &sUrl = "about:blank",
			const int &nWidth = 128,
			const int &nHeight = 128,
			const int &nX = 0,
			const int &nY = 0,
			const bool &bTransparent = true,
			const bool &bEnabled = true);
		PLAWESOMIUM_API SRPWindows *GetWindow(const PLCore::String &sName) const;
		PLAWESOMIUM_API Awesomium::WebView *GetAwesomiumWindow(const PLCore::String &sName) const;
		PLAWESOMIUM_API PLCore::HashMap<PLCore::String, SRPWindows*> *GetWindowsMap() const;
		PLAWESOMIUM_API void UpdateAwesomium();
		PLAWESOMIUM_API void DestroyInstance() const;
		PLAWESOMIUM_API void SetRenderers(PLRenderer::Renderer *pRenderer, PLScene::SceneRenderer *pSceneRenderer);
		PLAWESOMIUM_API sWindowsData *GetWindowData(const PLCore::String &sName) const;
		PLAWESOMIUM_API bool RemoveWindow(const PLCore::String &sName);
		PLAWESOMIUM_API SRPMousePointer *GetMousePointer() const;
		PLAWESOMIUM_API void FocusWindow(SRPWindows *pSRPWindows);
		PLAWESOMIUM_API SRPWindows *GetFocusedWindow() const;
		PLAWESOMIUM_API bool ConnectController(PLInput::Controller *pController);
		PLAWESOMIUM_API bool ConnectEventUpdate(PLScene::SceneContext *pSceneContext);
		PLAWESOMIUM_API bool SetMousePointerVisible(const bool &bVisible) const;
		PLAWESOMIUM_API void UnFocusAllWindows();
		PLAWESOMIUM_API bool SetWindowVisible(const PLCore::String &sName, const bool &bVisible = true);

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);
		
		void DestroyWindows();
		void Initialize();
		void StopAwesomium() const;
		void AddDummyWindow();
		PLCore::List<SRPWindows*> *GetMouseEnabledWindows();
		PLCore::List<SRPWindows*> *GetMouseOverWindows(const PLCore::List<SRPWindows*> *pEnabledWindows, const PLMath::Vector2i &vMousePos);
		void CreateMousePointer();
		void RemoveMousePointer() const;
		void OnUpdate();
		void MouseEvents(PLInput::Control &cControl);
		SRPWindows *GetTopMostWindow(PLCore::List<SRPWindows*> *pWindows);
		void MouseMove(const SRPWindows *pSRPWindow, const PLMath::Vector2i &vMousePos) const;
		void MouseClicks(SRPWindows *pSRPWindow, PLInput::Control &cControl);
		void MouseScrolls(SRPWindows *pSRPWindow, PLInput::Control &cControl);
		void OnControl(PLInput::Control &cControl);
		void DefaultCallBackHandler();
		void KeyboardEvents(PLInput::Control &cControl);
		void DragWindowHandler();
		void KeyboardHandler();
		void AddTextKey(const PLCore::String &sName, const PLCore::String &sKey, sButton *psButton);
		void AddKey(const PLCore::String &sName, const char &nKey, sButton *psButton);

		Awesomium::WebCore *m_pAwesomiumWebCore;
		bool m_bAwesomiumInitialized;
		bool m_bRenderersInitialized;
		PLCore::HashMap<PLCore::String, SRPWindows*> *m_pWindows;
		PLScene::SceneRenderer *m_pCurrentSceneRenderer;
		PLRenderer::Renderer *m_pCurrentRenderer;
		SRPMousePointer *m_pSRPMousePointer;
		SRPWindows *m_pFocusedWindow;
		bool m_bControlsEnabled;
		bool m_bIsUpdateConnected;
		bool m_bIsControllerConnected;
		PLCore::String m_sLastControl;
		PLCore::uint64 m_nLastMouseLeftReleaseTime;
		SRPWindows *m_pLastMouseWindow;
		bool m_bMouseLeftDown;
		PLMath::Vector2i m_vLastKnownMousePos;
		SRPWindows *m_pDragWindow;
		SRPWindows *m_pResizeWindow;
		bool m_bMouseMoved;
		PLMath::Vector2i m_vLockMousePos;
		PLCore::HashMap<PLCore::String, sButton*> *m_pTextButtonHandler;
		PLCore::HashMap<PLCore::String, sButton*> *m_pKeyButtonHandler;
		PLCore::uint64 m_nLastTextKeySendTime;
		PLCore::uint64 m_nLastKeySendTime;
		int m_nTextKeyHitCount;
		int m_nKeyHitCount;


};


};


#endif // __PLAWESOMIUM_GUI_H__