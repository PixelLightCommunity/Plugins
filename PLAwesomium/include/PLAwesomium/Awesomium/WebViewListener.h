///
/// @file WebViewListener.h
///
/// @brief The header for the WebViewListener events.
///
/// @author
///
/// This file is a part of Awesomium, a Web UI bridge for native apps.
///
/// Website: <http://www.awesomium.com>
///
/// Copyright (C) 2012 Khrona. All rights reserved. Awesomium is a
/// trademark of Khrona.
///
#ifndef AWESOMIUM_WEB_VIEW_LISTENER_H_
#define AWESOMIUM_WEB_VIEW_LISTENER_H_
#pragma once

#include <Awesomium/Platform.h>
#include <Awesomium/WebMenuItem.h>
#include <Awesomium/WebStringArray.h>

namespace Awesomium {

class WebURL;
class WebString;
class WebView;

///
/// An enumeration of all the possible web cursors.
///
/// @see WebViewListener::View::OnChangeCursor
///
enum Cursor {
  kCursor_Pointer,
  kCursor_Cross,
  kCursor_Hand,
  kCursor_IBeam,
  kCursor_Wait,
  kCursor_Help,
  kCursor_EastResize,
  kCursor_NorthResize,
  kCursor_NorthEastResize,
  kCursor_NorthWestResize,
  kCursor_SouthResize,
  kCursor_SouthEastResize,
  kCursor_SouthWestResize,
  kCursor_WestResize,
  kCursor_NorthSouthResize,
  kCursor_EastWestResize,
  kCursor_NorthEastSouthWestResize,
  kCursor_NorthWestSouthEastResize,
  kCursor_ColumnResize,
  kCursor_RowResize,
  kCursor_MiddlePanning,
  kCursor_EastPanning,
  kCursor_NorthPanning,
  kCursor_NorthEastPanning,
  kCursor_NorthWestPanning,
  kCursor_SouthPanning,
  kCursor_SouthEastPanning,
  kCursor_SouthWestPanning,
  kCursor_WestPanning,
  kCursor_Move,
  kCursor_VerticalText,
  kCursor_Cell,
  kCursor_ContextMenu,
  kCursor_Alias,
  kCursor_Progress,
  kCursor_NoDrop,
  kCursor_Copy,
  kCursor_None,
  kCursor_NotAllowed,
  kCursor_ZoomIn,
  kCursor_ZoomOut,
  kCursor_Grab,
  kCursor_Grabbing,
  kCursor_Custom
};

///
/// Used with WebViewListener::View::OnChangeFocus
///
/// @note  You should generally forward keyboard events to the active WebView
/// whenever one of the following element types are focused: input, text-input, 
/// editable-content, or plugin
///
enum FocusedElementType {
  kFocusedElementType_None = 0,        ///< Nothing is focused
  kFocusedElementType_Text,            ///< A text-node is focused
  kFocusedElementType_Link,            ///< A link is focused
  kFocusedElementType_Input,           ///< An input element is focused
  kFocusedElementType_TextInput,       ///< A text-input element is focused
  kFocusedElementType_EditableContent, ///< Some editable content is focused
  kFocusedElementType_Plugin,          ///< A plugin (eg, Flash) is focused
  kFocusedElementType_Other,           ///< Some other element is focused
};

/// Used with WebViewListener::Process::OnCrashed
enum TerminationStatus {
  kTerminationStatus_Normal,       ///< Zero Exit Status
  kTerminationStatus_Abnormal,     ///< Non-Zero exit status
  kTerminationStatus_Killed,       ///< e.g. SIGKILL or Task Manager kill
  kTerminationStatus_Crashed,      ///< e.g. Segmentation Fault
  kTerminationStatus_StillRunning  ///< Process hasn't exited yet
};

/// Used with WebFileChooserInfo
enum WebFileChooserMode {
  kWebFileChooserMode_Open,         ///< Select a file (file should exist)
  kWebFileChooserMode_OpenMultiple, ///< Select multiple files
  kWebFileChooserMode_OpenFolder,   ///< Select a folder (folder should exist)
  kWebFileChooserMode_Save,         ///< Select a file to save to
};

/// Used with WebViewListener::Dialog::OnShowFileChooser
struct OSM_EXPORT WebFileChooserInfo {
  WebFileChooserMode mode;       ///< The type of dialog to display
  WebString title;               ///< Title of the dialog
  WebString default_file_name;   ///< Suggested file name for the dialog
  WebStringArray accept_types;   ///< Valid mime types
};

/// Used with WebViewListener::Menu::OnShowPopupMenu
struct OSM_EXPORT WebPopupMenuInfo {
  Awesomium::Rect bounds;  ///< The location to display the menu
  int item_height;         ///< The height of each menu item
  double item_font_size;   ///< The font-size of each menu item
  int selected_item;       ///< The index of the currently-selected item
  WebMenuItemArray items;  ///< The actual menu items
  bool right_aligned;      ///< Whether or not the menu is right-aligned
};

/// Namespace containing all the WebView event-listener interfaces.
namespace WebViewListener {

///
/// @brief  An interface that you can use to handle all View-related events
///         for a certain WebView.
///
/// @note  See WebView::set_view_listener
///
class OSM_EXPORT View {
 public:
  /// This event occurs when the page title has changed.
  virtual void OnChangeTitle(Awesomium::WebView* caller,
                             const Awesomium::WebString& title) = 0;

