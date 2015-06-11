#ifndef __LIBXMLPP_EXAMPLES_MYPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPARSER_H

#include <libxml++-2.6/libxml++/libxml++.h>
#include <string>
#include "TextElement.h"


using namespace std;
class MySaxParser : public xmlpp::SaxParser
{
public:
  MySaxParser(std::vector<TextElement>*&);
  virtual ~MySaxParser();

protected:
  //overrides:
  virtual void on_start_document();
  virtual void on_end_document();
  virtual void on_start_element(const Glib::ustring& name,
                                const AttributeList& properties);
  virtual void on_end_element(const Glib::ustring& name);
  virtual void on_characters(const Glib::ustring& characters);
  virtual void on_comment(const Glib::ustring& text);
  virtual void on_warning(const Glib::ustring& text);
  virtual void on_error(const Glib::ustring& text);
  virtual void on_fatal_error(const Glib::ustring& text);
  void split(const string& s, char c,vector<string>& v);
};


#endif //__LIBXMLPP_EXAMPLES_MYPARSER_H
