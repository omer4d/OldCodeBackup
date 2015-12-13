/* 
 * File:   Text.hpp
 * Author: Stas
 *
 * Created on March 12, 2013, 9:33 PM
 */

#ifndef TEXT_HPP
#define	TEXT_HPP

#include <cstring>

#include "List.hpp"

struct Word {
    char* str;
    int len, tabs, spaces;
    
    Word()
    {
        this->str = NULL;
        this->len = 0;
        this->tabs = 0;
        this->spaces = 0;
    }
    
    Word(char const* start, char const* end)
    {
        len = end - start;
        str = new char[len + 1];
        strncpy(str, start, len);
        str[len] = 0;
        
        tabs = 0;
        spaces = 0;
    }
    
    Word(Word const& other)
    {
        len = other.len;
        tabs = other.tabs;
        spaces = other.spaces;
        str = new char[len + 1];
        
        strncpy(str, other.str, other.len + 1);
    }
    
    ~Word()
    {
        delete[] str;
    }
    
    Word* split(int i)
    {
        Word* second = new Word(&str[i], &str[len]);
        char* tmp = NULL;
        
        second->spaces = spaces;
        second->tabs = tabs;
        
        len = i;
        spaces = 0;
        tabs = 0;
        
        tmp = new char[len + 1];
        strncpy(tmp, str, i);
        tmp[len] = 0;
        delete[] str;
        str = tmp;
        
        return second;
    }
};

class Text {
    typedef List<Word*> WordList;
    typedef WordList::Node WordNode;
    
    WordList wordList;
    
public:

    class Iterator {
        friend class Text;
        
        WordNode* node;

        Iterator(WordNode* node)
        {
            this->node = node;
        }
        
    public:
        Iterator()
        {
            this->node = NULL;
        }
        
        Iterator& operator++()
        {
            node = node->getNext();
            return *this;
        }
        
        Iterator operator++(int)
        {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(Iterator const& other) const
        {
            return node == other.node;
        }
        
        bool operator!=(Iterator const& other) const
        {
            return !(*this == other);
        }
        
        int getWordSpaces() const
        {
            return node->data->spaces;
        }

        int getWordTabs() const
        {
            return node->data->tabs;
        }

        int getWordLen() const
        {
            return node->data->len;
        }

        char const* getWordStr() const
        {
            return node->data->str;
        }
    };
    
private:
    
    void init(char const* str)
    {
        char const* start = str;
        
        while(*str)
        {
            if(*str == 32 || *str == 9)
            {
                Word* word = new Word(start, str);
                
                while(*str == 9 || *str == 32)
                {
                    switch(*str)
                    {
                        case 9:
                            ++word->tabs;
                            break;
                        
                        case 32:
                            ++word->spaces;
                            break;
                    }
                    
                    ++str;
                }
                
                wordList.pushBack(word);
                start = str;
            }
            
            else
                ++str;
        }
        
        if(*start != 0)
            wordList.pushBack(new Word(start, str));
    }
    
//    void merge(WordNode* first, WordNode* last, int len)
//    {
//        if(last == NULL)
//            last = wordList.getLast();
//        
//        if(first == last)
//            return;
//        
//        Word* lastWord = last->data;
//        Word* merged = new Word();
//        char* tmp = new char[len + 1];
//        char* pos = tmp;
//
//        merged->len = len;
//        merged->spaces = lastWord->spaces;
//        merged->tabs = lastWord->tabs;
//        merged->str = tmp;
//        
//        wordList.insertAfter(last, merged);
//        
//        for(WordNode* node = first; node != last->getNext(); )
//        {
//            Word* word = node->data;
//            
//            strncpy(pos, word->str, word->len);
//            pos += word->len;
//            
//            delete word;
//            node = wordList.erase(node);
//        }
//        
//        tmp[len] = 0;
//    }
    
public:
    Text(char const* str)
    {
        init(str);
    }
    
    Text(Text const& other)
    {
        for(WordNode const* node = other.wordList.getFirst(); node != NULL; node = node->getNext())
            wordList.pushBack(new Word(*node->data));
    }
    
    ~Text()
    {
        for(WordNode* node = wordList.getFirst(); node != NULL; node = node->getNext())
            delete node->data;
    }
    
//    void merge()
//    {
//        WordNode* node = wordList.getFirst();
//        
//        while(node)
//        {   
//            if(!node->data->spaces && !node->data->tabs)
//            {
//                WordNode* first = node;
//                int len = 0;
//                
//                while(node && !node->data->spaces && !node->data->tabs)
//                {
//                    len += node->data->len;
//                    node = node->getNext();
//                }
//                
//                if(node)
//                    len += node->data->len;
//                
//                merge(first, node, len);
//            }
//            
//            else
//                node = node->getNext();
//        }
//    }
    
    Iterator begin()
    {
        return Iterator(wordList.getFirst());
    }
    
    Iterator end()
    {
        return Iterator(NULL);
    }
    
    void splitWord(Iterator iter, int i)
    {
        Word* word = iter.node->data;

        if(i < word->len)
        {
            Word* second = word->split(i);
            wordList.insertAfter(iter.node, second);
        }
    }
    
    void print() const
    {
        for(WordNode const* node = wordList.getFirst(); node != NULL; node = node->getNext())
        {
            Word* word = node->data;
            if(word->len)
                printf("%s ", word->str);
        }
    }
};

#endif	/* TEXT_HPP */

