//
//  main.cpp
//  render
//
//  Created by Raymond Kao on 11/14/23.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
using namespace std;
#include <sstream>
#include <streambuf>
#include <cassert>

const int MaxInput = 181;

int countLines(istream& inf);
void testRender(int lineLength, const char input[], const char expectedOutput[], int expectedReturnValue);
int render(int lineLength, istream& inf, ostream& outf);
bool nextWord(char line[], char currentWord[], int& wholeLineCounter, int& wordCounter, int& wordPortion);
void greaterLineLength(char currentWord[], int lineLength, int& lineLengthUpdate, ostream& outf, bool& wordPrinted);

int countLines(istream& inf)   // inf is a name of our choosing
{
    int lineCount = 0;
    string line;
    while (getline(inf, line))
        lineCount++;
    return lineCount;
}

void testRender(int lineLength, const char input[], const char expectedOutput[], int expectedReturnValue)
{
    istringstream iss(input);
    ostringstream oss;
    ostringstream dummy;
    streambuf* origCout = cout.rdbuf(dummy.rdbuf());
    int retval = render(lineLength, iss, oss);
    cout.rdbuf(origCout);
    if ( ! dummy.str().empty())
        cerr << "WROTE TO COUT INSTEAD OF THIRD PARAMETER FOR: " << input << endl;
    else if (retval != expectedReturnValue)
        cerr << "WRONG RETURN VALUE FOR: " << input << endl;
    else if (retval == 2)
    {
        if ( ! oss.str().empty())
            cerr << "WROTE OUTPUT WHEN LINELENGTH IS " << lineLength << endl;
    }
    else if (oss.str() != expectedOutput){
        cerr << "WRONG RESULT FOR: " << input << endl;
        cerr << "OUTPUT WAS: " << endl << oss.str() << endl;
    }
}

int main()
{
    const int MAX_FILENAME_LENGTH = 100;
            for (;;)
            {
                cout << "Enter input file name (or q to quit): ";
                char filename[MAX_FILENAME_LENGTH];
                cin.getline(filename, MAX_FILENAME_LENGTH);
                if (strcmp(filename, "q") == 0)
                    break;
                ifstream infile(filename);
                if (!infile)
                {
                    cerr << "Cannot open " << filename << "!" << endl;
                    continue;
                }
                cout << "Enter maximum line length: ";
                int len;
                cin >> len;
                cin.ignore(10000, '\n');
                int returnCode = render(len, infile, cout);
                cout << "Return code is " << returnCode << endl;
            }
}
int render(int lineLength, istream& inf, ostream& outf){
    char line[MaxInput];
    char currentWord[MaxInput];
    int counter = 0;
    int wordcounter = 0;
    int lineLengthUpdate = lineLength;
    bool containsPunctuation = false;
    bool previousParagraphBreak = false;
    bool returnTest = false;
    bool wordPrinted = false;
    
    int wordPortion = 0;
    bool wasWordP = false;
    
    if (lineLength < 1){
        return 2;
    }
    
    while (inf.getline(line, MaxInput) ){
        while(nextWord(line, currentWord, counter, wordcounter, wordPortion)){// go word by word
            if (wordcounter == 1 && strcmp(currentWord, "@P@") == 0) { //if its the first @P@ dont do anything
                currentWord[0] = '\0';
                continue;
            }
            if (wordcounter != 1 && strcmp(currentWord, "@P@") == 0) { //if its a @P@ wait till nextt word before doing something
                if (!previousParagraphBreak) {
                    previousParagraphBreak = true;
                }
                continue;
            }
            if ((int)strlen(currentWord) > lineLength) { //if lenght of the word is less than character count
                greaterLineLength(currentWord, lineLength, lineLengthUpdate, outf, wordPrinted);
                wordPrinted = true;
                returnTest = true;
                
            }
            else if (lineLengthUpdate == lineLength) { //print first word
                if(previousParagraphBreak){
                    outf << endl;
                    outf << endl;
                    lineLengthUpdate = lineLength;
                }
                outf << currentWord;
                wordPrinted = true;
                lineLengthUpdate = lineLength - (int)strlen(currentWord); //remaining characters in the line
                if (strcmp(currentWord, "@P@") != 0){
                    previousParagraphBreak = false;
                }
            }
            else if (containsPunctuation) {//if theres a punctuation need two spaces
                if(strlen(currentWord)+2 <= lineLengthUpdate){
                    if(previousParagraphBreak){
                        outf << endl;
                        outf << endl;
                        lineLengthUpdate = lineLength;
                    }
                    if (!previousParagraphBreak) {
                        outf << ' ' << ' ';
                    }
                    outf << currentWord;
                    wordPrinted = true;
                    lineLengthUpdate = lineLengthUpdate - 2 - (int)strlen(currentWord);
                    containsPunctuation = false;
                    if (strcmp(currentWord, "@P@") != 0){
                        previousParagraphBreak = false;
                    }
                }
                else{
                    outf << endl;
                    outf << currentWord;
                    wordPrinted = true;
                    lineLengthUpdate = lineLength - (int)strlen(currentWord);
                    containsPunctuation = false;
                    if(strcmp(currentWord, "@P@") != 0){
                        previousParagraphBreak = false;
                    }
                }
            }
            else if(wasWordP){ //if its a word portion to handle hyphens
                if(strlen(currentWord) <= lineLengthUpdate){
                    if(previousParagraphBreak){
                        outf << endl;
                        outf << endl;
                        lineLengthUpdate = lineLength;
                    }
                    outf << currentWord;
                    wordPrinted = true;
                    lineLengthUpdate = lineLengthUpdate - (int)strlen(currentWord);
                    containsPunctuation = false;
                    if (strcmp(currentWord, "@P@") != 0){
                        previousParagraphBreak = false;
                    }
                }
                else{
                    outf << endl << currentWord;
                    wordPrinted = true;
                    lineLengthUpdate = lineLength - (int)strlen(currentWord);
                    containsPunctuation = false;
                    if(strcmp(currentWord, "@P@") != 0){
                        previousParagraphBreak = false;
                    }
                }
            }
            else if (strlen(currentWord) >= lineLengthUpdate) { // if the word doesnt fit on the line and it fits on the next line
                if(previousParagraphBreak){
                    outf << endl;
                    outf << endl;
                    lineLengthUpdate = lineLength;
                }
                if (!previousParagraphBreak) {
                    outf << endl;
                }
                outf << currentWord;
                wordPrinted = true;
                lineLengthUpdate = lineLength - (int)strlen(currentWord);
                if (strcmp(currentWord, "@P@") != 0){
                    previousParagraphBreak = false;
                }
            }
            else if (strlen(currentWord) < lineLengthUpdate) {
                if(previousParagraphBreak){
                    outf << endl;
                    outf << endl;
                    lineLengthUpdate = lineLength;
                }
                if (!previousParagraphBreak) {
                    outf << ' ';
                }
                outf << currentWord;
                wordPrinted = true;
                lineLengthUpdate = lineLengthUpdate - 1 - (int)strlen(currentWord); //minus one to account for space
                if (strcmp(currentWord, "@P@") != 0){
                    previousParagraphBreak = false;
                }
            }
            if (currentWord[strlen(currentWord)-1] == '!' || currentWord[strlen(currentWord)-1] == '.' || currentWord[strlen(currentWord)-1] == ':' || currentWord[strlen(currentWord)-1] == '?'){
                containsPunctuation = true;
                previousParagraphBreak = false;
            }
            if(wordPortion == 888){ //lucky number but just checking if its a word portion helps handle hyphens
                wasWordP = true;
            }
        }
        counter = 0;
        containsPunctuation = false;
    }
    if(wordPrinted){
        outf << endl; //only print new line if theres a word printed like @P@ @P@ @P@ should not print new lines
    }
    if (returnTest) {
        return 1; //returns one if word is longer than character count
    }
    else {
        return 0;
    }
}

