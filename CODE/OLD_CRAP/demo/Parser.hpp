/* 
 * File:   Parser.hpp
 * Author: Stas
 *
 * Created on February 3, 2012, 1:41 AM
 */

#ifndef PARSER_HPP
#define	PARSER_HPP

#include <string>
#include <vector>
#include <list>

typedef std::vector<std::string> StringVec;

class ParsedWord {
    std::string word;
    char type;
    
public:
    ParsedWord()
    {
        type = 0;
    }
    
    void setWord(std::string const& word)
    {
        this->word = word;
    }
    
    void setType(char t)
    {
        type = t;
    }
    
    void clearType()
    {
        type = 0;
    }
    
    std::string const& getWord() const
    {
        return word;
    }
    
    char getType() const
    {
        return type;
    }
};

class ParsedCommand {
    std::vector<ParsedWord> parsedWordVec;
    
public:
    int length() const
    {
        return parsedWordVec.size();
    }
    
    void clear()
    {
        parsedWordVec.clear();
    }
    
    void pushWord(ParsedWord const& word)
    {
        parsedWordVec.push_back(word);
    }
    
    std::string const& word(int i) const
    {
        return parsedWordVec.at(i).getWord();
    }
    
    char type(int i) const
    {
        return parsedWordVec.at(i).getType();
    }
    
    std::string getPattern()
    {
        std::string pattern;
        
        pattern.reserve(parsedWordVec.size());
        for(int i = 0; i < parsedWordVec.size(); i++)
            pattern.push_back(parsedWordVec[i].getType());
        
        return pattern;
    }
    
    bool matchesPattern(std::string const& pattern) const
    {
        if(pattern.length() != parsedWordVec.size())
            return false;
        for(int i = 0; i < pattern.length(); i++)
            if(parsedWordVec[i].getType() != pattern.at(i))
                return false;
        return true;
    }
};

class Parser {
    StringVec nameVec, descVec, actionVec, keywordVec;
    
public:
    static void stringToVec(std::string const& str, StringVec& vec)
    {
        char* cstr = new char [str.size() + 1], *p;

        strcpy(cstr, str.c_str());

        for(int i = 0; i < str.size(); i++)
            cstr[i] = tolower(cstr[i]);
        p = strtok(cstr, " ,.;");

        while(p != NULL)
        {
            vec.push_back(std::string(p));
            p = strtok(NULL, " ");
        }

        delete[] cstr;
    }

    static bool vecContainsString(StringVec const& vec, std::string const& str)
    {
        for(int i = 0; i < vec.size(); i++)
            if(str.compare(vec[i]) == 0)
                return true;
        return false;
    }
    
    void pushName(std::string const& name) { nameVec.push_back(name); }
    void pushDesc(std::string const& desc) { descVec.push_back(desc); }
    void pushAction(std::string const& action) { actionVec.push_back(action); }
    void pushKeyword(std::string const& keyword) { keywordVec.push_back(keyword); }
    
    void clear()
    {
        nameVec.clear();
        descVec.clear();
        actionVec.clear();
        keywordVec.clear();
    }
    
    void parse(ParsedCommand& parsedCommand, std::string const& command)
    {
        ParsedWord temp;
        StringVec* wordVec = new StringVec();
        stringToVec(command, *wordVec);
        
        for(int i = 0; i < wordVec->size(); i++)
        {
            if(Parser::vecContainsString(nameVec, (*wordVec)[i])) temp.setType('n');
            if(Parser::vecContainsString(descVec, (*wordVec)[i])) temp.setType('d');
            if(Parser::vecContainsString(actionVec, (*wordVec)[i])) temp.setType('a');
            if(Parser::vecContainsString(keywordVec, (*wordVec)[i])) temp.setType('k');
            
            if(temp.getType())
            {
                temp.setWord((*wordVec)[i]);
                parsedCommand.pushWord(temp);
                temp.clearType();
            }
        }
        
        delete wordVec;
    }
    
    void parse(std::list<ParsedCommand>& parsedList, std::string const& command)
    {
        ParsedWord word;
        ParsedCommand parsedCommand;
        char lastClass = 'a';
        
        StringVec* wordVec = new StringVec();
        stringToVec(command, *wordVec);
        
        for(int i = 0; i < wordVec->size(); i++)
        {
            if(Parser::vecContainsString(nameVec, (*wordVec)[i]))
            {
                lastClass = 'n';
                word.setType('n');
            }
            
            else if(Parser::vecContainsString(descVec, (*wordVec)[i])) 
            {
                lastClass = 'd';
                word.setType('d');
            }
            
            else if(Parser::vecContainsString(keywordVec, (*wordVec)[i]))
            {
                lastClass = 'k';
                word.setType('k');
            }
            
            else if(Parser::vecContainsString(actionVec, (*wordVec)[i]))
            {
                if(lastClass != 'a')
                {
                    parsedList.push_back(parsedCommand);
                    parsedCommand.clear();
                }
                    
                lastClass = 'a';
                word.setType('a');
            }

            if(word.getType())
            {
                word.setWord((*wordVec)[i]);
                parsedCommand.pushWord(word);
                word.clearType();
            }
        }

        parsedList.push_back(parsedCommand);
        delete wordVec;
    }
};

#endif	/* PARSER_HPP */

