#include "text_edit.h"
#include <algorithm>
#include <windows.h>
#include <vsstyle.h>

#include "base/win/windows_version.h"
#include "ui/views/widget/widget.h"
#include "ui/views/controls/menu/menu_item_view.h"
#include "ui/views/controls/menu/menu_model_adapter.h"
#include "ui/views/controls/menu/menu_runner.h"
#include "ui/base/resource/resource_bundle.h"
#include "grit/ui_strings.h"
#include "skia/ext/skia_utils_win.h"
#include "ui/native_theme/native_theme_win.h"

namespace views
{

gfx::FontList GetDefaultFontList() {
    return ResourceBundle::GetSharedInstance().GetFontList(
        ResourceBundle::BaseFont);
}

HMODULE TextEdit::loaded_libarary_module_ = false;

TextEdit::TextEdit()
    : font_list_(GetDefaultFontList())
    , use_default_text_color_(true)
    , text_color_(SK_ColorBLACK)
    , bg_color_(0)
    , contains_mouse_(false)
{
    set_focusable(true);
}

void TextEdit::SetContent( const string16& new_contents )
{
    if (!IsWindow()) {
        cache_text_ = new_contents;
    }
    else {
        // 1200 is unicode
        SetTextEx(new_contents.c_str(), 0, 1200);
    }
}

void TextEdit::AppendContent( const string16 &contents )
{
    if (!IsWindow()) {
        cache_text_ = contents;
    }
    else {
        AppendText(contents.c_str());
    }
}

void TextEdit::ClearContent()
{
    SetContent(string16());
}

TextEdit::~TextEdit()
{
    if (IsWindow()) {
        DestroyWindow();
    }
}

void TextEdit::CreateNativeControl()
{
    if (!loaded_libarary_module_) {
        // msftedit.dll is RichEdit ver 4.1.
        // This version is available from WinXP SP1 and has TSF support.
        loaded_libarary_module_ = LoadLibrary(L"msftedit.dll");
    }

    RECT r = {0, 0, this->width(), this->height()};
    //DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_NOHIDESEL;
    DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_NOHIDESEL;
    //DWORD ex_style = WS_EX_CLIENTEDGE;
    DWORD ex_style = 0;
    HWND hwnd = Create(GetWidget()->GetNativeWindow(), r, NULL, style, ex_style);
    if (NULL == hwnd) {
        return;
    }
    NativeControlCreated(hwnd);

    // Disable auto font changing
    const LRESULT lang_option = SendMessage(EM_GETLANGOPTIONS, 0, 0);
    SendMessage(EM_SETLANGOPTIONS, 0, lang_option & ~IMF_AUTOFONT);

    SetFont();
}

void TextEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
}

void TextEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    SetMsgHandled(FALSE);
}

int TextEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (!cache_text_.empty()) {
        SetContent(cache_text_);
        cache_text_.clear();
    }
    SetMsgHandled(FALSE);
    return 0;
}

void TextEdit::OnContextMenu( CWindow wnd, CPoint point )
{
    POINT p(point);
    ui::MenuSourceType source_type = ui::MENU_SOURCE_MOUSE;
    if (point.x == -1 || point.y == -1) {
        source_type = ui::MENU_SOURCE_KEYBOARD;
        GetCaretPos(&p);
        MapWindowPoints(HWND_DESKTOP, &p, 1);
    }
    BuildContextMenu();

    MenuModelAdapter adapter(context_menu_contents_.get());
    context_menu_runner_.reset(new MenuRunner(adapter.CreateMenu()));

    ignore_result(context_menu_runner_->RunMenuAt(GetWidget(), NULL,
        gfx::Rect(gfx::Point(p), gfx::Size()), MenuItemView::TOPLEFT,
        source_type, MenuRunner::HAS_MNEMONICS));
}

void TextEdit::BuildContextMenu()
{
    if (context_menu_contents_.get()) {
        return;
    }
    context_menu_contents_.reset(new ui::SimpleMenuModel(this));
    context_menu_contents_->AddItemWithStringId(IDS_APP_UNDO, IDS_APP_UNDO);
    context_menu_contents_->AddSeparator(ui::NORMAL_SEPARATOR);
    context_menu_contents_->AddItemWithStringId(IDS_APP_CUT, IDS_APP_CUT);
    context_menu_contents_->AddItemWithStringId(IDS_APP_COPY, IDS_APP_COPY);
    context_menu_contents_->AddItemWithStringId(IDS_APP_PASTE, IDS_APP_PASTE);
    context_menu_contents_->AddSeparator(ui::NORMAL_SEPARATOR);
    context_menu_contents_->AddItemWithStringId(IDS_APP_SELECT_ALL,
        IDS_APP_SELECT_ALL);
}

bool TextEdit::IsCommandIdChecked( int command_id ) const
{
    return false;
}

bool TextEdit::IsCommandIdEnabled( int command_id ) const
{
    switch (command_id) {
    case IDS_APP_UNDO:       return !!CanUndo();
    case IDS_APP_CUT:        return !!CanCut();
    case IDS_APP_COPY:       return !!CanCopy();
    case IDS_APP_PASTE:      return !!CanPaste();
    case IDS_APP_SELECT_ALL: return !!CanSelectAll();
    default:                 NOTREACHED();
        return false;
    }
}

