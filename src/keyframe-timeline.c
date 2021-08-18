#include "keyframe-timeline.h"

#include "keyframe-composition-manager.h"
#include "keyframe-layer-properties-dialog.h"
#include "model/keyframe-composition.h"
#include "model/keyframe-layers.h"

typedef struct
{
    // For dispose
    GtkWidget *sw;

    GtkWidget *col_view;
    KeyframeCompositionManager *manager;
    KeyframeComposition *composition;

    ulong signal_id;
    ulong comp_update_id;
} KeyframeTimelinePrivate;

G_DEFINE_TYPE_WITH_PRIVATE (KeyframeTimeline, keyframe_timeline, GTK_TYPE_WIDGET)

enum {
    PROP_0,
    PROP_MANAGER,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

/**
 * keyframe_timeline_new:
 *
 * Create a new #KeyframeTimeline.
 *
 * Returns: (transfer full): a newly created #KeyframeTimeline
 */
KeyframeTimeline *
keyframe_timeline_new (KeyframeCompositionManager *manager)
{
    return g_object_new (KEYFRAME_TYPE_TIMELINE,
                         "manager", manager,
                         NULL);
}

static void
keyframe_timeline_finalize (GObject *object)
{
    KeyframeTimeline *self = (KeyframeTimeline *)object;
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    gtk_widget_unparent (priv->sw);

    G_OBJECT_CLASS (keyframe_timeline_parent_class)->finalize (object);
}

static void
keyframe_timeline_update_composition (KeyframeTimeline *self);

static void
keyframe_timeline_get_property (GObject    *object,
                                guint       prop_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
    KeyframeTimeline *self = KEYFRAME_TIMELINE (object);

    switch (prop_id)
      {
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
cb_current_changed (KeyframeCompositionManager *manager,
                    KeyframeTimeline           *self)
{
    keyframe_timeline_update_composition (self);
}

static void
keyframe_timeline_set_property (GObject      *object,
                                guint         prop_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
    KeyframeTimeline *self = KEYFRAME_TIMELINE (object);
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    switch (prop_id)
      {
      case PROP_MANAGER:
          if (priv->manager)
              g_signal_handler_disconnect (priv->manager, priv->signal_id);
          priv->manager = g_value_get_object (value);
          priv->signal_id = g_signal_connect (priv->manager, "current-changed", cb_current_changed, self);
          keyframe_timeline_update_composition (self);
          break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      }
}

static void
keyframe_timeline_class_init (KeyframeTimelineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_timeline_finalize;
    object_class->get_property = keyframe_timeline_get_property;
    object_class->set_property = keyframe_timeline_set_property;

    properties [PROP_MANAGER] =
        g_param_spec_object ("manager", "Manager", "Manager",
                             KEYFRAME_TYPE_COMPOSITION_MANAGER,
                             G_PARAM_WRITABLE);

    g_object_class_install_properties (object_class, N_PROPS, properties);
}

static GListModel *
create_layers_model_from_composition (KeyframeComposition *composition)
{
    if (composition == NULL)
        return NULL;

    g_return_if_fail (KEYFRAME_IS_COMPOSITION (composition));

    GListModel *model = g_list_store_new (KEYFRAME_TYPE_LAYER);

    GSList *layers = keyframe_composition_get_layers (composition);
    for (GSList *l = layers; l != NULL; l = l->next)
    {
        KeyframeLayer *layer = l->data;
        g_assert (KEYFRAME_IS_LAYER (layer));

        g_list_store_append (model, layer);
    }

    return G_LIST_MODEL (model);
}

static GListModel *
create_layers_tree (gpointer layer_ptr, gpointer unused)
{
    // TODO: Add support for sublayers at some point
    return NULL;
}

static void
setup_listitem_cb (GtkListItemFactory *factory,
                   GtkListItem        *list_item)
{
    GtkWidget *label = gtk_label_new ("");
    gtk_label_set_xalign (GTK_LABEL (label), 0);
    gtk_list_item_set_child (list_item, label);
}

static void
bind_listitem_cb (GtkListItemFactory *factory,
                  GtkListItem        *list_item,
                  const char         *prop)
{
    GtkWidget *label;
    KeyframeLayer *obj;
    label = gtk_list_item_get_child (list_item);
    obj = gtk_tree_list_row_get_item (gtk_list_item_get_item (list_item));

    char *layer_prop;
    g_object_get (obj,
                  prop, &layer_prop,
                  NULL);

    gtk_label_set_label (GTK_LABEL (label), layer_prop);
}

static void
keyframe_timeline_composition_changed (KeyframeComposition *composition,
                                       KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);
    g_assert (composition == priv->composition);

    // Update List model
    GListModel *model = create_layers_model_from_composition (priv->composition);

    GtkTreeListModel *treemodel = gtk_tree_list_model_new (model, FALSE, TRUE, create_layers_tree, NULL, NULL);
    GtkSingleSelection *selection = gtk_single_selection_new (G_LIST_MODEL (treemodel));

    gtk_column_view_set_model (GTK_COLUMN_VIEW (priv->col_view), GTK_SELECTION_MODEL (selection));
}

static void
keyframe_timeline_update_composition (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (priv->manager) {
        KeyframeComposition *new_comp = keyframe_composition_manager_get_current (priv->manager);

        // No change
        if (new_comp == priv->composition)
            return;

        if (priv->composition)
        {
            g_signal_handler_disconnect (priv->composition, priv->comp_update_id);
            g_object_unref (priv->composition);
        }

        priv->composition = g_object_ref (new_comp);
        priv->comp_update_id = g_signal_connect (priv->composition, "changed", keyframe_timeline_composition_changed, self);
    }

    if (priv->composition == NULL)
    {
        gtk_column_view_set_model (GTK_COLUMN_VIEW (priv->col_view), NULL);
        return;
    }

    // Regenerate List Model
    keyframe_timeline_composition_changed (priv->composition, self);
}

static void
cb_new_text_layer (GtkButton *btn, KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return;

    // TODO: This should really be an action
    KeyframeLayer *layer = g_object_new (KEYFRAME_TYPE_LAYER_TEXT,
                                         "name", "Untitled Layer",
                                         "markup", "Hello World",
                                         NULL);
    keyframe_composition_push_layer (priv->composition, layer);
}

static void
cb_new_cool_layer (GtkButton *btn, KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return;

    // TODO: This should really be an action
    KeyframeLayer *layer = g_object_new (KEYFRAME_TYPE_LAYER_COOL,
                                         "name", "Untitled Layer",
                                         NULL);
    keyframe_composition_push_layer (priv->composition, layer);
}

static void
cb_new_geometry_layer (GtkButton *btn, KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return;

    int w, h;
    g_object_get (priv->composition, "width", &w, "height", &h, NULL);

    // TODO: This should really be an action
    KeyframeLayer *layer = g_object_new (KEYFRAME_TYPE_LAYER_GEOMETRY,
                                         "name", "Untitled Layer",
                                         "width", (float)w,
                                         "height", (float)h,
                                         NULL);
    keyframe_composition_push_layer (priv->composition, layer);
}

static void
cb_new_layer_popover (GtkMenuButton *new_layer_btn, KeyframeTimeline *self)
{
    // New Layer Popover
    GtkWidget *popover_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

    /*GtkWidget *popover_create_btn = gtk_button_new_with_label ("Create");
    gtk_widget_add_css_class (popover_create_btn, "suggested-action");
    gtk_box_append (GTK_BOX (popover_box), popover_create_btn);*/

    GtkWidget *layer_btn = gtk_button_new_with_label ("Text");
    g_signal_connect (layer_btn, "clicked", cb_new_text_layer, self);
    gtk_box_append (GTK_BOX (popover_box), layer_btn);

    layer_btn = gtk_button_new_with_label ("Geometry");
    g_signal_connect (layer_btn, "clicked", cb_new_geometry_layer, self);
    gtk_box_append (GTK_BOX (popover_box), layer_btn);

    layer_btn = gtk_button_new_with_label ("Cool");
    g_signal_connect (layer_btn, "clicked", cb_new_cool_layer, self);
    gtk_box_append (GTK_BOX (popover_box), layer_btn);

    GtkWidget *popover = gtk_popover_new ();
    gtk_popover_set_child (popover, popover_box);

    gtk_menu_button_set_popover (new_layer_btn, popover);
}

static KeyframeLayer *
get_current_layer (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    if (!priv->composition)
        return NULL;

    GtkSelectionModel *selection = gtk_column_view_get_model (priv->col_view);
    GtkTreeListRow *tree_row = GTK_TREE_LIST_ROW (gtk_single_selection_get_selected_item (GTK_SINGLE_SELECTION (selection)));
    KeyframeLayer *layer = KEYFRAME_LAYER (gtk_tree_list_row_get_item (tree_row));

    if (!(KEYFRAME_IS_LAYER (layer)))
        return NULL;

    return layer;
}

static void
cb_delete_layer (GtkButton *btn, KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);
    KeyframeLayer *layer = get_current_layer (self);

    if (!layer)
        return;

    keyframe_composition_delete_layer (priv->composition, layer);

    // self->composition ... delete?
    // layer should probably have a reference to composition
    //   -> e.g. composition owns layers and is responsible for freeing them
}

static void
cb_edit_layer (GtkButton *btn, KeyframeTimeline *self)
{
    KeyframeLayer *layer = get_current_layer (self);

    if (!layer)
        return;

    GtkWidget *dlg = keyframe_layer_properties_dialog_new (layer);
    gtk_window_set_transient_for (GTK_WINDOW (dlg), GTK_WINDOW (gtk_widget_get_root (self)));
    gtk_window_set_modal (GTK_WINDOW (dlg), TRUE);

    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);
    g_signal_connect_swapped (dlg, "response", keyframe_composition_invalidate, priv->composition);

