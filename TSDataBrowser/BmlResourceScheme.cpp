#include "BmlResourceScheme.h"
#include <fstream>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

bool BmlResourceScheme::Open(CefRefPtr<CefRequest> request,
                             bool& handle_request,
                             CefRefPtr<CefCallback> callback) {
  LOG(INFO) << "BML Request: " << request->GetURL();
  std::ifstream input("../x64/Debug/sampledata/40/0000/startup.bml",
                      std::ios::binary);

  if (input) {
    std::stringstream buffer;
    buffer << input.rdbuf();

    input.close();

    std::string xml = buffer.str();

    rapidxml::xml_document<char> doc;
    try {
      doc.parse<0>(&xml[0]);
    } catch (rapidxml::parse_error& ex) {
      int position = (int)ex.where<char>() - (int)&xml[0];

      LOG(INFO) << "parse error (" << position << ") : " << ex.what()
                << std::endl;
      return false;
    }

    CheckBML(doc.first_node());

    rapidxml::print(std::ostream_iterator<char>(xml_output), doc,
                    rapidxml::print_script_noexpand);

    handle_request = true;
    return true;
  } else {
    return false;
  }
}

void BmlResourceScheme::GetResponseHeaders(CefRefPtr<CefResponse> response,
                                           int64& response_length,
                                           CefString& redirectUrl) {
  response->SetCharset("EUC-JP");
  response->SetMimeType("text/html");
  response->SetStatus(200);
  xml_output.seekg(0, std::ios::end);
  file_size = response_length = xml_output.tellg();
  xml_output.seekg(0, std::ios::beg);
}

void BmlResourceScheme::Cancel() {}

bool BmlResourceScheme::Read(void* data_out,
                             int bytes_to_read,
                             int& bytes_read,
                             CefRefPtr<CefResourceReadCallback> callback) {
  int remain = file_size - file_pos;
  auto size = std::min(remain, bytes_to_read);
  if (size > 0) {
    xml_output.read((char*)data_out, size);
    size = (int)xml_output.tellg() - file_pos;
    bytes_read = size;
    file_pos += size;
  } else {
    bytes_read = 0;
    return false;
  }

  return true;
}

void BmlResourceScheme::CheckBML(rapidxml::xml_node<>* node) {
  auto first_node = node->first_node();
  while (first_node) {
    CheckBML(first_node);
    first_node = first_node->next_sibling();
  }

  if (node->type() == rapidxml::node_type::node_cdata) {
    node->type(rapidxml::node_data);
  }

  if (node->value_size() == 0 && !node->first_node()) {
    node->append_node(
        node->document()->allocate_node(rapidxml::node_data, "", ""));
  }
}
