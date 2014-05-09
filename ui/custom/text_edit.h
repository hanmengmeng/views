#ifndef UI_CONTROL_TEXT_EDIT_H
#define UI_CONTROL_TEXT_EDIT_H

#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlmisc.h>

#include <string>
#include <vector>

#include "base/basictypes.h"
#include "ui/views/controls/native_control_win.h"
#include "ui/base/models/simple_menu_model.h"
#include "ui/gfx/font_list.h"

namespace views
{
class MenuRunner;

typedef int int32;

class TextEdit : public NativeControlWin
    , public CWindowImpl<TextEdit, CRichEditCtrl>
    , public ui::SimpleMenuModel::Delegate
    , public CRichEditCommands<TextEdit>
{
public:
    TextEdit();
    ~TextEdit();

    //DECLARE_WND_SUPERCLASS(NULL, CEdit::GetWndClassName())
    DECLARE_WND_SUPERCLASS(L"ViewsTextEdit", MSFTEDIT_CLASS);

    BEGIN_MSG_MAP_EX(TextEdit)
        MSG_WM_CREATE(OnCreate)
        MSG_WM_KEYDOWN(OnKeyDown)
        MSG_WM_CHAR(OnChar)
        MSG_WM_CONTEXTMENU(OnContextMenu)
        MSG_WM_NCPAINT(OnNCPaint)
        MSG_WM_NCCALCSIZE(OnNCCalcSize)
        MSG_WM_SETFOCUS(OnSetFocus)
        MSG_WM_MOUSEMOVE(OnMouseMove)
        MSG_WM_MOUSELEAVE(OnMouseLeave)
    END_MSG_MAP()

    void SetContent(const string16& new_contents);
    void AppendContent(const string16 &contents);
    void ClearContent();

    SkColor GetTextColor() const;
    void SetTextColor(SkColor color);

    // Implement NativeControlWin
    virtual void CreateNativeControl() override;

    // ui::SimpleMenuModel::Delegate:
    virtual bool IsCommandIdChecked(int command_id) const OVERRIDE;
    virtual bool IsCommandIdEnabled(int command_id) const OVERRIDE;
    virtual bool GetAcceleratorForCommandId(
        int command_id,
        ui::Accelerator* accelerator) OVERRIDE;
    virtual void ExecuteCommand(int command_id, int event_flags) OVERRIDE;

private:
    // Message handlers.
    void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnContextMenu(CWindow wnd, CPoint point);
    void OnNCPaint(HRGN region);
    int OnNCCalcSize(BOOL w_param, LPARAM l_param);
    void OnSetFocus(HWND hwnd);
    void OnMouseMove(UINT keys, const CPoint& point);
    void OnMouseLeave();

private:
    void BuildContextMenu();
    void SetFont();
    void SetTextColor();
    gfx::Insets CalculateInsets();
    void SetContainsMouse(bool contains_mouse);

private:
    string16 cache_text_;

    // The contents of the context menu for the edit.
    scoped_ptr<ui::SimpleMenuModel> context_menu_contents_;
    scoped_ptr<MenuRunner> context_menu_runner_;

    // The fonts used to render the text in the Textfield.
    gfx::FontList font_list_;

    // Whether the border is drawn.
    bool draw_border_;

    // Text color.  Only used if |use_default_text_color_| is false.
    SkColor text_color_;

    // Should we use the system text color instead of |text_color_|?
    bool use_default_text_color_;

    // Background color.  Only used if |use_default_background_color_| is false.
    SkColor background_color_;

    // Should we use the system background color instead of |background_color_|?
    bool use_default_background_color_;

    // Border insets.
    gfx::Insets content_insets_;

    COLORREF bg_color_;

    // If true, the mouse is over the edit.
    bool contains_mouse_;

    DISALLOW_COPY_AND_ASSIGN(TextEdit);

    static HMODULE loaded_libarary_module_;
};

}


#endif
