 class Column {

  public:
  std::vector<TextElement> * cells;
  int left;
  int right;
  int empty_cells;
  int header;
    
   Column(int l, int r) {
    cells = new std::vector<TextElement>();	 
    left = l;
    right = r;
    empty_cells = 0;
    header = -1;
  }
  
   Column() {
  	cells = new std::vector<TextElement>();
  	left = -1;
  	right = -1;
  	empty_cells = 0;
  	header = -1;
  }

};