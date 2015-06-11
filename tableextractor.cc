#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include "myparser.h"
#include "Line.h"
#include "TableBlock.h"
std::vector<TextElement>* v;
std::vector<Line> *lines;
std::vector<TableBlock> *tblock;

bool top_compare(const TextElement lhs, const TextElement rhs) {
  return lhs.top < rhs.top;
}

bool left_comparator(const TextElement * lhs,const TextElement * rhs){
  return  lhs->left < rhs->left;
}

bool same_line(Line  *& lines_,TextElement * element_) {
  if( (lines_->top - (lines_->height/2)) < element_->top && (lines_->top + (lines_->height/2)) > element_->top) {
    return true;
  } else {
    return false;
  }
}

void add_element_line(Line * & line,TextElement * el) {
 // cout<<"  add_element_line_before "<<line->leftmost<<" "<<el->left<<endl;

  line->top = std::min(line->top,el->top);
  line->bottom =  std::max(line->bottom,el->top + el->height);
  line->height = line->bottom-line->top;
  line->leftmost = std::min(line->leftmost,el->left);
  line->rightmost = std::max(line->rightmost,el->right);
  line->last_top = std::max(line->last_top,el->top);
  line->first_top = std::min(line->first_top,el->top);
  line->used_space++;
  line->typ  = "text";
  line->texts->push_back(el);
 // cout<<"  add_element_line "<<line->leftmost<<endl;

}

void populate_line_(Line * & line,TextElement * el) {
    //cout<<"  populate_line_before "<<line->leftmost<<" "<<el->left<<endl;

  line->top = el->top;
  line->bottom =  el->top + el->height;
  line->height = el->height;
  line->leftmost = el->left;
  line->rightmost = el->right;
  line->last_top = el->top;
  line->first_top = el->top;
  line->used_space=1;
  line->typ  = "text";
  line->texts->push_back(el);
  //cout<<"  populate_line_ "<<line->leftmost<<endl;

}

void set_block_values(TableBlock *& mlb, Line  l, int lineno) {
  mlb->begin = lineno;
  mlb->end = lineno;
  mlb->leftmost = l.leftmost;
  mlb->rightmost = l.rightmost;
  mlb->max_elements = int(l.texts->size());
  mlb->avg_distance = 0;
}

void update_block(TableBlock & mlb, Line  l) {
  mlb.end++;
  mlb.leftmost = std::min(mlb.leftmost,l.leftmost);
  mlb.rightmost = std::max(mlb.rightmost,l.rightmost);
  mlb.max_elements = std::max(mlb.max_elements,int(l.texts->size()));
}

int
main(int argc, char* argv[])
{
  std::string filepath;
  if(argc > 1 )
    filepath = argv[1]; //Allow the user to specify a different XML file to parse.
  else
    filepath = "example.xml";
    
  // Parse the entire document in one go:
  try
  {
    // Elements to store Texts Lines and Table Blocks.
    v =  new std::vector<TextElement>();
    lines =  new std::vector<Line>();
    tblock =  new std::vector<TableBlock>;

    //Invoking parser to get text elements from hocr file.
    MySaxParser * parser = new MySaxParser(v);
    parser->set_substitute_entities(true); //
    parser->parse_file(filepath);

    //Sorting the Text Element in order of top value
    sort(v->begin(),v->end(),top_compare);
    std::vector<TextElement>::iterator it = v->begin();


    //Populating Lines with elements which belong to particular line
    Line * curr_line = new Line();
    populate_line_(curr_line,&(*it));
    lines->push_back(*curr_line);
    ++it;
   // std::cout <<curr_line->top<<" curr_line ";
    for(; it != v->end(); ++it) {
      if (same_line(curr_line,&(*it)))  {
        add_element_line(curr_line,&(*it));
       // cout<<" leftmost k"<<curr_line->leftmost<<endl;
        lines->pop_back();
        lines->push_back(*curr_line);
      } else {
        curr_line = new Line();
        populate_line_(curr_line,&(*it));
        lines->push_back(*curr_line);
      }
    }   

    // Find the block which can be probable tables.
    bool multi_modus = false;        
    //std::cout <<lines->size()<<" lines size ";
    for(unsigned int i = 0; i < lines->size();i++) {
     // std::cout<<endl;
      Line curr = lines->at(i);
      sort(curr.texts->begin(),curr.texts->end(),left_comparator);
      if (curr.texts->size() > 1) {
        if (multi_modus == true) {
          TableBlock curr_block = tblock->back();
      //    cout<<"update_block  "<<curr.leftmost<<endl;
          update_block(curr_block,curr);
          //cout<<"update_block  end "<<curr_block.end<<endl;
          tblock->pop_back();
          tblock->push_back(curr_block);

        } else {
          TableBlock * block =  new TableBlock();
       //   cout<<"update_block  "<<curr.leftmost<<endl;
          set_block_values(block,curr,i);
          tblock->push_back(*block);
          multi_modus = true;
        }
      } else if (curr.texts->size() == 1){
        if (multi_modus == true) {
          TableBlock curr_block = tblock->back();
          if((curr.leftmost >= (curr_block.leftmost-3) ) ) {
            //cout<<"update_block  "<<curr_block.end<<endl;
            update_block(curr_block,curr);
            //cout<<"update_block  end "<<curr_block.end<<endl;
            tblock->pop_back();
            tblock->push_back(curr_block);

          } else {
            multi_modus = false;
          }

        }

      }
      /*for(std::vector<TextElement *>::iterator itt = curr.texts->begin();itt!=curr.texts->end();++itt) {
        std::cout<<(*itt)->value <<"("<<(*itt)->left<<")      ";
      }    */
    }
    std::cout <<tblock->size()<<" block size ";
    for(unsigned int i = 0; i < tblock->size();i++) {

            cout<<endl<<"Block "<<i+1<<"  "<<tblock->at(i).begin<< "   "<<tblock->at(i).end<< "   "<<tblock->at(i).leftmost<< "   "<<tblock->at(i).rightmost<<"  "<< tblock->at(i).max_elements<<endl;

          for(unsigned int l = tblock->at(i).begin; l <= tblock->at(i).end;l++) {
            std::cout<<endl;
            Line curr = lines->at(l);
            for(std::vector<TextElement *>::iterator itt = curr.texts->begin();itt!=curr.texts->end();++itt) {
              std::cout<<(*itt)->left <<"("<<(*itt)->right<<")      ";
            }     
        }

       }
    
    /*
    for(std::vector<Line>::iterator itl = lines->begin(); itl != lines->end(); ++itl) {
      std::cout<<endl;
      sort(itl->texts->begin(),itl->texts->end(),left_comparator);
      for(std::vector<TextElement *>::iterator itt = itl->texts->begin();itt!=itl->texts->end();++itt) {
        std::cout<<(*itt)->value <<"("<<(*itt)->left<<")      ";
      }
    }*/

  }
  catch(const xmlpp::exception& ex)
  {
    std::cout << "libxml++ exception: " << ex.what() << std::endl;
  }

 


  return 0;
}
