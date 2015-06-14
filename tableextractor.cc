
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
#include "TableTree.h"
#include "Column.h"


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





bool in_boundaries(int l1, int r1, int l2, int r2) {
  if ((l1 >= l2 && r1 <= r2) || 
    (l1 >= l2 && l1 <= r2 && r1 > r2) ||
    (l1 < l2 && r1 >= l2 && r1 <= r2) ||
    (l2 >= l1 && r2 <= r1))  {
      return true;  
  }
  return false;
}


bool insert_into_tree(TextElement t,TableTree * n,int l){
   
      if (n->content.compare("null") == 0) {
    
      if (insert_into_tree(t, &(n->nodes->at(0)),l))  {
        return true;
      }
    }
    else {
      if (in_boundaries(t.left, t.right, n->left, n->right) || (n->content.compare("root") == 0)) {
      int pos = 0;
  
      for (int i=0;i<n->nodes->size();i++) {
      
        TableTree next = (TableTree) n->nodes->at(i);
        // it was t.left > next.right. which means completely on the right side
        if (t.left > next.left) { pos++; }
          
        if ((in_boundaries(t.left, t.right, next.left, next.right) && next.level < l) || (next.content.compare("null") == 0)) {
          if (insert_into_tree(t,&next,l)) {

          return true;
          }
        }
      } // end of for
      

      for (int j=n->level; j < l-1; j++) {
        TableTree * dummy = new TableTree("null", t, j+1);
        //n.nodes.insertElementAt(dummy,pos);
        n->nodes->insert(n->nodes->begin()+pos, *dummy);
        n = dummy;
        pos = 0;
      } 
      TableTree * current = new TableTree(t,l);
    //  cout<<"insert_into_tree  "<<t.value<<"   "<<n->content<<endl;
      n->nodes->insert(n->nodes->begin()+pos, *current);
      return true;
      }
    
    }
    
    return false;

}

  void print_tree(TableTree * n) {
  std::cout<<" "<<n->content<<"  "<<n->level<<"  "<<n->nodes->size()<<endl;
  for (int i=0;i<n->nodes->size();i++) {
    //std::cout<<" "<<n->content<<"  "<<n->level<<"  "<<n->nodes->size()<<endl;
     print_tree( &(n->nodes->at(i)));
     
    }
    //cout<<endl;
  }

  void actualize_column_values(Column * c, TextElement * t) {
     if (c->left ==  -1) {
       c->left = t->left;
    }
    else {
     std::min(c->left,t->left);
     } 
   
     c->right = std::max(c->right,t->left + t->width);
   }
   
    void actualize_column_values_with_another_column(Column * c1, Column * c2) {
     c1->left = std::min(c1->left,c2->left);
     c1->right = std::max(c1->right,c2->right);
   }



   int convert_to_table(TableTree  * n, Column * c, std::vector<Column *> * v, int l) {

     if (c == NULL) {
   // root node
      int spanning =0;
      for (int i=0; i < n->nodes->size(); i++) {
      Column *new_column = new Column();
      v->push_back(new_column);
      spanning += convert_to_table(&(n->nodes->at(i)), new_column, v, l);
    }
    return spanning;
   }
   else {
   // not root node
      int pos = 0;
      if (n->content.compare("null") != 0) {
        c->cells->push_back(n->text_element);   
      pos = c->cells->size();
      if (n->text_element.colspan == 1) {
          actualize_column_values(c, &(n->text_element));
      }
    }
    else {
      TextElement *t = new TextElement();
      c->cells->push_back(*t);
      pos = c->cells->size();
    }

    if (n->nodes->size() >= 1) {
      int spanning = 0;
      Column * store_column = new Column();
      store_column->cells->insert(store_column->cells->end(),c->cells->begin(),c->cells->end());
      for(int itr =0;itr<store_column->cells->size();itr++) {
        store_column->cells->at(itr).value = "null";
      }
      spanning += convert_to_table( &(n->nodes->at(0)), c, v, l);

      for (int i=1; i < n->nodes->size(); i++) {
        Column * new_column = new Column();
        //std::cout<<spanning<<" ds dsss <br>"<<endl;
      new_column->cells->insert(new_column->cells->end(),store_column->cells->begin(),store_column->cells->begin()+store_column->cells->size());
      v->push_back(new_column);
      spanning += convert_to_table( &(n->nodes->at(i)), new_column, v, l);
      }
      
      TextElement t =  c->cells->at(pos-1);
      t.colspan = spanning;
      return spanning;
    }
    else {
    // no children means that we are at the leaf of a branch
        while (c->cells->size() < l) {
        TextElement* t = new TextElement();
        c->cells->push_back(*t);
      }
      return 1;
    }
   }
    
   }

