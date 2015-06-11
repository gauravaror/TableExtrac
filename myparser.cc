#include "myparser.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
using namespace std;
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
vector<TextElement> *textElements;

string current_element = "";
int current_element_left = -1;
int current_element_right = -1;
int current_element_top = -1;
int current_element_bottom = -1;


MySaxParser::MySaxParser(vector<TextElement> *& te)
  : xmlpp::SaxParser()
{
  textElements = te;
}

MySaxParser::~MySaxParser()
{
}
void MySaxParser::split(const string& s, char c,
           vector<string>& v) {
   string::size_type i = 0;
   string::size_type j = s.find(c);

   while (j != string::npos) {
      v.push_back(s.substr(i, j-i));
      i = ++j;
      j = s.find(c, j);

      if (j == string::npos)
         v.push_back(s.substr(i, s.length()));
   }
}

void MySaxParser::on_start_document()
{
  std::cout << "on_start_document()" << std::endl;
}

void MySaxParser::on_end_document()
{
  std::cout << "on_end_document()" << std::endl;
}

void MySaxParser::on_start_element(const Glib::ustring& name,
                                   const AttributeList& attributes)
{
  //std::cout << "node name=" << name << std::endl;
  //std::cout << "node name=" <<name.compare("span")<< std::endl;

  if (name.compare("span") == 0) {
    new_element = true;
	  //std::cout << "attribute" << attributes[0].value << std::endl;
	  //std::cout << "attribute" << attributes[2].value << std::endl;
    //std::string bbox = attributes[2].value.split(";")[0];
    //std::cout << "attribute" << bbox << std::endl;
    vector<std::string> * n = new vector<std::string>();
    split(attributes[2].value,';',*n);
    vector<std::string> * item = new vector<std::string>();
    split(n->front(),' ',*item);
   // std::cout<< "New : "<<attributes[2].value<<std::endl;

    prev_left = left_;
    prev_bottom = bottom;
    prev_right = right_;
    prev_top = top;

    bottom = atoi(item->back().c_str());
    item->pop_back();
    right_ = atoi(item->back().c_str());
    item->pop_back();
    top = atoi(item->back().c_str());
    item->pop_back();
    left_ = atoi(item->back().c_str());
    item->pop_back();


    //std::cout << "attribute  " <<left_<<"  "<< top << "  "<< right_ << "  "<<bottom << std::endl;
    if(attributes[0].value.compare("ocr_line") == 0) {
      std::cout<<std::endl;
      line_left = left_;
      line_bottom = bottom;
      line_right = right_;
      line_top  = top;
      first_in_line = true;

     // std::cout<< "New Line: "<<attributes[2].value<<std::endl;
    } 
  }

}

void MySaxParser::on_end_element(const Glib::ustring& name)
{
  //std::cout << "on_end_element()" << std::endl;
}

void MySaxParser::on_characters(const Glib::ustring& text)
{

  std::string whitespaces (" \t\f\v\n\r");
  //std::cout <<" jkd "<< text.size()<<"   "<<(text.find_first_not_of(whitespaces) == std::string::npos)  << "      "<< text <<std::endl;
  
  if((text.find_first_not_of(whitespaces) == std::string::npos) == 0 ) {
   // std::cout<< first_in_line<< " "<<prev_right<< "  " << left_ << "  "<< line_bottom << "  "<<line_top<<" " <<line_bottom - line_top << " " <<left_ - prev_right<<endl;
    //std::cout << " " << text << " ";
  
    if( new_element && (first_in_line || (left_ - prev_right) > (line_bottom - line_top)) ) {
//      std::cout << "One Element "<<current_element << " " <<current_element_left << "  "<<current_element_right << "  "<<current_element_top<<" "<<current_element_bottom<<endl;
      if (!current_element.empty()) {
        TextElement* t = new TextElement(current_element,current_element_top,current_element_left,(current_element_right-current_element_left),(current_element_bottom - current_element_top),-1,"text","text");
        textElements->push_back(*t);
      }
      current_element = "";
      current_element_left = left_;
      
    } 
    current_element += " " +text;
    current_element_right = right_;
    current_element_top = line_top;
    current_element_bottom = line_bottom;

  }
  first_in_line = false;
  new_element = false;
}

void MySaxParser::on_comment(const Glib::ustring& text)
{
  //std::cout << "on_comment(): " << text << std::endl;
}

void MySaxParser::on_warning(const Glib::ustring& text)
{
  std::cout << "on_warning(): " << text << std::endl;
}

void MySaxParser::on_error(const Glib::ustring& text)
{
  std::cout << "on_error(): " << text << std::endl;
}

void MySaxParser::on_fatal_error(const Glib::ustring& text)
{
  std::cout << "on_fatal_error(): " << text << std::endl;
}

