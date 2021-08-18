#pragma once

#include <glib-2.0/glib-object.h>

#include "keyframe-layer.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER_TEXT (keyframe_layer_text_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayerText, keyframe_layer_text, KEYFRAME, LAYER_TEXT, KeyframeLayer)

struct _KeyframeLayerTextClass
{
  KeyframeLayerClass parent_class;
};

KeyframeLayer *keyframe_layer_text_new (const char *name, const char *markup);

G_END_DECLS
