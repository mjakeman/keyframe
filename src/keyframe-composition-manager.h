#pragma once

#include <glib-2.0/glib-object.h>

#include "model/keyframe-composition.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_COMPOSITION_MANAGER (keyframe_composition_manager_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeCompositionManager, keyframe_composition_manager, KEYFRAME, COMPOSITION_MANAGER, GObject)

struct _KeyframeCompositionManagerClass
{
  GObjectClass parent_class;
};

KeyframeCompositionManager *keyframe_composition_manager_new (void);

KeyframeComposition *
keyframe_composition_manager_new_composition (KeyframeCompositionManager *self);

KeyframeComposition *
keyframe_composition_manager_get_current (KeyframeCompositionManager *self);

void
keyframe_composition_manager_make_current (KeyframeCompositionManager *self,
                                           KeyframeComposition        *composition);

G_END_DECLS
