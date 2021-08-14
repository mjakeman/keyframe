#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_CANVAS (keyframe_canvas_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeCanvas, keyframe_canvas, KEYFRAME, CANVAS, GtkWidget)

struct _KeyframeCanvasClass
{
  GtkWidgetClass parent_class;
};

KeyframeCanvas *keyframe_canvas_new (void);

G_END_DECLS
