#include "mypdfparser.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

using namespace std;

MyPdfParser::MyPdfParser(std::vector<std::vector<TextElement> > *& te)
  : xmlpp::SaxParser()
{
  pageElements = te;
}

MyPdfParser::~MyPdfParser()
{
}
void MyPdfParser::split(const string& s, char c,
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

void MyPdfParser::on_start_document()
{
  //std::cout << "on_start_document()" << std::endl;
}

void MyPdfParser::on_end_document()
{
  pageElements->push_back(currElem);
  //std::cout << "on_end_document()" << std::endl;
}

void MyPdfParser::on_start_element(const Glib::ustring& name,
                                   const AttributeList& attributes)
{
  //std::cout << "node name=" << name << std::endl;
  //std::cout << "node name=" <<name.compare("span")<< std::endl;

  if (name.compare("page") == 0) {
    new_element = true;
	 // cout<<currElem.size()<<"  kdjflkkljd  "<<endl;
    if (currElem.size()  != 0) {
  	  pageElements->push_back(currElem);
	   currElem.clear();
   }
    line_left = left_;
    line_bottom = bottom;
    line_right = right_;
    line_top  = top;
    first_in_line = true;
	
  } else if (name.compare("text") == 0 ) {
    new_element = true;
	  //std::cout << "attribute" << attributes[0].value << std::endl;
	  //std::cout << "attribute" << attributes[2].value << std::endl;
    //std::string bbox = attributes[2].value.split(";")[0];
    ////std::cout << "attribute" << bbox << std::endl;

    prev_left = left_;
    prev_bottom = bottom;
    prev_right = right_;
    prev_top = top;

    top = atoi(attributes[0].value.c_str());
    left_ = atoi(attributes[1].value.c_str());
    bottom = atoi(attributes[3].value.c_str()) + top;
    right_ = atoi(attributes[2].value.c_str()) + left_;


  }

}

void MyPdfParser::on_end_element(const Glib::ustring& name)
{
  ////std::cout << "on_end_element()" << std::endl;
}

void MyPdfParser::on_characters(const Glib::ustring& text)
{

  std::string whitespaces (" \t\f\v\n\r");
  //std::cout <<" jkd "<< text.size()<<"   "<<(text.find_first_not_of(whitespaces) == std::string::npos)  << "      "<< text <<std::endl;
  
    //std::cout<< first_in_line<< " "<<prev_right<< "  " << left_ << "  "<< line_bottom << "  "<<line_top<<" " <<line_bottom - line_top << " " <<left_ - prev_right<<endl;
    //std::cout << " " << text << " ";
  
    if( new_element || (first_in_line ) ) {
    //  std::cout << "One Element "<<current_element << " " <<current_element_left << "  "<<current_element_right << "  "<<current_element_top<<" "<<current_element_bottom<<endl;
    current_element = text;
    current_element_right = right_;
    current_element_top = top;
    current_element_bottom = bottom;
      current_element_left = left_;
      if (!current_element.empty()) {
        TextElement* t = new TextElement(current_element,current_element_top,current_element_left,(current_element_right-current_element_left),(current_element_bottom - current_element_top),-1,"text","text");
        currElem.push_back(*t);
		   // std::cout<<" - "<<currElem.size();
      }
      
    } 

 
  first_in_line = false;
  new_element = false;
}

void MyPdfParser::on_comment(const Glib::ustring& text)
{
  //std::cout << "on_comment(): " << text << std::endl;
}

void MyPdfParser::on_warning(const Glib::ustring& text)
{
  //std::cout << "on_warning(): " << text << std::endl;
}

void MyPdfParser::on_error(const Glib::ustring& text)
{
  //std::cout << "on_error(): " << text << std::endl;
}

void MyPdfParser::on_fatal_error(const Glib::ustring& text)
{
  //std::cout << "on_fatal_error(): " << text << std::endl;
}