    gtk_widget_show (dlg);
}

static void
keyframe_timeline_init (KeyframeTimeline *self)
{
    KeyframeTimelinePrivate *priv = keyframe_timeline_get_instance_private (self);

    GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_layout_manager (GTK_WIDGET (self), gtk_bin_layout_new ());
    gtk_widget_set_parent (box, GTK_WIDGET (self));

    // Toolbar
    GtkWidget *toolbar = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_add_css_class (toolbar, "toolbar");
    gtk_box_append (GTK_BOX (box), toolbar);

    GtkWidget *add_layer_btn = gtk_menu_button_new ();
    gtk_menu_button_set_icon_name (GTK_MENU_BUTTON (add_layer_btn), "document-new-symbolic");
    gtk_menu_button_set_direction (GTK_MENU_BUTTON (add_layer_btn), GTK_ARROW_UP);
    gtk_menu_button_set_create_popup_func (GTK_MENU_BUTTON (add_layer_btn),
                                           cb_new_layer_popover,
                                           g_object_ref (self),
                                           g_object_unref);
    gtk_box_append (GTK_BOX (toolbar), add_layer_btn);

    GtkWidget *delete_layer_btn = gtk_button_new ();
    gtk_button_set_child (GTK_BUTTON (delete_layer_btn), gtk_image_new_from_icon_name ("user-trash-symbolic"));
    g_signal_connect (delete_layer_btn, "clicked", cb_delete_layer, self);
    gtk_box_append (GTK_BOX (toolbar), delete_layer_btn);

    GtkWidget *edit_layer_btn = gtk_button_new ();
    gtk_button_set_child (GTK_BUTTON (edit_layer_btn), gtk_image_new_from_icon_name ("document-edit-symbolic"));
    g_signal_connect (edit_layer_btn, "clicked", cb_edit_layer, self);
    gtk_box_append (GTK_BOX (toolbar), edit_layer_btn);

    // Timeline
    priv->sw = gtk_scrolled_window_new ();
    gtk_widget_set_hexpand (priv->sw, true);
    gtk_box_append (box, priv->sw);

    priv->col_view = GTK_COLUMN_VIEW (gtk_column_view_new (NULL));
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (priv->sw), GTK_WIDGET (priv->col_view));
    gtk_scrolled_window_set_min_content_height (GTK_SCROLLED_WINDOW (priv->sw), 200);

    gtk_column_view_set_reorderable (priv->col_view, false);
    gtk_column_view_set_show_column_separators (priv->col_view, true);

    GtkListItemFactory *name_factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (name_factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (name_factory, "bind", bind_listitem_cb, "name");

    GtkListItemFactory *type_factory = gtk_signal_list_item_factory_new ();
    g_signal_connect (type_factory, "setup", setup_listitem_cb, NULL);
    g_signal_connect (type_factory, "bind", bind_listitem_cb, "type");

    GtkColumnViewColumn* col1 = gtk_column_view_column_new("Layer Name", name_factory);
    gtk_column_view_column_set_resizable (col1, true);
    gtk_column_view_append_column (priv->col_view, col1);

    GtkColumnViewColumn* col2 = gtk_column_view_column_new("Type", type_factory);
    gtk_column_view_column_set_resizable (col2, true);
    gtk_column_view_append_column (priv->col_view, col2);

    GtkColumnViewColumn* col3 = gtk_column_view_column_new("Graph", name_factory);
    gtk_column_view_column_set_resizable (col3, true);
    gtk_column_view_append_column (priv->col_view, col3);
    gtk_column_view_column_set_expand (col3, true);
}
