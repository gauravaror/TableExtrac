
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
  mlb->elements = new std::map<int,int>();
  mlb->elements->insert(std::pair<int,int>(int(l.texts->size()),1));
 // std::cout<<"increadsert_block "<<endl;
  mlb->avg_distance = 0;
}

void update_block(TableBlock & mlb, Line  l) {
  mlb.end++;
  mlb.leftmost = std::min(mlb.leftmost,l.leftmost);
  mlb.rightmost = std::max(mlb.rightmost,l.rightmost);
  mlb.max_elements = std::max(mlb.max_elements,int(l.texts->size()));
  std::map<int,int>::iterator it = mlb.elements->find(int(l.texts->size()));
  if(it == mlb.elements->end()) {
  //    std::cout<<"incread update_block "<<endl;
      mlb.elements->insert(std::pair<int,int>(int(l.texts->size()),1));
  } else {
   ///   std::cout<<it->second +1<<"increadind "<<endl;
      int new_v = it->second +1;
      it->second++;
  }

}

int get_max_column(TableBlock & mymap) {
  int colum = 0;
  int value = 0;
    for (std::map<int,int>::iterator it=mymap.elements->begin(); it!=mymap.elements->end(); ++it) {
         // std::cout << it->first << " => " << it->second << '\n';
          if(it->second> colum) {
            colum = it->second;
            value = it->first;
          }
    }
    return value;
}

bool check_mergable_columns(std::vector<TextElement *> * & textElement) {
  //std::cout<<" vvv " <<textElement->size()<<" "<<textElement->at(0)->value<<" "<<textElement->at(0)->value.size()<<endl;
  bool mergable = false;
  if(textElement->size() <= 3 ) {
  for (int i =0;i<(textElement->size()-1);i++) {
    if((textElement->at(i)->value.size() == 2) && ((i+1)<textElement->size()) && (textElement->at(i+1)->value.size() != 2) ) {
      //std::cout<<i+1<<"  exxx "<<textElement->size()<<endl;
      TextElement* tel =     textElement->at(i);
      TextElement* tel1 =     textElement->at(i+1);
      tel->value += " ";
      tel->value += tel1->value;
      tel->right = tel1->right;
      textElement->erase(textElement->begin()+(i+1));
      mergable = true;
      return mergable;
    }
  }
  return mergable;
  }
  /*if ((textElement->size() == 2) && (textElement->at(0)->value.size() == 2)) {
    TextElement* tel =     textElement->at(0);
    TextElement* tel1 =     textElement->at(1);
    tel->value += " ";
    tel->value += tel1->value;
    tel->right = tel1->right;
    textElement->pop_back();
    return true;
  } */
  return mergable;
}

