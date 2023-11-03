#ifdef _WIN32
#include <Windows.h>
#define MY_SLEEP(time) Sleep(time * 1025 / 1000)
#else
#include <unistd.h>
#define MY_SLEEP(time) usleep(time * 1025)
#endif

#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>
#include <sstream>

#define newl std::cout << "\n";
constexpr bool IGNORE_SPACES = true;

struct textDisplay
{
    std::string text;
    std::vector<std::string> params = {};
};

std::unordered_map<std::string, std::string> colorMap = {
    {"red", "31m"},    {"green", "32m"},  {"blue", "34m"}, {"black", "30m"},
    {"yellow", "33m"}, {"purple", "35m"}, {"cyan", "36m"}, {"white", "37m"},
};

std::unordered_map<std::string, std::string> formattingParams = {
    {"bold", "1;"},
    {"underline", "4;"},
    {"strikethrough", "9;"},
};

std::unordered_map<std::string, int> alignments = {
    {"left-", 20},   {"left", 50},   {"left+", 80},   {"center", 100},
    {"right-", 140}, {"right", 180}, {"right+", 200},
};

void console_print(const char& text, const std::string& fullcode = "")
{
    if (!fullcode.empty())
    {
        if (text != ' ')
        {
            std::cout << fullcode << text << "\033[0m" << std::flush;
        } else
        {
            std::cout << text << std::flush;
        }
    } else
    {
	    std::cout << text << std::flush;
    }
    
};

void type_text(const std::string & text, unsigned int time = 0,
    const std::vector<std::string>& params = {}) {

	std::string color = "white";
    std::string formattingCode = "\033[0;";
    
    int align = 0;
    int paramCount = 0;
    const unsigned int sleep_time = time / text.size();


    for (const std::string& param : params) {
        if (colorMap.find(param) != colorMap.end()) {
            color = param;
        }
        if (formattingParams.find(param) != formattingParams.end()) {
            formattingCode += formattingParams[param];
            paramCount++;
        }
        if (alignments.find(param) != alignments.end()) {
            align = alignments[param];
        }
    }

    const std::string colorCode = colorMap[color];
    const std::string fullCode = "\033[" + formattingCode + colorCode;

    if (align != 0) {
        std::cout << std::setw(align - text.size() / 2 + paramCount * 2);
    }

    for (std::size_t i = 0; i < text.size(); ++i) {
        console_print(text[i], fullCode);
        MY_SLEEP(sleep_time);
    }
}

std::string get_full_code(const std::vector<std::string>& params)
{
    std::string color = "white";
    std::string formattingCode = "\033[0;";

    int paramCount = 0;

    for (const std::string& param : params) {
        if (colorMap.find(param) != colorMap.end()) {
            color = param;
        }
        if (formattingParams.find(param) != formattingParams.end()) {
            formattingCode += formattingParams[param];
            paramCount++;
        }
    }

    const std::string colorCode = colorMap[color];
    const std::string fullCode = "\033[" + formattingCode + colorCode;
    return fullCode;

}

unsigned int make_same_size(std::vector<textDisplay> &text_values)
{
    unsigned int iterations = 0;
    unsigned int longest_text = 0;
    for (textDisplay item : text_values)
    {
        if (iterations == 0)
        {
            longest_text = item.text.length();
        }
        else
        {
            if (item.text.length() > longest_text)
            {
                longest_text = item.text.length();
            }
        }
        iterations++;
    }

    for (textDisplay &item : text_values)
    {
        const unsigned int diff_length = longest_text - item.text.length();
	    if (diff_length != 0)
	    {
		    for (int i = 0; i < diff_length; i++)
		    {
                item.text += " ";
		    }
	    }
    }
    return longest_text;
}

std::vector<textDisplay> createDisplay(const textDisplay& display_main, const char* longString, std::vector<std::string> params = {}) {
    if (std::strlen(longString) == 0) return {};

    std::vector<textDisplay> resultDisplays;
    std::stringstream ss(longString);
    std::string line;

    size_t index = 0;
    while (std::getline(ss, line, '\n')) {
        if (index < 1) {
            textDisplay display = display_main;
            display.text = line;
            display.params = params;
            resultDisplays.push_back(display);
        }
        else {
            textDisplay display;
            display.text = line;
            display.params = params;
            resultDisplays.push_back(display);
        }
        index++;
    }


    return resultDisplays;
}


void multi_text(std::vector<textDisplay> text_values, unsigned int time = 0)
{
    if (text_values.empty()) return;
    if (text_values.size() == 1)
    {
        type_text(text_values[0].text, time, text_values[0].params);
        return;
    }

	std::vector<std::string> full_code;
    full_code.reserve(text_values.size());
    const unsigned int text_len = make_same_size(text_values);

    std::string color = "white";
    std::string formattingCode = "\033[0;";
    
    const unsigned int sleep_time = time / text_len;


    for (textDisplay &items : text_values)
    {
        full_code.push_back(get_full_code(items.params));
    }


    constexpr char esc(27);

    const std::string linesAbove = std::to_string(text_values.size() - 1);
    for (std::size_t i = 0; i < text_len; ++i) {
        
        
        console_print(text_values[0].text[i], full_code[0]);

        for (int j = 1; j < text_values.size(); j++)
        {
            std::cout << esc << "[1B";
            std::cout << "\033[1D";
            
            console_print(text_values[j].text[i], full_code[j]);
            
        }

        std::cout << esc << "[" + linesAbove + "A";
        MY_SLEEP(sleep_time);
    }
    std::cout << esc << "[" + linesAbove + "B";
}



