//Aicha N Elizabeth R
//Stage 0

#include <stage0.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>

using namespace std;

//removed commented out portion and fixed brackets to match style chosen
/* debugging 
#ifdef DEBUG
  cout << "DEBUG: In function ProgStmt()" << endl;
  cout << "DEBUG: In function ProgStmt(): token = " << token << endl;
#endif
*/


Compiler::Compiler(char **argv) 			 // constructor
{
    	//open sourceFile using argv[1]
  	sourceFile.open(argv[1]);
	//open listingFile using argv[2]
	listingFile.open(argv[2]);
	//open objectFile using argv[3]
	objectFile.open(argv[3]);
	//verify if opened successfully
	/*if(!sourceFile.is_open() || !listingFile.is_open() || objectFile.is_open()){
	//cerr is for errors
		cerr << "Failed to open file" << endl;
	}*/

} 

Compiler::~Compiler()
{           // destructor
	//close all open files
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
	//print "STAGE0:", name(s), DATE, TIME OF DAY
	//print "LINE NO:", "SOURCE STATEMENT"
	//line numbers and source statements should be aligned under the headings
	time_t now = time(NULL);
	listingFile << "STAGE0:  Aicha Ndir, Elizabeth Robles     " << ctime(&now) << endl;
	listingFile << "LINE NO.              " << "SOURCE STATEMENT" << endl << endl;
}

void Compiler::parser()
{
	nextChar();
//ch must be initialized to the first character of the source file
	if (nextToken() != "program")
	{
		//processError(keyword "program" expected);
		processError("keyword \"program\" expected");
	}
//a call to nextToken() has two effects
// (1) the variable, token, is assigned the value of the next token
// (2) the next token is read from the source file in order to make
// the assignment. The value returned by nextToken() is also
// the next token.
	prog();
//parser implements the grammar rules, calling first rule
}

void Compiler::createListingTrailer()
{
	//print "COMPILATION TERMINATED", "# ERRORS ENCOUNTERED"
	//must check if output looks correct and if num of erors is correct
	// Write the termination message with error count
   	listingFile << endl << "COMPILATION TERMINATED      " << errorCount 
                << (errorCount == 1 ? " ERROR ENCOUNTERED" : " ERRORS ENCOUNTERED") 
                << endl;
}

//moved up like pseudocode example
void Compiler::processError(string err)
{
	//Output err to listingFile
	listingFile << "\nError: Line " << lineNo << ": " << err << endl;

	//track number of errors 
	errorCount++; 
	//call func
	createListingTrailer();
	listingFile.close();
	sourceFile.close();
	objectFile.close();
	//Call exit(EXIT_FAILURE) to terminate program
	exit(EXIT_FAILURE);
	//STILL WORKING ON IT 
}


//***********************************************
  // Methods implementing the grammar productions
void Compiler::prog()           // stage 0, production 1
{
	if (token != "program")
		processError("keyword \"program\" expected");
	progStmt();
	if (token == "const")
		consts();
	if (token == "var")
		vars();
	if (token != "begin")
		processError("keyword \"begin\" expected");
	beginEndStmt();
	if (token[0] != END_OF_FILE)  //END_OF_FILE = char Token = string
		processError("No text may follow \"end\""); //mac keys output different " can cause error
}

void Compiler::progStmt()       // stage 0, production 2
{
	string x;
	if (token != "program")
		processError("keyword \"program\" expected");
	x = nextToken();
	if (!isNonKeyId(token))					//token is not a NON_KEY_ID
		processError("program name expected");
	if (nextToken() != ";")
		processError("semicolon expected");
	nextToken();
	code("program", x);
	insert(x, PROG_NAME, CONSTANT, x, NO, 0);
	
}

void Compiler::consts()          // stage 0, production 3, part of the syntax analysis phase, where it parses and validates constant declarations in the PascalLite program
{
    // Check if the current token is "const"
    if (token != "const")
    {
        processError("keyword \"const\" expected");
    }

    // Advance to the next token
    nextToken();

    // Ensure the next token is a non-keyword identifier
    if (!isNonKeyId(token))
    {
        processError("non-keyword identifier must follow \"const\"");
    }

    // Call the function to handle constant statements
    constStmts();
}

void Compiler::vars()            // stage 0, production 4
{
	if (token != "var")
		processError("keyword \"var\" expected");
	if (!isNonKeyId(nextToken()))
		processError("non-keyword identifier must follow \"var\"");
	varStmts();
}