bool line_mergable(Line &curr_l,TableBlock& curr_bl) {
  bool mergable = true;
  Line prev_line = lines->at(curr_bl.end);

  //Check if the colum is mergable
  for(int curr_itr = 0;curr_itr < curr_l.texts->size();curr_itr++) {
    bool element_mergable = false;
    for(int curr_itr_blk = 0;curr_itr_blk < prev_line.texts->size();curr_itr_blk++) {
      TextElement *mergable_element = curr_l.texts->at(curr_itr);
      TextElement *test_element_curr = prev_line.texts->at(curr_itr_blk);
     // std::cout<<test_element_curr->value<<" itr value  "<<curr_itr_blk<<" "<<mergable_element->value<<" "<<mergable_element->left<<" left rights "<<(test_element_curr->left -(test_element_curr->height/2))<<endl;

      if(prev_line.texts->size() -1 != curr_itr_blk) {
        TextElement* test_element_next = prev_line.texts->at(curr_itr_blk+1);  
      //  std::cout<<"  rigg  " <<mergable_element->right << "  "<<test_element_next->right<<"  "<<(test_element_next->left-(test_element_next->height/2))<<endl;
        if( (mergable_element->left > (test_element_curr->left -(test_element_curr->height/2))) && (mergable_element->right < (test_element_next->left-(test_element_next->height/2)))) {
          element_mergable= true;
          //cout<<"merding"<<endl;
        }
      } else {
        if( (mergable_element->left > (test_element_curr->left -(test_element_curr->height/2)))) {
          element_mergable = true;
         // cout<<"merging"<<endl;
        }
      }
    }
   // std::cout<<" mergable_element "<<element_mergable<<endl;
    mergable &= element_mergable;
  }
    //Execute the merge
    if(mergable == true) {
    for(int curr_itr = 0;curr_itr < curr_l.texts->size();curr_itr++) {
    for(int curr_itr_blk = 0;curr_itr_blk < prev_line.texts->size();curr_itr_blk++) {
      TextElement *mergable_element = curr_l.texts->at(curr_itr);
      TextElement *test_element_curr = prev_line.texts->at(curr_itr_blk);
   //   std::cout<<test_element_curr->value<<" itr value  "<<mergable_element->value<<" "<<mergable_element->left<<" left rights "<<(test_element_curr->left -3)<<endl;

      if(prev_line.texts->size() - 1 != curr_itr_blk) {
        TextElement* test_element_next = prev_line.texts->at(curr_itr_blk+1);  
        if( (mergable_element->left > (test_element_curr->left -(test_element_curr->height/2))) && (mergable_element->right < (test_element_next->left-(test_element_next->height/2)))) {
          test_element_curr->value += "  ";
          test_element_curr->value += mergable_element->value;
          test_element_curr->left = std::min(test_element_curr->left,mergable_element->left);
          test_element_curr->right = std::max(test_element_curr->right,mergable_element->right);
    //            std::cout<<test_element_curr->value<<" value  "<<mergable_element->value<<endl;

        }
      } else {
        if( (mergable_element->left > (test_element_curr->left -(test_element_curr->height/2)))) {
          test_element_curr->value += "  ";
          test_element_curr->value += mergable_element->value;
          test_element_curr->left = std::min(test_element_curr->left,mergable_element->left);
          test_element_curr->right = std::max(test_element_curr->right,mergable_element->right);
  //              std::cout<<test_element_curr->value<<" value  "<<mergable_element->value<<endl;

        }
      }
    }
  }
  }

  return mergable;
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
      //std::cout<<endl;
      Line curr = lines->at(i);
      sort(curr.texts->begin(),curr.texts->end(),left_comparator);
      //std::cout<< check_mergable_columns(curr.texts)<<" merged colum "<<curr.texts->at(0)->value<<endl;
      check_mergable_columns(curr.texts);
      if (curr.texts->size() > 1) {
        if (multi_modus == true) {
          TableBlock curr_block = tblock->back();
          if ( curr.leftmost < (curr_block.leftmost - curr.height)) {
            multi_modus = false;
         }
        }
        if (multi_modus == true) {
          TableBlock curr_block = tblock->back();
          Line prev_line = lines->at(curr_block.end);
          if( prev_line.texts->size() > curr.texts->size() && ((curr.top - prev_line.bottom )< (2*prev_line.height))) {
            bool merged = line_mergable(curr,curr_block);
            cout<<merged<<" merged  "<<endl;
            if(merged) {
              lines->erase(lines->begin()+i);
              i--;
              continue;
            }

          }
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
          if((curr.leftmost >= (curr_block.leftmost-(curr.height/2)) )) {
            bool merged_ = line_mergable(curr,curr_block);
            //cout<<merged_<<" merged_  "<<endl;
            if(merged_) {
              lines->erase(lines->begin()+i);
              i--;
              continue;
            } else {
              multi_modus = false;
              continue;
            }

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
     /* for(std::vector<TextElement *>::iterator itt = curr.texts->begin();itt!=curr.texts->end();++itt) {
        std::cout<<(*itt)->value <<"("<<(*itt)->left<<")      ";
      }  */  
    }
    std::cout <<tblock->size()<<" block size ";
    for(unsigned int i = 0; i < tblock->size();i++) {

            cout<<endl<<"Block "<<i+1<<"  "<<tblock->at(i).begin<< "   "<<tblock->at(i).end<< "   "<<tblock->at(i).leftmost<< "   "<<tblock->at(i).rightmost<<"  "<< tblock->at(i).max_elements<<"  max "<<get_max_column(tblock->at(i))<<endl;

          for(unsigned int l = tblock->at(i).begin; l <= tblock->at(i).end;l++) {
            std::cout<<endl;
            Line curr = lines->at(l);
            for(std::vector<TextElement *>::iterator itt = curr.texts->begin();itt!=curr.texts->end();++itt) {
              std::cout<<(*itt)->value<<"  "<<(*itt)->left <<"("<<(*itt)->right<<")      ";
            }     
        }

       }
    
    
    /*for(std::vector<Line>::iterator itl = lines->begin(); itl != lines->end(); ++itl) {
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