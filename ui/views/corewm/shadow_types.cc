// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/views/corewm/shadow_types.h"
#if defined(USE_AURA)
#include "ui/aura/window_property.h"

DECLARE_WINDOW_PROPERTY_TYPE(views::corewm::ShadowType);

namespace views {
namespace corewm {

void SetShadowType(aura::Window* window, ShadowType shadow_type) {
  window->SetProperty(kShadowTypeKey, shadow_type);
}

ShadowType GetShadowType(aura::Window* window) {
  return window->GetProperty(kShadowTypeKey);
}

DEFINE_WINDOW_PROPERTY_KEY(ShadowType, kShadowTypeKey, SHADOW_TYPE_NONE);

}  // namespace corewm
}  // namespace views
#endif