void FindTables() {
  for ( int tb = 0; tb < tblock->size(); tb++) {
    TableBlock curr_table_block =  tblock->at(tb);
    int lines_before = 0;
    int line_count = 0;
    std::cout<<"<b>Table Block "<<tb<<"</b><br>"<<endl;
    if (curr_table_block.end - curr_table_block.begin >= 2) {
     // if (curr_table_block.elements->size() == 1) {
        int b = curr_table_block.begin;
        TableTree * root = new TableTree("root",-1);
        while (b <= curr_table_block.end) {
             Line l = (Line) lines->at(b);
       
           for (int j=0;j<l.texts->size();j++) {
            TextElement * t =  l.texts->at(j);
             // std::cout<<t->value<<"  cddd "<<endl;
              insert_into_tree(*t,root,lines_before);
           }  

          
             b++;            
             lines_before++;
        } 
        //print_tree(root);
        std::vector<Column *> * v = new std::vector<Column *>();
        convert_to_table(root, NULL, v, lines_before);
        std::cout<<"<table border=2>"<<endl;
        for (int li = 0;li<lines_before;li++) {
          if(li!=0) {
            std::cout<<"</tr>"<<endl;
          } 
          std::cout<<"<tr>"<<endl;
         for(int k =0;k<v->size();k++) {
            std::cout<<"<td>";
            if((v->at(k)->cells->at(li).value) != "null")
              std::cout<<(v->at(k)->cells->at(li).value);
            std::cout<<"</td>";
          }
          
        }
        std::cout<<"</table>"<<endl;
    //  }
    }
  }
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
          test_element_curr->value += "  <br> ";
          test_element_curr->value += mergable_element->value;
          test_element_curr->left = std::min(test_element_curr->left,mergable_element->left);
          test_element_curr->right = std::max(test_element_curr->right,mergable_element->right);
    //            std::cout<<test_element_curr->value<<" value  "<<mergable_element->value<<endl;

        }
      } else {
        if( (mergable_element->left > (test_element_curr->left -(test_element_curr->height/2)))) {
          test_element_curr->value += "  <br> ";
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
           // cout<<merged<<" merged  "<<endl;
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
    //std::cout <<tblock->size()<<" block size ";
    for(unsigned int i = 0; i < tblock->size();i++) {

          //  cout<<endl<<"Block "<<i+1<<"  "<<tblock->at(i).begin<< "   "<<tblock->at(i).end<< "   "<<tblock->at(i).leftmost<< "   "<<tblock->at(i).rightmost<<"  "<< tblock->at(i).max_elements<<"  max "<<get_max_column(tblock->at(i))<<endl;

          for(unsigned int l = tblock->at(i).begin; l <= tblock->at(i).end;l++) {
            std::cout<<endl;
            Line curr = lines->at(l);
            for(std::vector<TextElement *>::iterator itt = curr.texts->begin();itt!=curr.texts->end();++itt) {
            //  std::cout<<(*itt)->value<<"  "<<(*itt)->left <<"("<<(*itt)->right<<")      ";
            }     
        }

       }
    
    FindTables();
    
  }
  catch(const xmlpp::exception& ex)
  {
    std::cout << "libxml++ exception: " << ex.what() << std::endl;
  }

 


  return 0;
}