class pBar {
public:
    void update(double newProgress) {
        currentProgress += newProgress;
        amountOfFiller = (int)((currentProgress / neededProgress) * (double)pBarLength);
    }
    void print() {
        using namespace std;
        currUpdateVal %= pBarUpdater.length();
        cout << "\r" //Bring cursor to start of line
            << firstPartOfpBar; //Print out first part of pBar
        for (int a = 0; a < amountOfFiller; a++) { //Print out current progress
            cout << pBarFiller;
        }
        cout << pBarUpdater[currUpdateVal];
        for (int b = 0; b < pBarLength - amountOfFiller; b++) { //Print out spaces
            cout << " ";
        }
        cout << lastPartOfpBar //Print out last part of progress bar
            << " (" << (int)(100 * (currentProgress / neededProgress)) << "%)" //This just prints out the percent
            << flush;
        currUpdateVal += 1;
    }
    std::string firstPartOfpBar = "[", //Change these at will (that is why I made them public)
        lastPartOfpBar = "]",
        pBarFiller = "|",
        pBarUpdater = "/-\\|";
private:
    int amountOfFiller,
        pBarLength = 50, //I would recommend NOT changing this
        currUpdateVal = 0; //Do not change
    double currentProgress = 0, //Do not change
        neededProgress = 100; //I would recommend NOT changing this
};

void progressBar(const unsigned int &time = 5000, const int& TotalBarLength = 50, const double& progressIncrement = 1, const int& NeededProgress = 100,
    const std::vector<std::string>& Symbols = {"[", "]", "|", "/-\\|"})
{
    //User Defines
    double newProgress = progressIncrement;

	//Variables
    int barLength = TotalBarLength, updateVal = 0, amountOfFiller;
    double currentProgress = 0, neededProgress = NeededProgress;

    //Symbols
    std::string firstPartOfpBar = Symbols[0],
        lastPartOfpBar = Symbols[1],
        pBarFiller = Symbols[2],
        pBarUpdater = Symbols[3];

    auto update = [&]()
    {
        currentProgress += newProgress;
        amountOfFiller = static_cast<int>((currentProgress / neededProgress) * static_cast<double>(barLength));
    };

    auto print = [&]()
    {
        updateVal %= pBarUpdater.length();
        std::cout << "\r" << firstPartOfpBar;

        for (int a = 0; a < amountOfFiller; a++) { 
	        std::cout << pBarFiller;
        }

        std::cout << (currentProgress != NeededProgress ? pBarUpdater[updateVal] : pBarFiller[0]);

        for (int b = 0; b < barLength - amountOfFiller; b++) { 
	        std::cout << " ";
        }
        std::cout << lastPartOfpBar 
            << " (" << static_cast<int>(100 * (currentProgress / neededProgress)) << "%)" 
            << std::flush;
        updateVal += 1;
    };

    const float sleepTime = time / NeededProgress;

    for (int i = 0; i < 100; i++)
    {
        update();
        print();
        MY_SLEEP(sleepTime);
    }
}

int main() {


    textDisplay display1 = {"Hello World! This is the first phrase!", {"blue", "bold", "underline"}};
    textDisplay display2 = { "Hello World! Second Phrase Here!", {"yellow", "bold", "underline", "strikethrough"}};
    textDisplay display3 = { "Hello World! Wow, the third phrase is finally done!", {"purple", "underline"}};
    textDisplay long_display;


    const char* longString = R""""(                                                 
        ,--,                                     
      ,--.'|            ,--,    ,--,              
   ,--,  | :          ,--.'|  ,--.'|             
,---.'|  : '          |  | :  |  | :     ,---.   
|   | : _' |          :  : '  :  : '    '   ,'\  
:   : |.'  |   ,---.  |  ' |  |  ' |   /   /   | 
|   ' '  ; :  /     \ '  | |  '  | |  .   ; ,. : 
'   |  .'. | /    /  ||  | :  |  | :  '   | |: :
|   | :  | '.    ' / |'  : |__'  : |__'   | .; : 
'   : |  : ;'   ;   /||  | '.'|  | '.'|   :    |
|   | '  ,/ '   |  / |;  :    ;  :    ;\   \  /  
;   : ;--'  |   :    ||  ,   /|  ,   /  `----'   
|   ,/       \   \  /  ---`-'  ---`-'            
'---'         `----'                             
)"""";



    //type_text("Lorem ipsum dolor sit amet", 750, { "red", "bold", "underline", "strikethrough"});
    newl
    //multi_text({ display1,display2,display3 }, 950);
    newl
	//multi_text(createDisplay(long_display, longString, {"green"}), 950);
    newl
	//std::cout << "Tests have Finished!";
    newl
	progressBar(5000, 50, 1,200);
    
}