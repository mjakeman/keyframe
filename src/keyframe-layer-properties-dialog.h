#pragma once

#include <gtk/gtk.h>

#include "model/keyframe-layers.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER_PROPERTIES_DIALOG (keyframe_layer_properties_dialog_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayerPropertiesDialog, keyframe_layer_properties_dialog, KEYFRAME, LAYER_PROPERTIES_DIALOG, GtkDialog)

struct _KeyframeLayerPropertiesDialogClass
{
  GtkDialogClass parent_class;
};

GtkWidget *keyframe_layer_properties_dialog_new (KeyframeLayer *layer);

G_END_DECLS
