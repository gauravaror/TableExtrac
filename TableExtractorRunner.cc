#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "myparser.h"
#include "mypdfparser.h"
#include "Line.h"
#include <string>
#include "TableBlock.h"
#include "Column.h"
#include "TableExtractor.h"

using namespace std;

string correct_non_utf_8(string *str)
{
    int i,f_size=str->size();
    unsigned char c,c2,c3,c4;
    string to;
    to.reserve(f_size);

    for(i=0 ; i<f_size ; i++){
        c=(unsigned char)(*str)[i];
		//std::cout<<c;
        if(c<32){//control char
            if(c==9 || c==10 || c==13){//allow only \t \n \r
                to.append(1,c);
				//std::cout<<endl<<"  work "<<c<<"  10 "<<endl;
            }
            continue;
        }else if(c<127){//normal ASCII
            to.append(1,c);
            continue;
        }else if(c<160){//control char (nothing should be defined here either ASCI, ISO_8859-1 or UTF8, so skipping)
            if(c2==128){//fix microsoft mess, add euro
                to.append(1,226);
                to.append(1,130);
                to.append(1,172);
				//std::cout<<"  work "<<c<<"  8 "<<endl;
            }
            if(c2==133){//fix IBM mess, add NEL = \n\r
                to.append(1,10);
                to.append(1,13);
				//std::cout<<"  work "<<c<<"  7 "<<endl;
            }
            continue;
        }else if(c<192){//invalid for UTF8, converting ASCII
            to.append(1,(unsigned char)194);
            to.append(1,c);
				//std::cout<<endl<<"  work  "<<c<<"  6 "<<endl;
            continue;
        }else if(c<194){//invalid for UTF8, converting ASCII
            to.append(1,(unsigned char)195);
            to.append(1,c-64);
				//std::cout<<endl<<"  work  "<<c<<"  5 "<<endl;
            continue;
        }else if(c<224 && i+1<f_size){//possibly 2byte UTF8
            c2=(unsigned char)(*str)[i+1];
            if(c2>127 && c2<192){//valid 2byte UTF8
                if(c==194 && c2<160){//control char, skipping
                    ;
                }else{
                    to.append(1,c);
                    to.append(1,c2);
				//std::cout<<endl<<"  work  "<<c<<"  4 "<<endl;
                }
                i++;
                continue;
            }
        }else if(c<240 && i+2<f_size){//possibly 3byte UTF8
            c2=(unsigned char)(*str)[i+1];
            c3=(unsigned char)(*str)[i+2];
            if(c2>127 && c2<192 && c3>127 && c3<192){//valid 3byte UTF8
                //to.append(1,c);
                //to.append(1,c2);
                //to.append(1,c3);
                i+=2;
                continue;
            }
        }else if(c<245 && i+3<f_size){//possibly 4byte UTF8
            c2=(unsigned char)(*str)[i+1];
            c3=(unsigned char)(*str)[i+2];
            c4=(unsigned char)(*str)[i+3];
            if(c2>127 && c2<192 && c3>127 && c3<192 && c4>127 && c4<192){//valid 4byte UTF8
                //to.append(1,c);
                //to.append(1,c2);
                //to.append(1,c3);
                //to.append(1,c4);
				//std::cout<<endl<<"  work  "<<c<<"  2 "<<endl;
                i+=3;
                continue;
            }
        }
    }
    return to;
}

int main(int argc, char** argv) {

	string filename = argv[1];
	cout<<"<br><br><b>"<<filename<<"</b><br><br>";
	TableExtractor * tb = new TableExtractor(filename);
	std::ifstream file(filename);
 	std::string temp;
 	std::string textFile = "";
 	while(std::getline(file, temp)) {
 		textFile += temp;
	}
	std::string tempII = correct_non_utf_8(&textFile);
	//std::cout<<endl<<"  sdfdsklflkdfs "<<tempII<<endl;
	std::istringstream iss(tempII);


	std::vector<std::vector<TextElement>> *va =  new std::vector<std::vector<TextElement> >();

    //Invoking parser to get text elements from hocr file.
    MyPdfParser * parser = new MyPdfParser(va);
    parser->set_substitute_entities(true); //
    parser->parse_stream(iss);
    //cout<<"dsfsd "<<va->size()<<endl;
    int index = va->size()-1;
      for (unsigned i=0; i<va->size(); i++) {
           // cout<<va->at(i).size()<<endl;
            std::vector<TextElement> work = va->at(i);
            for (unsigned j=0; j<va->at(i).size(); j++) {
                cout<<endl<<"<br>  news "<<work.at(j).value<<"  "<<work.at(j).left<<" "<<work.at(j).top<<endl;
            }
            tb = new TableExtractor(filename);
            cout<<tb->run_extractor(filename,&(work))<<endl;
        }

   // cout<<"dsfsd2222 "<<va->at(index)<<endl;
	//cout<<tb->run_extractor(filename,&(va->front()))<<endl;

}
