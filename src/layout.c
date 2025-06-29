#include "layout.h"
#include <assert.h>

LayoutBox layout_row(LayoutBox root, int child_count, int id)
{
    assert(child_count > 0);
    return (LayoutBox) {
        .x = root.x + (root.width / child_count) * id,
        .width = root.width / child_count,

        .y = root.y,
        .height = root.height,
    };
}

LayoutBox layout_col(LayoutBox root, int child_count, int id)
{
    assert(child_count > 0);
    return (LayoutBox) {
        .x = root.x,
        .width = root.width,

        .y = root.y + (root.height / child_count) * id,
        .height = root.height / child_count,
    };
}

LayoutBox layout_pad(LayoutBox box, double padding)
{
    return (LayoutBox)
    {
        .x = box.x + padding,
        .width = box.width - padding * 2,

        .y = box.y + padding,
        .height = box.height - padding * 2,
    };
}
