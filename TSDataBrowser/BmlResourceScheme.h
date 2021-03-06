#pragma once

#include <fstream>
#include "pch.h"
#include "rapidxml.hpp"

class BmlResourceScheme : public CefResourceHandler {
 public:
  bool Open(CefRefPtr<CefRequest> request,
            bool& handle_request,
            CefRefPtr<CefCallback> callback) override;

  void GetResponseHeaders(CefRefPtr<CefResponse> response,
                          int64& response_length,
                          CefString& redirectUrl) override;

  void Cancel() override;

  bool Read(void* data_out,
            int bytes_to_read,
            int& bytes_read,
            CefRefPtr<CefResourceReadCallback> callback) override;

 private:
  std::stringstream xml_output;
  int file_size, file_pos;

  void CheckBML(rapidxml::xml_node<>* node);

  IMPLEMENT_REFCOUNTING(BmlResourceScheme);
};