bool nextWord(char line[], char currentWord[], int& wholeLineCounter, int& wordCounter, int& wordPortion) {
    int CstringCounter = 0;
    wordPortion = 0;
    while (isspace(line[wholeLineCounter])) { //gets rid of extra spaces
        wholeLineCounter++;
    }
    if (line[wholeLineCounter] == '\0') { //where it ends
        return false;
    }
    while (line[wholeLineCounter] != '\0' && !isspace(line[wholeLineCounter]) && line[wholeLineCounter] != '-' && CstringCounter < MaxInput-1 ) {
        currentWord[CstringCounter++] = line[wholeLineCounter++]; //basically saves each word
    }
    if(line[wholeLineCounter] == '-') {//treat word portions
        if(!isspace(line[wholeLineCounter + 1]) && line[wholeLineCounter+1] != '\0'){
            wordPortion = 888;
        }
        currentWord[CstringCounter++] = line[wholeLineCounter++];
    }
    currentWord[CstringCounter] = '\0';
    wordCounter++;
    return true;
}

void greaterLineLength(char currentWord[], int lineLength, int& lineLengthUpdate, ostream& outf, bool& wordPrinted) {
    char smallerWord[MaxInput];
    if(strlen(currentWord) > lineLength) {
        if (lineLength != lineLengthUpdate) { //if its not at the start already make it a new line
            outf << endl;
        }
        int k = 0;
        for (k = 0; k < strlen(currentWord);) { //loops through and prints each new word
            for (int i = 0; i < lineLength; i++) {
                smallerWord[i] = currentWord[k];
                k++;
                smallerWord[i+1] = '\0';
                if (k >= strlen(currentWord)) {
                    break;
                }
                smallerWord[k] = '\0';
            }
            outf << smallerWord;
            if (k != strlen(currentWord)) {
                outf << endl;
            }
        }
        lineLengthUpdate = lineLength - (int)strlen(smallerWord); //keeps count of the very last part because thats the one that matters to find the remaining line length
        if(k < strlen(currentWord)){
            outf << endl;
            wordPrinted = false; //stops it from printing two new lines at the end of file
        }
        
    }
}