void Compiler::beginEndStmt()    // stage 0, production 5
{
	if (token != "begin")
		processError("keyword \"begin\" expected");
	if (nextToken() != "end")
		processError("keyword \"end\" expected");
	if (nextToken() != ".")
		processError("period expected");
	nextToken();
	code("end", ".");
	//may need to check for after end...
}

void Compiler::constStmts()      // stage 0, production 6
{
    string x, y;
    //check for _ at end to satisfy 20 and middle _ _ for 49 and up
    //.back() – Direct access to the last character.
    if (token.back() == '_')
    {
    	processError("Illegal symbol: \"_\"");
    }
    //loop for double underscore 
    int size = token.size() - 1;
    for (int i = 1; i < size; i++)
    {
    	if (token[i] == '_')
    	{
    		i++;
    		if (token[i] == '_')
    		{
    		    processError("Illegal symbol: \"__\"");
    		}
    	}
    }
	
    // Check if the token is a non-keyword identifier
    if (!isNonKeyId(token))
    {
        processError("non-keyword identifier expected");
    }

    // Save the identifier
    x = token;

    // Advance to the '=' token
    if (nextToken() != "=")
    {
        processError("'=' expected after constant identifier");
    }

    // Advance to the right-hand side of the assignment
    y = nextToken();

	// Check for valid right-hand side in bool
	if (y == "not")
	{
		nextToken(); // Move to the token after 'not'
	
		// Check if the token is a boolean literal
		if (isBoolean(token)) 
		{
			y = (token == "true") ? "false" : "true"; // Negate the boolean
		}
		else if (isNonKeyId(token)) 
		{
			// Check if the non-keyword identifier is already declared as a constant
			if (token == "yes" || token == "no") // Replace with the actual constants you expect
			{
				// Negate the boolean value of the identifier
				y = (token == "yes") ? "false" : "true";
			}
			else
			{
				processError("boolean constant expected after 'not'");
			}
		}	
		else 
		{
			processError("boolean expected after 'not'");
		}
	}
    else if (y == "+" || y == "-")
    {
        // Handle signed integers
        string sign = y;    // Save the sign
        y = nextToken();    // Move to the integer

        if (!isInteger(y))
        {
            processError("integer expected after sign");
        }
        y = sign + y;       // Combine the sign and the integer
    }
    else if (!isLiteral(y) && !isNonKeyId(y)) // covers also also boolean literals, BUT
    {
        processError("literal or non-keyword identifier expected after '='");
    }
    // can add this for clarity
    else if (y == "true" || y == "false")
	{
		// Valid boolean literal assignment
		// Nothing more needed, `y` already holds the correct value
	}

    // Advance to the ';' token
    if (nextToken() != ";")
    {
        processError("';' expected after constant declaration");
    }

    // Insert the constant into the symbol table
    insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);

    // Check if another constant declaration follows
    nextToken(); // Look ahead to the next token
    if (token == "begin" || token == "var" || isNonKeyId(token))
    {
        if (isNonKeyId(token)) // If a new constant declaration
        {
            constStmts(); // Recursively process additional declarations
        }
    }
    else
    {
        processError("non-keyword identifier, 'begin', or 'var' expected");
    }
}

void Compiler::varStmts()        // stage 0, production 7 : making sure non-key id either 
{
    string x, y;

	    //check for _ at end to satisfy 20 and middle _ _ for 49 and up
    //.back() – Direct access to the last character.
    if (token.back() == '_')
    {
    	processError("Illegal symbol: \"_\"");
    }
	    //loop for double underscore 
    int size = token.size() - 1;
    for (int i = 1; i < size; i++){
    	if (token[i] == '_'){
    		i++;
    		if (token[i] == '_'){
    		    processError("Illegal symbol: \"__\"");
    		}
    	}
    }
    // Ensure the current token is a non-keyword identifier
    if (!isNonKeyId(token))
    {
        processError("non-keyword identifier expected");
    }
    // Parse the list of identifiers and save them
    x = ids();
    // Ensure the current token is ":"
    if (token != ":")
    {
        processError("\":\" expected");
    }
    // Move to the type token
    if (nextToken() != "integer" && token != "boolean")
    {
        processError("illegal type follows \":\"");
    }
    // Save the type
    y = token;

    // Ensure the declaration ends with a semicolon
    if (nextToken() != ";")
    {
        processError("';' expected");
    }

    // Insert the variable(s) into the symbol table
    if (y == "integer")
    	insert(x, INTEGER, VARIABLE, "1", YES, 1);
	else
	    insert(x, BOOLEAN, VARIABLE, "1", YES, 1);
    // Move to the next token
    if (nextToken() != "begin" && !isNonKeyId(token))
    {
        processError("non-keyword identifier or 'begin' expected");
    }

    // Recursively process additional variable declarations
    if (isNonKeyId(token))
    {
        varStmts();
    }
}


