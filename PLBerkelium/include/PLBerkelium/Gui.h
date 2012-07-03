#ifndef __PLBERKELIUM_GUI_H__
#define __PLBERKELIUM_GUI_H__
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

#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"

#include "PLBerkelium.h"
#include "SRPWindow.h"
#include "SRPMousePointer.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLBerkelium {


//[-------------------------------------------------------]
//[ Defines                                               ]
//[-------------------------------------------------------]
#define BERKELIUMDUMMYWINDOW "berkeliumdummywindow"


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
	pl_class(PLBERKELIUM_RTTI_EXPORT, Gui, "PLBerkelium", PLCore::Object, "")
		pl_constructor_0(DefaultConstructor, "Default constructor", "")
		pl_slot_0(OnUpdate, "Called on event update by scene context", "")
		pl_slot_1(OnControl, PLInput::Control&, "Called when a control event has occurred, occurred control as first parameter", "")
	pl_class_end


	public:
		PLBERKELIUM_API Gui();
		PLBERKELIUM_API virtual ~Gui();

		PLBERKELIUM_API bool IsBerkeliumInitialized() const;
		PLBERKELIUM_API bool AddWindow(const PLCore::String &sName,
			const bool &pVisible = false,
			const PLCore::String &sUrl = "about:blank",
			const int &nWidth = 32,
			const int &nHeight = 32,
			const int &nX = 0,
			const int &nY = 0,
			const bool &bTransparent = true,
			const bool &bEnabled = true);
		PLBERKELIUM_API SRPWindow *GetWindow(const PLCore::String &sName);
		PLBERKELIUM_API Berkelium::Window *GetBerkeliumWindow(const PLCore::String &sName);
		PLBERKELIUM_API PLCore::HashMap<PLCore::String, SRPWindow*> *GetWindowsMap() const;
		PLBERKELIUM_API void UpdateBerkelium();
		PLBERKELIUM_API void DestroyInstance() const;
		PLBERKELIUM_API void SetRenderers(PLRenderer::Renderer *pRenderer, PLScene::SceneRenderer *pSceneRenderer);
		PLBERKELIUM_API sWindowsData *GetWindowData(const PLCore::String &sName);
		PLBERKELIUM_API bool RemoveWindow(const PLCore::String &sName);
		PLBERKELIUM_API SRPMousePointer *GetMousePointer() const;
		PLBERKELIUM_API void FocusWindow(SRPWindow *pSRPWindow);
		PLBERKELIUM_API SRPWindow *GetFocusedWindow() const;
		PLBERKELIUM_API bool ConnectController(PLInput::Controller *pController);
		PLBERKELIUM_API bool ConnectEventUpdate(PLScene::SceneContext *pSceneContext);
		PLBERKELIUM_API bool SetMousePointerVisible(const bool &bVisible) const;
		PLBERKELIUM_API void UnFocusAllWindows();
		PLBERKELIUM_API bool SetWindowVisible(const PLCore::String &sName, const bool &bVisible = true);
		PLBERKELIUM_API void DebugNamesOfWindows();

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);
		
		void DestroyWindows();
		void Initialize();
		void StopBerkelium() const;
		void AddDummyWindow();
		PLCore::List<SRPWindow*> *GetMouseEnabledWindows();
		PLCore::List<SRPWindow*> *GetMouseOverWindows(const PLCore::List<SRPWindow*> *pEnabledWindows, const PLMath::Vector2i &vMousePos);
		void CreateMousePointer();
		void DestroyMousePointer() const;
		void OnUpdate();
		void MouseEvents(PLInput::Control &cControl);
		SRPWindow *GetTopMostWindow(PLCore::List<SRPWindow*> *pWindows);
		void MouseMove(const SRPWindow *pSRPWindow, const PLMath::Vector2i &vMousePos) const;
		void MouseClicks(SRPWindow *pSRPWindow, PLInput::Control &cControl);
		void MouseScrolls(SRPWindow *pSRPWindow, PLInput::Control &cControl);
		void OnControl(PLInput::Control &cControl);
		void DefaultCallBackHandler();
		void KeyboardEvents(PLInput::Control &cControl);
		void DragWindowHandler();
		void KeyboardHandler();
		void AddTextKey(const PLCore::String &sName, const PLCore::String &sKey, sButton *psButton);
		void AddKey(const PLCore::String &sName, const char &nKey, sButton *psButton);

		bool m_bBerkeliumInitialized;
		bool m_bRenderersInitialized;
		PLCore::HashMap<PLCore::String, SRPWindow*> *m_pWindows;
		PLScene::SceneRenderer *m_pCurrentSceneRenderer;
		PLRenderer::Renderer *m_pCurrentRenderer;
		SRPMousePointer *m_pSRPMousePointer;
		SRPWindow *m_pFocusedWindow;
		bool m_bControlsEnabled;
		bool m_bIsUpdateConnected;
		bool m_bIsControllerConnected;
		PLCore::String m_sLastControl;
		PLCore::uint64 m_nLastMouseLeftReleaseTime;
		SRPWindow *m_pLastMouseWindow;
		bool m_bMouseLeftDown;
		PLMath::Vector2i m_vLastKnownMousePos;
		SRPWindow *m_pDragWindow;
		SRPWindow *m_pResizeWindow;
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


#endif // __PLBERKELIUM_GUI_H__