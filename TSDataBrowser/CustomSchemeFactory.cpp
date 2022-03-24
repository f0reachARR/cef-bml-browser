#include "CustomSchemeFactory.h"

#include "BmlResourceScheme.h"

CefRefPtr<CefResourceHandler> CustomSchemeFactory::Create(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& scheme_name,
    CefRefPtr<CefRequest> request) {
  if (scheme_name == "bml")
    return new BmlResourceScheme();

  return nullptr;
}