string Compiler::ids()           // stage 0, production 8 : same -, non-key id
{
    string temp, tempString;

    // Ensure the current token is a non-keyword identifier
    if (!isNonKeyId(token))
    {
        processError("non-keyword identifier expected");
    }

    // Save the current identifier
    tempString = token;
    temp = token;

    // Check if a comma follows the identifier
    if (nextToken() == ",")
    {
        // Move to the next identifier after the comma
        if (!isNonKeyId(nextToken()))
        {
            processError("non-keyword identifier expected after ','");
        }

        // Recursively parse additional identifiers and concatenate them
        tempString = temp + "," + ids();
    }

    return tempString;
}

//**************************************************
  // Helper functions for the Pascallite lexicon
bool Compiler::isKeyword(string s) const   // determines if s is a keyword
{
    string keywords[] = {		// we're using set so it's easier to navigate through membership instead of looping through a list
        "program", "begin", "end", "const", "var", 
        "integer", "boolean", "if", "then", "else", 
        "while", "do", "not", "true", "false"
    };
	for (int i = 0; i < 15; i++){
		if (keywords[i] == s){
			return true;
		}
	}
	return false;
}

bool Compiler::isSpecialSymbol(char c) const  // determines if c is a special symbol
{
    char specialSymbols[] = {
        ';', ',', '.', ':', '(', ')', '*', '/', '=', '<', '>', ':', '+', '-'
    };
	for (int i = 0; i < 14; i++){
		if (specialSymbols[i] == c){
			return true;
		}
	}
	return false;
}

bool Compiler::isNonKeyId(string s) const  // determines if s is a non_key_id
{
    // Check if the string is empty or starts with a non-alphabetic character
    if (s.empty() || !isalpha(s[0]))
    {
        return false; // Must start with a letter
    }

    // Check that all characters are alphanumeric or underscores
    for (char c : s)
    {
        if (!isalnum(c) && c != '_')
        {
            return false; // Only alphanumeric and underscores are allowed
        }
    }

    // Ensure the string is not a keyword
    return !isKeyword(s);
}

bool Compiler::isInteger(string s) const {
    if (s.empty()) return false; // Empty string is not an integer

    // Check for a negative sign at the start
    size_t start = 0;
    if (s[0] == '-' || s[0] == '+') {
        if (s.length() == 1) return false; // A single '-' is not a number
        start = 1; // Skip the negative sign for digit check
    }

    // Ensure all remaining characters are digits
    for (size_t i = start; i < s.length(); i++) {
        if (!isdigit(s[i])) {
            return false;
        }
    }

    return true;
}

bool Compiler::isBoolean(string s) const   // determines if s is a boolean
{
    return s == "true" || s == "false";
}

bool Compiler::isLiteral(string s) const   // determines if s is a literal, combines the results of isInteger and isBoolean, 
{					   // as literals in PascalLite are either integers or booleans.
    return isInteger(s) || isBoolean(s);
}


//*********************************************
    // Action routines
void Compiler::insert(string externalName, storeTypes inType, modes inMode,
              string inValue, allocation inAlloc, int inUnits)          
{
    string name;
    size_t i = 0; // Index for parsing the externalName string

    // Iterate over the externalName list
    while (i < externalName.length())
    {
        name = "";

        // Extract individual names from the comma-separated list
        while (i < externalName.length() && externalName[i] != ',')
        {
            name += externalName[i];
            i++;
        }

        // Handle the case when name is extracted
        if (!name.empty())
        {
            // Check if name is already defined in the symbol table
            if (symbolTable.find(name) != symbolTable.end())
            {
                processError("symbol " + name + " is multiply defined");
            }
            // Check if name is a reserved keyword
            else if (isKeyword(name))
            {
                processError("illegal use of " + name + " keyword");
            }
            else
            {
                // Create a table entry based on the case of the first character of name
                if (isupper(name[0])) // Name begins with an uppercase letter
                {
                    //symbolTable[name] = SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits);
                    symbolTable.insert({name.substr(0, 15), SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)});
                }
                else // Name does not begin with an uppercase letter
                {
                    //symbolTable[name] = SymbolTableEntry(
                    //    genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits);
                    symbolTable.insert({name.substr(0, 15), SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)});
                }
            }
        }

        // Prevent symbol table overflow
        if (symbolTable.size() > 256)
        {
            processError("symbol table overflow");
        }

        // Move past the comma (if present) for the next iteration
        if (i < externalName.length() && externalName[i] == ',')
        {
            i++;
        }
    }
}
              
