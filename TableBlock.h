
class TableBlock {
	
  public:
  int begin;
  int end;
  int leftmost;
  int rightmost;
  int max_elements;
  int avg_distance;
  int page;
  int used_space;


   TableBlock() {
  
  }
  
   TableBlock(int b, int e, int l,int r,int me,int ad,int p) {
  	begin = b;
  	end = e;
  	leftmost = l;
  	rightmost = r;
  	max_elements = me;
  	avg_distance = ad;
  }	
};