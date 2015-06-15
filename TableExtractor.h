 class TableExtractor {

  public:
  std::vector<TextElement>* v;
  std::vector<Line> *lines;
  std::vector<TableBlock> *tblock;
  std::string run_extractor(std::string fname, std::vector<TextElement> * textEle);
  std::string FindTables();
  bool line_mergable(Line&, TableBlock&);
  void append_lines(string & output,int begin,int end);

   TableExtractor(std::vector<TextElement> * textEle) {
    v = textEle;
  }
  
   TableExtractor(string fname) {
    v =  new std::vector<TextElement>();
    lines =  new std::vector<Line>();
    tblock =  new std::vector<TableBlock>;

    //Invoking parser to get text elements from hocr file.
    MySaxParser * parser = new MySaxParser(v);
    parser->set_substitute_entities(true); //
    parser->parse_file(fname);

  }

};