storeTypes Compiler::whichType(string name) // tells which data type a name has
{
	storeTypes dataType;
	if (isLiteral(name)){
		if (isBoolean(name))
			dataType = BOOLEAN;
		else
			dataType = INTEGER;
	}
	else //name is an identifier and hopefully a constant
	{
		if(symbolTable.find(name) != symbolTable.end()) 							//(isSpecialSymbol(name))
			dataType = symbolTable.find(name)->second.getDataType();
		else
			processError("reference to undefined constant");
	}
	return dataType;
  }
  
  /*
  	auto e = symbolTable.find(name);
		if (e != symbolTable.end()){
			type = e->second.getDataType();
			*/
  
string Compiler::whichValue(string name) // tells which value a name has
{
    string value;			//direcly assigns the value for literals without specific handling for "true" or "false," assuming isLiteral(name) confirms it.

    // Check if the name is a literal
    if (isLiteral(name))
    {
        if(name == "true")
        	value = "-1";
        else if (name == "false")
        	value = "0";
        else
            value = name; // Directly assign the literal as the value
        
    }
    else // Otherwise, name is an identifier and hopefully a constant
    {
        // Check if the name exists in the symbol table
        if (symbolTable.find(name) != symbolTable.end())
        {
            value = symbolTable.at(name).getValue(); // Retrieve the value from the symbol table
        }
        else
        {
            processError("reference to undefined constant: " + name); // Handle undefined constant
        }
    }

    return value; // Return the resolved value
}


void Compiler::code(string op, string operand1, string operand2)
{
    if (op == "program")
    {
        emitPrologue(operand1);
    }
    else if (op == "end")
    {
        emitEpilogue();
    }
    else
    {
        processError("compiler error: function code should not be called with illegal arguments " + op);
    }
}


//*********************************************
  // Emit Functions
void Compiler::emit(string label, string instruction, string operands,
            string comment)
{
    // Turn on left justification in objectFile
    objectFile << left;

    // Output label in a field of width 8
    objectFile << setw(8) << label;

    // Output instruction in a field of width 8
    objectFile << setw(8) << instruction;

    // Output the operands in a field of width 24
    objectFile << setw(24) << operands;

    // Output the comment, if provided
    if (!comment.empty())
    {
        objectFile << "; " << comment;
    }
    
    // debugging  
   // objectFile << setw(8) << comment << endl;

    // End the line
    objectFile << endl;
}

void Compiler::emitPrologue(string progName, string operand2)
{

	time_t now = time(NULL);

    // Output identifying comments at the beginning of the object file
    objectFile << "; Aicha Ndir Elizabeth Robles " << setw(8) << right << ctime(&now);

    // Output the %INCLUDE directives
    objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"" << endl << endl;

    // Emit the SECTION directive
    emit("SECTION", ".text");

    // Emit the global entry point directive
    emit("global", "_start", "", "program " + progName);
	objectFile << endl;
	
    // Emit the starting label
    emit("_start:");
}

void Compiler::emitEpilogue(string operand1, string operand2)
{
    // Emit the exit instruction with a placeholder for exit code
    emit("", "Exit", "{0}");
    //after exit space
	objectFile << endl;
    // Emit the storage directives (for data and uninitialized variables)
    emitStorage();
}

