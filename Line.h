class Line {
public:
  std::vector<TextElement *> * texts;
  
  int top;
  int bottom;
  int height;
  int leftmost;
  int rightmost;
  int font;  
  int last_top;
  int first_top;
  int used_space;
  string typ;
  
  public:
   Line() {
    texts = new std::vector<TextElement *>();	            
  }
};