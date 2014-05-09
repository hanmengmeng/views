#ifndef UI_CONTROL_RENDER_TEXT_VIEW_H
#define UI_CONTROL_RENDER_TEXT_VIEW_H

#include "base/memory/scoped_ptr.h"
#include "ui/views/view.h"
#include "ui/gfx/render_text.h"

namespace views
{

class RenderTextView : public View
{
public:
    RenderTextView();

    // Implement View
    virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
    virtual void Layout() OVERRIDE;
    virtual gfx::Size GetPreferredSize() OVERRIDE;
    virtual int GetHeightForWidth(int w) OVERRIDE;
    virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

    void SetText(const string16& new_contents);

private:
    scoped_ptr<gfx::RenderText> render_text_;
    DISALLOW_COPY_AND_ASSIGN(RenderTextView);
};

}

#endif
