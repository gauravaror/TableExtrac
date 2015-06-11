
#include <string>
#include <vector> 

using namespace std;

class TextElement {
	public:
string value;
int top;
int left;
int width;
int height;
int right;
int font;
string format ;
string typ;
int count_lines;
vector<string>* elements;
int last_top; 
int first_top; 
int colspan;
bool artificial;

  TextElement(string v,int t,int l,int w,int h,int f,string f2,string t2) {
  	value = v;
  	top = t;
  	left = l;
  	width = w;
	  right = l+w;
  	height = h;
  	font = f;  
  	format = f2;	
  	typ = t2;
    last_top = t; // no line merged to this text element
    first_top = t;
    colspan = 1;
    count_lines = 1;
    elements = new vector<string>();
    right = left + width;
    artificial = false;
  }	
  
   TextElement() {  	
  	value = "null";
    colspan = 1;  	
    count_lines = 1;
    artificial = true;
  }

   TextElement(string s) {
  	value = s;
    colspan = 1;  	
    count_lines = 1;
    artificial = false;
  }  
  
};