bool TextEdit::GetAcceleratorForCommandId( int command_id, ui::Accelerator* accelerator )
{
    // The standard Ctrl-X, Ctrl-V and Ctrl-C are not defined as accelerators
    // anywhere so we need to check for them explicitly here.
    switch (command_id) {
    case IDS_APP_CUT:
        *accelerator = ui::Accelerator(ui::VKEY_X, ui::EF_CONTROL_DOWN);
        return true;
    case IDS_APP_COPY:
        *accelerator = ui::Accelerator(ui::VKEY_C, ui::EF_CONTROL_DOWN);
        return true;
    case IDS_APP_PASTE:
        *accelerator = ui::Accelerator(ui::VKEY_V, ui::EF_CONTROL_DOWN);
        return true;
    }
    return GetWidget()->GetAccelerator(command_id, accelerator);
}

void TextEdit::ExecuteCommand( int command_id, int event_flags )
{
   // ScopedFreeze freeze(this, GetTextObjectModel());
    switch (command_id) {
    case IDS_APP_UNDO:       Undo();           break;
    case IDS_APP_CUT:        Cut();            break;
    case IDS_APP_COPY:       Copy();           break;
    case IDS_APP_PASTE:      Paste();          break;
    case IDS_APP_SELECT_ALL: SetSelAll(); break;
    default:                 NOTREACHED();     break;
    }
}

void TextEdit::SetFont()
{
    SendMessage(WM_SETFONT,
        reinterpret_cast<WPARAM>(
        font_list_.GetPrimaryFont().GetNativeFont()),
        TRUE);
    // Setting the font blows away any text color we've set, so reset it.
    SetTextColor();
}

void TextEdit::SetTextColor()
{
    CHARFORMAT cf = {0};
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = skia::SkColorToCOLORREF(GetTextColor());
    CRichEditCtrl::SetDefaultCharFormat(cf);
}

SkColor TextEdit::GetTextColor() const {
    if (!use_default_text_color_)
        return text_color_;

    return GetNativeTheme()->GetSystemColor(
        ui::NativeTheme::kColorId_TextfieldDefaultColor);
}

void TextEdit::SetTextColor(SkColor color) {
    text_color_ = color;
    use_default_text_color_ = false;
}

void TextEdit::OnNCPaint( HRGN region )
{
    HDC hdc = GetWindowDC();

    CRect window_rect;
    GetWindowRect(&window_rect);
    // Convert to be relative to 0x0.
    window_rect.MoveToXY(0, 0);

    ExcludeClipRect(hdc,
        window_rect.left + content_insets_.left(),
        window_rect.top + content_insets_.top(),
        window_rect.right - content_insets_.right(),
        window_rect.bottom - content_insets_.bottom());

    HBRUSH brush = CreateSolidBrush(bg_color_);
    FillRect(hdc, &window_rect, brush);
    DeleteObject(brush);

    int part;
    int state;

    if (base::win::GetVersion() < base::win::VERSION_VISTA) {
        part = EP_EDITTEXT;

    if (contains_mouse_)
        state = ETS_HOT;
    else
        state = ETS_NORMAL;
    } else {
        part = EP_EDITBORDER_HVSCROLL;

        if (GetFocus() == m_hWnd)
            state = EPSHV_FOCUSED;
        else if (contains_mouse_)
            state = EPSHV_HOT;
        else
            state = EPSHV_NORMAL;
        // Vista doesn't appear to have a unique state for readonly.
    }

    ui::NativeThemeWin::instance()->PaintTextField(hdc, part, state,
        0, &window_rect,
        bg_color_, false, true);

    // NOTE: I tried checking the transparent property of the theme and invoking
    // drawParentBackground, but it didn't seem to make a difference.

    ReleaseDC(hdc);
    SetMsgHandled(TRUE);
}

int TextEdit::OnNCCalcSize( BOOL w_param, LPARAM l_param )
{
    content_insets_.Set(0, 0, 0, 0);
    content_insets_ = CalculateInsets();
    if (w_param) {
        NCCALCSIZE_PARAMS* nc_params =
            reinterpret_cast<NCCALCSIZE_PARAMS*>(l_param);
        nc_params->rgrc[0].left += content_insets_.left();
        nc_params->rgrc[0].right -= content_insets_.right();
        nc_params->rgrc[0].top += content_insets_.top();
        nc_params->rgrc[0].bottom -= content_insets_.bottom();
    } else {
        RECT* rect = reinterpret_cast<RECT*>(l_param);
        rect->left += content_insets_.left();
        rect->right -= content_insets_.right();
        rect->top += content_insets_.top();
        rect->bottom -= content_insets_.bottom();
    }
    return 0;
}

gfx::Insets TextEdit::CalculateInsets() {
    return gfx::Insets(3, 3, 3, 3);
}

void TextEdit::OnSetFocus( HWND hwnd )
{
    SetMsgHandled(FALSE);  // We still want the default processing of the message.

    views::FocusManager* focus_manager = GetFocusManager();
    if (!focus_manager) {
        NOTREACHED();
        return;
    }
    focus_manager->SetFocusedView(this);

    DefWindowProc();

    SetMsgHandled(TRUE);
}

void TextEdit::OnMouseMove( UINT keys, const CPoint& point )
{
    SetContainsMouse(true);
}

void TextEdit::OnMouseLeave()
{
    SetContainsMouse(false);
}

void TextEdit::SetContainsMouse(bool contains_mouse) {
    if (contains_mouse == contains_mouse_)
        return;

    contains_mouse_ = contains_mouse;

    if (contains_mouse_) {
        // Register for notification when the mouse leaves. Need to do this so
        // that we can reset contains mouse properly.
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        tme.dwHoverTime = 0;
        TrackMouseEvent(&tme);
    }
    RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_FRAME);
}

}