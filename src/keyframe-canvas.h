#pragma once

#include <gtk/gtk.h>

#include "model/keyframe-composition.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_CANVAS (keyframe_canvas_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeCanvas, keyframe_canvas, KEYFRAME, CANVAS, GtkWidget)

struct _KeyframeCanvasClass
{
  GtkWidgetClass parent_class;
};

GtkWidget *keyframe_canvas_new (KeyframeComposition *composition);

G_END_DECLS
