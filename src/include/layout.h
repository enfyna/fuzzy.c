#ifndef LAYOUT_H
#define LAYOUT_H

#include "raylib.h"

typedef Rectangle LayoutBox;

LayoutBox layout_row(LayoutBox root, int child_count, int id);
LayoutBox layout_col(LayoutBox root, int child_count, int id);
LayoutBox layout_pad(LayoutBox box, double padding);

#endif // LAYOUT_H
