#ifndef __LIBXMLPP_EXAMPLES_MYPDFPARSER_H
#define __LIBXMLPP_EXAMPLES_MYPDFPARSER_H

#include <libxml++-2.6/libxml++/libxml++.h>
#include <string>
#include "TextElement.h"


using namespace std;
class MyPdfParser : public xmlpp::SaxParser
{
int line_top  = -1;
int line_right = -1;
int line_bottom = -1;
int line_left = -1;
int prev_top = -1;
int prev_right = -1;
int prev_bottom = -1;
int prev_left = -1;
int bottom  = -1;
int top = -1;
int left_ = -1;
int  right_ = -1;
bool first_in_line =true;
bool new_element = false;
std::vector<std::vector<TextElement> > * pageElements;
string current_element = "";
int current_element_left = -1;
int current_element_right = -1;
int current_element_top = -1;
int current_element_bottom = -1;
std::vector<TextElement>  currElem ;
public:
  MyPdfParser(std::vector<std::vector<TextElement>> *&);
  virtual ~MyPdfParser();

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


#endif //__LIBXMLPP_EXAMPLES_MYPDFPARSER_H
