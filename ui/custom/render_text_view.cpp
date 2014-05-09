#include "render_text_view.h"

namespace views
{

RenderTextView::RenderTextView()
    : render_text_(gfx::RenderText::CreateInstance())
{
    render_text_->SetHorizontalAlignment(gfx::ALIGN_LEFT);
    render_text_->SetVerticalAlignment(gfx::ALIGN_TOP);
    render_text_->SetColor(SK_ColorBLACK);
    render_text_->SetMultiline(true);
    render_text_->SetCursorEnabled(true);
    set_border(Border::CreateEmptyBorder(5,5,5,5));
}

void RenderTextView::OnPaint( gfx::Canvas* canvas )
{
    View::OnPaint(canvas);
    render_text_->Draw(canvas);
}

void RenderTextView::Layout()
{
    SizeToPreferredSize();
}

gfx::Size RenderTextView::GetPreferredSize()
{
    render_text_->SetMultiline(false);
    int w = render_text_->GetContentWidth();
    render_text_->SetMultiline(true);
    if (parent() && w > parent()->width()) {
        w = parent()->width();
    }
    gfx::Size size(w, render_text_->GetStringSize().height());
    size.Enlarge(GetInsets().width(), GetInsets().height());

    return size;
}

int RenderTextView::GetHeightForWidth( int w )
{
    if (w == 0) {
        return View::GetHeightForWidth(w);
    }
    gfx::Rect rect = render_text_->display_rect();
    rect.set_width(w - GetInsets().width());
    render_text_->SetDisplayRect(rect);
    return render_text_->GetStringSize().height() + GetInsets().height();
}

void RenderTextView::OnBoundsChanged( const gfx::Rect& previous_bounds )
{
    gfx::Rect bounds = GetLocalBounds();
    bounds.Inset(GetInsets());
    render_text_->SetDisplayRect(bounds);
}

void RenderTextView::SetText( const string16& new_contents )
{
    // Color and style the text inside |test_range| to test colors and styles.
    //gfx::Range test_range(1, 21);
    //test_range.set_start(std::min(test_range.start(), new_contents.length()));
    //test_range.set_end(std::min(test_range.end(), new_contents.length()));

    render_text_->SetText(new_contents);
    //render_text_->SetColor(SK_ColorBLACK);
    //render_text_->ApplyColor(0xFFFF0000, test_range);
    //render_text_->SetStyle(gfx::DIAGONAL_STRIKE, false);
    //render_text_->ApplyStyle(gfx::DIAGONAL_STRIKE, true, test_range);
    //render_text_->SetStyle(gfx::UNDERLINE, false);
    //render_text_->ApplyStyle(gfx::UNDERLINE, true, test_range);

    InvalidateLayout();
}

}