#include "pch.h"

#include "RenderProcessHandler.h"

void RenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> window = context->GetGlobal();

  // Create a new V8 string value. See the "Basic JS Types" section below.
  CefRefPtr<CefV8Value> browser_object =
      CefV8Value::CreateObject(nullptr, nullptr);

  // Add the string to the window object as "window.myval". See the "JS
  // Objects" section below.
  window->SetValue("browser", browser_object, V8_PROPERTY_ATTRIBUTE_NONE);
}
