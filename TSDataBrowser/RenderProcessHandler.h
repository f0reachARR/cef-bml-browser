#pragma once

#include "pch.h"

class RenderProcessHandler : public CefRenderProcessHandler {
 public:
  void OnContextCreated(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) override;

  IMPLEMENT_REFCOUNTING(RenderProcessHandler);
};