  /// This event occurs when the page URL has changed.
  virtual void OnChangeAddressBar(Awesomium::WebView* caller,
                                  const Awesomium::WebURL& url) = 0;

  /// This event occurs when the tooltip text has changed. You
  /// should hide the tooltip when the text is empty.
  virtual void OnChangeTooltip(Awesomium::WebView* caller,
                               const Awesomium::WebString& tooltip) = 0;

  /// This event occurs when the target URL has changed. This
  /// is usually the result of hovering over a link on a page.
  virtual void OnChangeTargetURL(Awesomium::WebView* caller,
                                 const Awesomium::WebURL& url) = 0;

  /// This event occurs when the cursor has changed. This is
  /// is usually the result of hovering over different content.
  virtual void OnChangeCursor(Awesomium::WebView* caller,
                              Awesomium::Cursor cursor) = 0;

  /// This event occurs when the focused element changes on the page.
  /// This is usually the result of textbox being focused or some other
  /// user-interaction event.
  virtual void OnChangeFocus(Awesomium::WebView* caller,
                             Awesomium::FocusedElementType focused_type) = 0;

  /// This event occurs when a WebView creates a new child WebView
  /// (usually the result of window.open or an external link). It
  /// is your responsibility to display this child WebView in your
  /// application. You should call Resize on the child WebView
  /// immediately after this event to make it match your container
  /// size.
  virtual void OnShowCreatedWebView(Awesomium::WebView* caller,
                                    Awesomium::WebView* new_view,
                                    const Awesomium::WebURL& opener_url,
                                    const Awesomium::WebURL& target_url,
                                    const Awesomium::Rect& initial_pos,
                                    bool is_popup) = 0;

 protected:
  virtual ~View() {}
};

///
/// @brief  An interface that you can use to handle all page-loading
///         events for a certain WebView.
///
/// @note: See WebView::SetLoadListener
///
class OSM_EXPORT Load {
 public:
  /// This event occurs when the page begins loading a frame.
  virtual void OnBeginLoadingFrame(Awesomium::WebView* caller,
                                   int64 frame_id,
                                   bool is_main_frame,
                                   const Awesomium::WebURL& url,
                                   bool is_error_page) = 0;

  /// This event occurs when a frame fails to load. See error_desc
  /// for additional information.
  virtual void OnFailLoadingFrame(Awesomium::WebView* caller,
                                  int64 frame_id,
                                  bool is_main_frame,
                                  const Awesomium::WebURL& url,
                                  int error_code,
                                  const Awesomium::WebString& error_desc) = 0;

  /// This event occurs when the page finishes loading a frame.
  /// The main frame always finishes loading last for a given page load.
  virtual void OnFinishLoadingFrame(Awesomium::WebView* caller,
                                    int64 frame_id,
                                    bool is_main_frame,
                                    const Awesomium::WebURL& url) = 0;

  /// This event occurs when the DOM has finished parsing and the
  /// window object is available for JavaScript execution.
  virtual void OnDocumentReady(Awesomium::WebView* caller,
                               const Awesomium::WebURL& url) = 0;

 protected:
  virtual ~Load() {}
};

///
/// @brief  An interface that you can use to handle all process-related
///         events for a certain WebView.
///
/// Each WebView has an associated "render" process, you can use these events
/// to recover from crashes or hangs).
///
/// @note  See WebView::set_process_listener
///
class OSM_EXPORT Process {
 public:
  /// This event occurs when the process hangs.
  virtual void OnUnresponsive(Awesomium::WebView* caller) = 0;

  /// This event occurs when the process becomes responsive after
  /// a hang.
  virtual void OnResponsive(Awesomium::WebView* caller) = 0;

  /// This event occurs when the process crashes.
  virtual void OnCrashed(Awesomium::WebView* caller,
                         Awesomium::TerminationStatus status) = 0;
 protected:
  virtual ~Process() {}
};

///
/// @brief  An interface that you can use to handle all menu-related events
///         for a certain WebView.
///
/// @note  See WebView::set_menu_listener
///
class OSM_EXPORT Menu {
 public:
  ///
  /// This event occurs when the page requests to display a dropdown
  /// (popup) menu. This is usually the result of a user clicking on
  /// a "select" HTML input element. It is your responsibility to
  /// display this menu in your application. This event is not modal.
  ///
  /// @see WebView::DidSelectPopupMenuItem
  /// @see WebView::DidCancelPopupMenu
  ///
  virtual void OnShowPopupMenu(Awesomium::WebView* caller,
                               const WebPopupMenuInfo& menu_info) = 0;
 protected:
  virtual ~Menu() {}
};

///
/// @brief  An interface that you can use to handle all dialog-related events
///         for a certain WebView.
///
/// @see  WebView::set_dialog_listener
///
class OSM_EXPORT Dialog {
 public:
  ///
  /// This event occurs when the page requests to display a file chooser
  /// dialog. This is usually the result of a user clicking on an HTML
  /// input element with `type='file`. It is your responsibility to display
  /// this menu in your application. This event is not modal.
  ///
  /// @see WebView::DidChooseFiles
  ///
  virtual void OnShowFileChooser(Awesomium::WebView* caller,
                                 const WebFileChooserInfo& chooser_info) = 0;
 protected:
  virtual ~Dialog() {}
};

}  // namespace WebViewListener

}  // namespace Awesomium

#endif  // AWESOMIUM_WEB_VIEW_LISTENER_H_
