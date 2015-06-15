#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "myparser.h"
#include "Line.h"
#include <string>
#include "TableBlock.h"
#include "Column.h"
#include "TableExtractor.h"

using namespace std;
int main(int argc, char** argv) {

	string filename = argv[1];
	cout<<filename<<endl;
	TableExtractor * tb = new TableExtractor(filename);
	std::vector<TextElement> *va =  new std::vector<TextElement>();

    //Invoking parser to get text elements from hocr file.
    MySaxParser * parser = new MySaxParser(va);
    parser->set_substitute_entities(true); //
    parser->parse_file(filename);
    //cout<<"dsfsd "<<va->size();
	cout<<tb->run_extractor(filename,va)<<endl;

}