void Compiler::emitStorage()
{
    // Declare iterator for the symbol table
    map<string, SymbolTableEntry>::iterator i;

    // Emit the .data section for constants
    emit("SECTION", ".data");
    for (i = symbolTable.begin(); i != symbolTable.end(); ++i)
    {
        if (i->second.getAlloc() == YES && i->second.getMode() == CONSTANT)
        {
            // Emit a line for each constant
            emit(i->second.getInternalName(), "dd", i->second.getValue(), "" + i->first);
        }
    }

    // Add a blank line for separation
    objectFile << endl;

    // Emit the .bss section for variables
    emit("SECTION", ".bss");
    for (i = symbolTable.begin(); i != symbolTable.end(); ++i)
    {
        if (i->second.getAlloc() == YES && i->second.getMode() == VARIABLE)
        {
            // Emit a line for each variable
            emit(i->second.getInternalName(), "resd", "1", "" + i->first);
        }
    }
}


//******************************************************
  // Lexical routines
char Compiler::nextChar()  // returns the next character or END_OF_FILE marker
{
    static char prevChar = '\n'; // Tracks the previous character to detect new lines

    if (!sourceFile.get(ch)) // Try to get the next character, check for EOF
    {
        ch = END_OF_FILE; // If EOF is reached, set the character to END_OF_FILE
        return ch;
    }

    // Handle line number updates
    if (prevChar == '\n')
    {
        ++lineNo; // Increment line number before logging it
        listingFile << std::setw(5) << lineNo << '|'; // Write the new line number to listingFile
    }

    // Log the character to the listing file
    listingFile.put(ch);

    prevChar = ch; // Update prevChar for line tracking

    return ch; // Return the next character
}

string Compiler::nextToken() // Returns the next token or END_OF_FILE marker (token = program, begin, end, ;, :=)
{
    token.clear(); // Clear the token variable for the next token

    while (token.empty()) // Keep looping until a valid token is found
    {
        if (ch == '{') // Handle comments
        {
            while (nextChar() != END_OF_FILE && ch != '}') 
                ; // Skip characters inside the comment, next char called means ch = "next char" now
		
	    if (ch == END_OF_FILE)
                processError("Unexpected end of file within comment."); // Error for unclosed comments
            else
                nextChar(); // Move past the closing '}', xhatever was before was just a comment
        }
        else if (ch == '}') // Handle standalone '}'
        {
            processError("'}' cannot begin a token.");
            nextChar();
        }
        else if (isspace(ch)) // Skip whitespace characters
        {
            nextChar();
        }
        
        else if (isSpecialSymbol(ch)) // Handle special symbols
        {
            token = ch; // Assign the special symbol to the token
            nextChar();

            // Handle compound operators
            if (token == ":" && ch == '=')
            {
                token += ch;
                nextChar();
            }
            else if (token == "<" && (ch == '>' || ch == '='))
            {
                token += ch;
                nextChar();
            }
            else if (token == ">" && ch == '=')
            {
                token += ch;
                nextChar();
            }
        }
        else if (islower(ch)) // Handle identifiers
        {
            token += ch;

            while (nextChar(), islower(ch) || isdigit(ch) || ch == '_')
                token += ch;

            if (ch == END_OF_FILE)
                processError("Unexpected end of file during identifier parsing.");
        }
        else if (isdigit(ch)) // Handle numeric constants
        {
            token = ch;

            while (nextChar(), isdigit(ch))
                token += ch;

            if (ch == END_OF_FILE)
                processError("Unexpected end of file during number parsing.");
        }
        else if (ch == END_OF_FILE) // Handle EOF
        {
            token = ch; // Set token to END_OF_FILE
        }
        else // Handle illegal symbols
        {
            token = ch; // Assign the illegal character to token
            processError("Illegal symbol: " + token); // Log an error
            nextChar(); // Move to the next character to avoid infinite loops
        }
    }

    // Truncate token if it exceeds allowed length
   /* if (token.length() > 15)
    {
        processError("Token exceeds maximum length: " + token.substr(0, 15));
        token = token.substr(0, 15);
    }*/
    token = token.substr(0, 15);

    return token; // Return the identified token
}


  // Other routines
string Compiler::genInternalName(storeTypes stype) const
{
    static int intCount = 0;   // Counter for integer internal names
    static int boolCount = 0;  // Counter for boolean internal names
    string internalName;

    if (stype == PROG_NAME)
    {
    	internalName = "P0";
    }
    else if (stype == INTEGER)
    {
    	internalName = "I" + to_string(intCount);
    	intCount++;
  	}

  	else if (stype == BOOLEAN)
  	{
    	internalName = "B" + to_string(boolCount);
    	boolCount++;
  	}
  	return internalName;
}
    
