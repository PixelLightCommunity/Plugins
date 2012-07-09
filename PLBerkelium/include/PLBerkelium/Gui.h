#ifndef __PLBERKELIUM_GUI_H__
#define __PLBERKELIUM_GUI_H__
#pragma once

// Gui verified [09-juli-2012 at 23:35 by Icefire]


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

		/**
		*  @brief
		*    Returns whether or not berkelium is initialized
		*
		*  @return
		*    'true' if berkelium is initialized, else 'false'
		*/
		PLBERKELIUM_API bool IsBerkeliumInitialized() const;
		
		/**
		*  @brief
		*    Adds a window based on given parameters
		*
		*  @param[in] const PLCore::String & sName
		*  @param[in] const bool & pVisible
		*  @param[in] const PLCore::String & sUrl
		*  @param[in] const int & nWidth
		*  @param[in] const int & nHeight
		*  @param[in] const int & nX
		*  @param[in] const int & nY
		*  @param[in] const bool & bTransparent
		*  @param[in] const bool & bEnabled
		*
		*  @return
		*    'true' if the window was added, else 'false'
		*/
		PLBERKELIUM_API bool AddWindow(const PLCore::String &sName,
			const bool &pVisible = false,
			const PLCore::String &sUrl = "about:blank",
			const int &nWidth = 32,
			const int &nHeight = 32,
			const int &nX = 0,
			const int &nY = 0,
			const bool &bTransparent = true,
			const bool &bEnabled = true);
		
		/**
		*  @brief
		*    Returns a window by name
		*
		*  @remarks
		*    If the window cannot be found this returns a pointer to the dummy window.
		*    You could for example verify that the returned window is the one you requested by verifying its name or properties.
		*
		*  @param[in] const PLCore::String & sName
		*
		*  @return
		*    pointer to window instance (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API SRPWindow *GetWindow(const PLCore::String &sName);
		
		/**
		*  @brief
		*    Returns the berkelium window instance of a window by name
		*
		*  @remarks
		*    If the window holding the berkelium window instance cannot be found this returns a pointer to the one of the dummy window.
		*
		*  @param[in] const PLCore::String & sName
		*
		*  @return
		*    pointer to berkelium window instance (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API Berkelium::Window *GetBerkeliumWindow(const PLCore::String &sName);
		
		/**
		*  @brief
		*    Returns the instance of the HashMap containing the windows
		*
		*  @remarks
		*    Should you so desire you can use this to get other information from the windows as well as iterate trough them.
		*
		*  @return
		*    pointer to HashMap holding the windows (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API PLCore::HashMap<PLCore::String, SRPWindow*> *GetWindowsMap() const;
		
		/**
		*  @brief
		*    Updates berkelium to process pending messages
		*
		*  @note
		*    You dont really need to call this, because it would be called by the SceneContext OnUpdate event if ConnectEventUpdate() is set.
		*/
		PLBERKELIUM_API void UpdateBerkelium();
		
		/**
		*  @brief
		*    Destroys this Gui instance
		*
		*  @remarks
		*    This will also destroy all created windows and stop berkelium from processing messages.
		*
		*  @note
		*    You must destroy berkelium yourself.
		*/
		PLBERKELIUM_API void DestroyInstance() const;
		
		/**
		*  @brief
		*    Set the renderers for future use
		*
		*  @remarks
		*    Windows are dependent on these renderers so these should be the first thing to set.
		*
		*  @note
		*    If you try to create windows or do anything else related if will fail.
		*
		*  @param[in] PLRenderer::Renderer * pRenderer
		*  @param[in] PLScene::SceneRenderer * pSceneRenderer
		*/
		PLBERKELIUM_API void SetRenderers(PLRenderer::Renderer *pRenderer, PLScene::SceneRenderer *pSceneRenderer);
		
		/**
		*  @brief
		*    Returns the data of a window by name
		*
		*  @remarks
		*    If the window holding the data cannot be found this returns data of the dummy window.
		*
		*  @param[in] const PLCore::String & sName
		*
		*  @return
		*    pointer to window data (do not destroy the returned instance!)
		*/
		PLBERKELIUM_API sWindowsData *GetWindowData(const PLCore::String &sName);
		
		/**
		*  @brief
		*    Removes a window by name
		*
		*  @remarks
		*    This removes and destroys a window.
		*
		*  @param[in] const PLCore::String & sName
		*
		*  @return
		*    'true' if the window was removed, else 'false'
		*/
		PLBERKELIUM_API bool RemoveWindow(const PLCore::String &sName);
		
		/**
		*  @brief
		*    Returns the mouse pointer
		*
		*  @return
		*    pointer to mouse pointer (can be a null pointer, do not destroy the returned instance!)
		*/
		PLBERKELIUM_API SRPMousePointer *GetMousePointer() const;
		
		/**
		*  @brief
		*    Focuses a window
		*
		*  @remarks
		*    Because there can be only one focused window this will unfocus the previously focused window.
		*
		*  @param[in] SRPWindow * pSRPWindow
		*/
		PLBERKELIUM_API void FocusWindow(SRPWindow *pSRPWindow);
		
		/**
		*  @brief
		*    Returns the focused window
		*
		*  @return
		*    pointer to focused window (can be a null pointer, do not destroy the returned instance!)
		*/
		PLBERKELIUM_API SRPWindow *GetFocusedWindow() const;
		
		/**
		*  @brief
		*    Connects a OnControl slot to the given Controller
		*
		*  @remarks
		*    This is required for mouse and keyboard input, not mandatory.
		*
		*  @param[in] PLInput::Controller * pController
		*
		*  @return
		*    'true' if successful, else 'false'
		*/
		PLBERKELIUM_API bool ConnectController(PLInput::Controller *pController);
		
		/**
		*  @brief
		*    Connects a OnUpdate slot to the given SceneContext (EventUpdate)
		*
		*  @remarks
		*    This is needed to process the update structure which includes;
		*    -> UpdateBerkelium()
		*    -> KeyboardHandler()
		*    -> DefaultCallBackHandler()
		*    -> DragWindowHandler()
		*    -> ResizeWindowHandler()
		*
		*  @note
		*    Not setting this will disable the above from being called by the EventUpdate.
		*
		*  @param[in] PLScene::SceneContext * pSceneContext
		*
		*  @return
		*    'true' if successful, else 'false'
		*/
		PLBERKELIUM_API bool ConnectEventUpdate(PLScene::SceneContext *pSceneContext);
		
		/**
		*  @brief
		*    Sets the visibility of the mouse pointer
		*
		*  @param[in] const bool & bVisible
		*
		*  @return
		*    'true' if successful, else 'false'
		*/
		PLBERKELIUM_API bool SetMousePointerVisible(const bool &bVisible) const;
		
		/**
		*  @brief
		*    Unfocus all windows
		*
		*  @remarks
		*    Normally only one window should be focused but because a user is free to focus any window manually we need a way to undo that.
		*
		*  @note
		*    This will unfocus every window created.
		*/
		PLBERKELIUM_API void UnFocusAllWindows();
		
		/**
		*  @brief
		*    Sets the visibility of a window by name
		*
		*  @note
		*    Window will be unfocused if you want to hide it.
		*
		*  @param[in] const PLCore::String & sName
		*  @param[in] const bool & bVisible
		*
		*  @return
		*    'true' if the visibility of the window was set, else 'false'
		*/
		PLBERKELIUM_API bool SetWindowVisible(const PLCore::String &sName, const bool &bVisible = true);
		
		/**
		*  @brief
		*    A debug method to output the names of all windows
		*
		*  @note
		*    This will be deprecated when no longer needed.
		*/
		PLBERKELIUM_API void DebugNamesOfWindows();

	protected:

	private:
		void DebugToConsole(const PLCore::String &sString);
		
		/**
		*  @brief
		*    Destroys created windows
		*
		*  @remarks
		*    Iterates trough created windows and destroys them.
		*/
		void DestroyWindows();
		
		/**
		*  @brief
		*    Initializes needed components
		*
		*  @remarks
		*    This creates and initializes berkelium and creates a dummy window.
		*/
		void Initialize();
		
		/**
		*  @brief
		*    Stops running berkelium
		*/
		void StopBerkelium() const;
		
		/**
		*  @brief
		*    Adds a dummy window and set its data
		*/
		void AddDummyWindow();
		
		/**
		*  @brief
		*    Returns mouse enabled window(s)
		*
		*  @remarks
		*    This iterates trough all windows and returns a list of those that allow mouse events.
		*
		*  @return
		*    Pointer to the list created (can be a null pointer, destroy the returned instance when you no longer need it)
		*/
		PLCore::List<SRPWindow*> *GetMouseEnabledWindows();
		
		/**
		*  @brief
		*    Returns window(s) if the mouse is currently over them
		*
		*  @note
		*    This iterates trough windows given as parameter and returns a list of those where the mouse is currently over.
		*
		*  @param[in] const PLCore::List<SRPWindow * > * plstEnabledWindows
		*  @param[in] const PLMath::Vector2i & vMousePos
		*
		*  @return
		*    Pointer to the list created (can be a null pointer, destroy the returned instance when you no longer need it)
		*/
		PLCore::List<SRPWindow*> *GetMouseOverWindows(const PLCore::List<SRPWindow*> *plstEnabledWindows, const PLMath::Vector2i &vMousePos);
		
		/**
		*  @brief
		*    Creates the mouse pointer
		*/
		void CreateMousePointer();
		
		/**
		*  @brief
		*    Destroys the mouse pointer
		*/
		void DestroyMousePointer() const;
		
		/**
		*  @brief
		*    Processes update methods
		*
		*  @remarks
		*    Processes the update structure which includes;
		*    -> UpdateBerkelium()
		*    -> KeyboardHandler()
		*    -> DefaultCallBackHandler()
		*    -> DragWindowHandler()
		*    -> ResizeWindowHandler()
		*/
		void OnUpdate();
		
		/**
		*  @brief
		*    Processes all mouse events
		*
		*  @param[in] PLInput::Control & cControl
		*/
		void MouseEvents(PLInput::Control &cControl);
		
		/**
		*  @brief
		*    Returns top most window
		*
		*  @remarks
		*    This iterates trough windows given as parameter and returns the top most one.
		*
		*  @param[in] PLCore::List<SRPWindow * > * plstWindows
		*
		*  @return
		*    pointer to window (can be a null pointer, do not destroy the returned instance!)
		*/
		SRPWindow *GetTopMostWindow(PLCore::List<SRPWindow*> *plstWindows);
		
		/**
		*  @brief
		*    Process mouse moving on a window
		*
		*  @param[in] const SRPWindow * pSRPWindow
		*  @param[in] const PLMath::Vector2i & vMousePos
		*/
		void MouseMove(const SRPWindow *pSRPWindow, const PLMath::Vector2i &vMousePos) const;
		
		/**
		*  @brief
		*    Process mouse clicking on a window
		*
		*  @param[in] SRPWindow * pSRPWindow
		*  @param[in] PLInput::Control & cControl
		*/
		void MouseClicks(SRPWindow *pSRPWindow, PLInput::Control &cControl);
		
		/**
		*  @brief
		*    Process mouse scrolling on a window
		*
		*  @param[in] SRPWindow * pSRPWindow
		*  @param[in] PLInput::Control & cControl
		*/
		void MouseScrolls(SRPWindow *pSRPWindow, PLInput::Control &cControl);
		
		/**
		*  @brief
		*    On control method that should be called when a control event is fired
		*
		*  @note
		*    This only happens if you have connected the controller, see ConnectController().
		*
		*  @param[in] PLInput::Control & cControl
		*/
		void OnControl(PLInput::Control &cControl);
		
		/**
		*  @brief
		*    Handles all default callbacks
		*
		*  @remarks
		*    This iterates trough the windows and checks if any of them have callbacks.
		*    If the callbacks are predefined as default then a correspondent action is taken.
		*/
		void DefaultCallBackHandler();
		
		/**
		*  @brief
		*    Processes all keyboard events
		*
		*  @param[in] PLInput::Control & cControl
		*/
		void KeyboardEvents(PLInput::Control &cControl);
		
		/**
		*  @brief
		*    Handles the dragging of a window
		*
		*  @remarks
		*    If the dragging window is defined and certain parameter are met.
		*    The window will be dragged on screen relative to the mouse movement.
		*/
		void DragWindowHandler();
		
		/**
		*  @brief
		*    Handles all keyboard related processes on update
		*/
		void KeyboardHandler();
		
		/**
		*  @brief
		*    Adds a text key to the keyboard handler
		*
		*  @param[in] const PLCore::String & sName
		*  @param[in] const PLCore::String & sKey
		*  @param[in] sButton * psButton
		*/
		void AddTextKey(const PLCore::String &sName, const PLCore::String &sKey, sButton *psButton);
		
		/**
		*  @brief
		*    Adds a key to the keyboard handler
		*
		*  @param[in] const PLCore::String & sName
		*  @param[in] const char & nKey
		*  @param[in] sButton * psButton
		*/
		void AddKey(const PLCore::String &sName, const char &nKey, sButton *psButton);
		
		/**
		*  @brief
		*    Handles the resizing of a window
		*
		*  @remarks
		*    If the resizing window is defined and certain parameter are met.
		*    The window will be resizing on screen relative to the mouse movement.
		*
		*  @note
		*    This method is somewhat experimental and is subject to change
		*/
		void ResizeWindowHandler();

		bool m_bBerkeliumInitialized;
		bool m_bRenderersInitialized;
		PLCore::HashMap<PLCore::String, SRPWindow*> *m_pmapWindows;
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
		PLCore::HashMap<PLCore::String, sButton*> *m_pmapTextButtonHandler;
		PLCore::HashMap<PLCore::String, sButton*> *m_pmapKeyButtonHandler;
		PLCore::uint64 m_nLastTextKeySendTime;
		PLCore::uint64 m_nLastKeySendTime;
		int m_nTextKeyHitCount;
		int m_nKeyHitCount;


};


};


#endif // __PLBERKELIUM_GUI_H__