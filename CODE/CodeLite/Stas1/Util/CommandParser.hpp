/* 
 * File:   CommandParser.hpp
 * Author: Stas
 *
 * Created on October 6, 2013, 12:55 PM
 */

#ifndef COMMANDPARSER_HPP
#define	COMMANDPARSER_HPP

#include <functional>
#include <string>
#include <map>
#include <cstring>
#include <vector>
#include <sstream>

namespace command_parser_details {

template<int ...>
struct Seq { };

template<int N, int ...S>
struct SeqGen : SeqGen<N - 1, N - 1, S...> { };

template<int ...S>
struct SeqGen<0, S...> {
	typedef Seq<S...> seq;
};

class CallData {
	friend class CommandParser;
	std::vector<std::string> mStrVec;
	
public:
	std::string const& getName() const
	{
		return mStrVec[0];
	}
	
	std::string const& getArg(int n) const
	{
		return mStrVec.at(1 + n);
	}
	
	int getArgNum() const
	{
		return int(mStrVec.size()) - 1;
	}
	
	void print() const
	{
		for(int i = 0; i < getArgNum(); ++i)
			printf("[%s]", mStrVec[1 + i].c_str());
	}
};

class CommandBase {
	
};

struct LexcastError {};

template <typename T>
T lexcast(std::string const& str)
{
	std::stringstream converter;
	T out;
	
	converter << str;
	converter >> out;
	
	if(converter.fail())
		throw LexcastError();
	
	return out;
}

template<typename ...Params, typename F, int ...S>
void call(CallData const& cmd, F func, Seq<S...>)
{
	func(lexcast<Params>(cmd.getArg(S))...);
}

template<typename ...Params, typename F>
void call(CallData const& cmd, F func)
{
	call<Params...>(cmd, func, typename SeqGen<sizeof...(Params)>::seq());
}

class CommandParser {
public:
    typedef std::function<void(CallData&) > Action;
	
	enum Error {
		ERR_BAD_ARGS, ERR_UNREC_CMD
	};

private:
    std::map<std::string, Action> mActionMap;
	std::function<void(Error, std::string const&)> mOnError;

    static void stringToVec(std::vector<std::string>& vec, std::string const& str)
    {
        char* cstr = new char [str.size() + 1], *p;

        strcpy(cstr, str.c_str());
        p = strtok(cstr, " ,");

        while(p != NULL)
        {
            vec.push_back(std::string(p));
            p = strtok(NULL, " ,");
        }

        delete[] cstr;
    }

	void reportError(Error error, std::string const& cmdName) const
	{
		if(mOnError)
			mOnError(error, cmdName);
	}

	void exec(CallData& cmd) const
    {
		// A return value of false means to stop parsing!
		
        if(cmd.mStrVec.size() < 1)
            return;
			
        auto i = mActionMap.find(cmd.getName());

        if(i == mActionMap.end())
			reportError(ERR_UNREC_CMD, cmd.getName());
        else
			i->second(cmd);
    }

public:
	void onError(std::function<void(Error, std::string const&)> const& f)
	{
		mOnError = f;
	}
	
	template <typename ...Params, typename F>
	void reg(std::string const& cmdName, F func, 
		     std::vector<std::string> const& defs={})
	{
		mActionMap[cmdName] = [=](CallData& cmd)
		{
			if(cmd.getArgNum() < sizeof...(Params)) // Not enough strings supplied?
			{
				// Can we account for the rest using the default values?
				if(cmd.getArgNum() + defs.size() >= sizeof...(Params))
				{
					int missing = sizeof...(Params) - cmd.getArgNum();
				
					for(int i = defs.size() - missing; i < defs.size(); ++i)
						cmd.mStrVec.push_back(defs[i]);
				}
				
				else
				{
					reportError(ERR_BAD_ARGS, cmdName);
					return;
				}
			}
			
			try { call<Params...>(cmd, func); }
			catch(LexcastError const&) { reportError(ERR_BAD_ARGS, cmdName); }
		};
	}

    void unreg(std::string const& cmdName)
    {
        mActionMap.erase(cmdName);
    }

    bool recognize(std::string const& cmdName) const
    {
        return mActionMap.find(cmdName) != mActionMap.end();
    }

    void parse(std::vector<std::string> const& vec) const
    {
        std::string lastCmdName = "";
        CallData cmd;

        for(int i = 0; i < int(vec.size()); ++i)
        {
            if(recognize(vec[i])) // Found a new command?
            {
                if(lastCmdName.length()) // Execute the old one, if there is one.
                {
					exec(cmd);
                    cmd.mStrVec.clear();
                }

                lastCmdName = vec[i];
                cmd.mStrVec.push_back(vec[i]);
            }

            else // Maybe it's an argument?
            {
                if(lastCmdName.length()) // Is there a command to attach it to?
                    cmd.mStrVec.push_back(vec[i]);

                else
					reportError(ERR_UNREC_CMD, vec[i]);
            }
        }

        if(lastCmdName.length())
			exec(cmd);
    }

	void parse(int argc, char const* const* argv) const
	{
		std::vector<std::string> vec;
		
		for(int i = 1; i < argc; ++i)
			vec.push_back(argv[i]);
		
		parse(vec);
	}
	
    void parse(std::string const& str) const
    {
        std::vector<std::string> vec;
        stringToVec(vec, str);
		parse(vec);
    }
};

}

typedef command_parser_details::CommandParser CommandParser;

#endif	/* COMMANDPARSER_HPP */

