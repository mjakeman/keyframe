#pragma once

#include <gtk/gtk.h>

#include "keyframe-composition-manager.h"
#include "model/keyframe-layers.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER_LIST (keyframe_layer_list_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayerList, keyframe_layer_list, KEYFRAME, LAYER_LIST, GtkWidget)

struct _KeyframeLayerListClass
{
  GtkWidgetClass parent_class;
};

KeyframeLayerList *keyframe_layer_list_new (KeyframeCompositionManager *manager);

G_END_DECLS
