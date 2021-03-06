#include "Potatoes.h"

Potatoes::Potatoes()
	: m_tokenizer(Tokenizer()),
	m_currentLineString(""),
	m_keywordLUT(m_tokenizer.getKeywordArray()),
	m_isInitialized(false),
	m_operatorIterator(1),
	m_operatorList{}	
{

	initializeOperatorList();
	// print("Done building regex.");
}

Potatoes::~Potatoes()
{

}

KeyWordArray Potatoes::getKeywordLUT()
{
	return m_keywordLUT;
}

void Potatoes::setLine(std::string& line)
{
	m_currentLineString = line;
}

// Check if there are any arithmetic operators 
// WARNING: INCOMING BAD DESIGN DECISION
//	- Operators are not allowed on the same line as the keyword. For now. 
//	- This should speed up development of the parser.

TokenList Potatoes::parseLine()
{
	std::vector<std::string> parsedStringList;
	std::vector<std::string> outputStringList;
	TokenList tokenList;

	std::string tempString = "";

	int pos = -1;

	// print("Before regex matching.");

	//Search line for keyWords first. 

	bool containsKeyword = false;

	containsKeyword = parseForKeyword(outputStringList);
	// std::cout << "Contains Keyword = " << containsKeyword << std::endl;
	// std::cout << "PRINT PARSED STRING LIST" << std::endl;

	if(!containsKeyword)
	{
		outputStringList.clear();
		std::string stringToSplit = m_currentLineString;

		// std::cout << "stringToSplit = " << stringToSplit << '\n';

		//Match Arithmetic Operators
		pos = positionMatch(stringToSplit, m_operatorList[1]);

		if(stringToSplit.size() < 1)
		{
			return tokenList;
		}

		// std::cout << "pos = " << pos << std::endl;

		splitString(outputStringList, stringToSplit, pos);	
	}

	// // std::cout << "Output String List = {";
	// for(auto outputString : outputStringList)
	// {
	// 	std::cout << outputString << ",";
	// }
	// std::cout << "}" << std::endl;
	
	bool isArg = false;

	for (auto& str : outputStringList)
	{
		Token token;

		token = m_tokenizer.generateToken(str, isArg);

		Name tokenName = getName(token);
		// std::cout << "Token = {" << getName(token) << ", " << getValue(token) << "}" << std::endl;

		//Set the isArg flag so the next token that comes after the keyword is an argument. 
		if(tokenName == "KEYWD")
		{
			isArg = true;
		}
		else
		{
			isArg = false;
		}
		

		tokenList.push_back(token);
	}

	return tokenList;
}

bool Potatoes::parseForKeyword(std::vector<std::string>& outputStringList)
{
	//positionMatch method does not work for this, as we are not matching single character literals for the keywords. 

	std::string regExString = m_operatorList[0];

	std::regex rExp(regExString);

	std::smatch m;

	//m will contain an index for the full string, plus all capture groups. 
	// If the capture group contains a match, it will have a string value in it. 
	// Thus it is valid to 

	std::regex_search(m_currentLineString, m, rExp);

	// Check the size of the matches. Specifically the sub matches. 
	// std::cout << "Number of Groups to match = " << m.size() << '\n';

	int numMatches = 0;
	//Skip 0 because that is the full match. 
	for(int i = 1; i < m.size(); i++)
	{
		if(m[i] != "")
		{
			// std::cout << "Match " << i << ": ";
			// std::cout << m[i] << '\n';
			outputStringList.push_back(m[i]);
			numMatches++;
		}
	}

	if(m.empty())
	{
		return false;
	}
	else
	{
		return true;
	}
}

// void Potatoes::splitArithmeticOperators()

int Potatoes::positionMatch(std::string& inputString,std::string inputOper)
{
	std::string regExString = inputOper;

	// std::cout << "RegExString: " << regExString << std::endl;
	std::regex r(regExString);

	std::smatch m;

	int position;

	std::regex_search(inputString, m, r);

	if(m.empty())
	{
		return -1;
	}
	else
	{
		position = m.position(0);
		return position;
	}

}

void Potatoes::initializeOperatorList()
{
	if(!m_isInitialized)
	{
		
		std::string regExKeyWordString = generateKeywordRegEx();

		// print(regExKeyWordString.c_str());

		m_operatorList.push_back(regExKeyWordString);

		// = sign is the highest priority after keywords.
		m_operatorList.push_back("\\=");

		//Place Brackets here once ready to implement

		//Add arithmetic Operators
		m_operatorList.push_back("[\\+\\-\\/\\*\\%\\^]");

		//EmptyBrackets with nothing in the middle indicates a new list. 

		//m_operatorList.push_back("\{\}");

		m_isInitialized = true;
	}
}

//Example Keyword regex generated from this method is:
//
//	(?:[\s]|^)((print)|(do)|(while))\((.*)\)(?=[\s]|$)
//
//	Refer to this example for what the regex is matching. https://regex101.com/r/uRWvgx/5
//
//	Current limitation is even keywords like break will require parenthesis
// 		which is like syntactic salt not sugar :(
//
//	Uses the keyword Look up table to generate the regex to match. 

std::string Potatoes::generateKeywordRegEx()
{
	std::string regEx("(?:[\s]|^)(");

	for(int index = 0; index < NUM_KEYWORDS; index++)
	{
		// regEx.append("(");
		regEx.append(m_keywordLUT[index]);
		// regEx.append(")");

		if(index < NUM_KEYWORDS-1)
		{
			regEx.append("|");
		}
	}
	regEx.append(")\\((.*)\\)(?=[\s]|$)");

	return regEx;
}

void Potatoes::splitString(std::vector<std::string>& outputStringList, std::string& inputString, int inputPos)
{

	// std::cout << "inputString: " << inputString << std::endl;
	// std::cout << "inputPos = " << inputPos << '\n';
	
	
	std::string leftSideString; 
	std::string rightSideString;
	
	if(inputPos == -1)
	{		
		leftSideString = "";
		rightSideString = inputString;

		outputStringList.push_back(rightSideString);

	}
	else
	{
		leftSideString = inputString.substr(0,inputPos);
		rightSideString = inputString.substr(inputPos+1, inputString.size()-1);

		outputStringList.push_back(leftSideString);
		outputStringList.push_back(inputString.substr(inputPos,1));
	}
	
	// std::cout << leftSideString << "      " << rightSideString << std::endl; 

	//Manage the priority operator calls.

	// std::cout << "Operator Iterator Before change = " << m_operatorIterator << '\n';

	if(m_operatorIterator >= 2)
	{
		m_operatorIterator = 2;
	}
	else
	{
		m_operatorIterator++;
	}

	// std::cout << "Operator Iterator = " << m_operatorIterator << '\n';

	std::string opString;

	// std::cout << rightSideString << '\n';
	
	int pos = positionMatch(rightSideString,m_operatorList[m_operatorIterator]);
	// std::cout << "pos = " << pos << std::endl;

	if(pos != -1)
	{
		splitString(outputStringList,rightSideString, pos);
	}
	else
	{
		//Need a better state handler for if there are no arithmetic operators. 
		outputStringList.push_back(rightSideString);
		return;
	}
}
