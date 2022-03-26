#include "pch.h"

#include "BmlResourceScheme.h"
#include "CustomSchemeFactory.h"

CefRefPtr<CefResourceHandler> CustomSchemeFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request) {
  if (scheme_name == "bml")
    return new BmlResourceScheme();

  return nullptr;
}
