class TableTree {

public:
int left;
int right;
int level;
string content;
std::vector<TableTree> * nodes;
TextElement text_element;

   TableTree(string c, int l) {
    // used only for the construction of the root element in second_classification.java
    content = c;
	  level = l;
	  nodes = new std::vector<TableTree>();
  }
  
   TableTree(TextElement t , int l) {
    text_element = t;
    content = t.value;
	  left = t.left;
	  right = t.right;
	  level = l;
	  nodes = new std::vector<TableTree>();
  }
  
   TableTree(string c, TextElement t , int l) {
    content = c;
	  left = t.left;
	  right = t.right;
	  level = l;
	  nodes = new std::vector<TableTree>();
  }
  
};
