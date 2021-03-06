/* keyframe-application.h
 *
 * Copyright 2021 Matthew Jakeman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <gtk/gtk.h>
#include <adwaita.h>
#include <libpanel.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_APPLICATION (keyframe_application_get_type())

G_DECLARE_FINAL_TYPE (KeyframeApplication, keyframe_application, KEYFRAME, APPLICATION, AdwApplication)

KeyframeApplication *keyframe_application_new (gchar *application_id,
                                               GApplicationFlags  flags);

G_END_DECLS
