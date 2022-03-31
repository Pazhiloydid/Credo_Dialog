#include <iostream>
#include <string_view>
#include <fstream>
#include <sstream>
#include <list>

const char delimetr = '\n';

enum class COMMENT_TYPE : int {
    SINGLE,
    MULTI_OPEN,
    MULTI_CLOSE,
    NEW_LINE
};

using std::string;
using std::list;
using std::string_view;


std::string readFile(const string& fileName) {
    std::ifstream f(fileName);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

list<std::pair<COMMENT_TYPE, size_t>> getCommentSymbols(const string& fileData) {
    list<std::pair<COMMENT_TYPE, size_t>> comments;
    for (size_t index = 0; index < fileData.size() - 1;) {
        if (fileData[index] == '/') {
            if (fileData[index + 1] == '/') {
                comments.emplace_back(COMMENT_TYPE::SINGLE, index);
                index += 2;
            }
            else if (fileData[index + 1] == '*') {
                comments.emplace_back(COMMENT_TYPE::MULTI_OPEN, index);
                index += 2;
            }
            else
                index++;
        }
        else if (fileData[index] == '*') {
            if (fileData[index + 1] == '/') {
                comments.emplace_back(COMMENT_TYPE::MULTI_CLOSE, index);
                index += 2;
            }
            else
                index++;
        }
        else if (fileData[index] == '\n') {
            comments.emplace_back(COMMENT_TYPE::NEW_LINE, index);
            index += 1;
        }
        else {
            index++;
        }
    }
    return comments;
}

std::string deleteComments(const string& fileData) {
    list<std::pair<COMMENT_TYPE, size_t>> comments = getCommentSymbols(fileData);
    string result;
    size_t current_index = 0;
    while (comments.size()) {
        if (comments.front().first == COMMENT_TYPE::SINGLE) {
            size_t delPosStart = comments.front().second;
            result += fileData.substr(current_index, delPosStart - current_index);
            while (comments.front().first != COMMENT_TYPE::NEW_LINE) {
                comments.pop_front();
            }
            current_index = comments.front().second + 1;
        }
        else if (comments.front().first == COMMENT_TYPE::MULTI_OPEN) {
            size_t delPosStart = comments.front().second;
            result += fileData.substr(current_index, delPosStart - current_index);
            while (comments.front().first != COMMENT_TYPE::MULTI_CLOSE) {
                comments.pop_front();
            }
            //case int/**/main()
            if (delPosStart + 2 == comments.front().second)
                result += ' ';
            current_index = comments.front().second + 2;
        }
        else {
            comments.pop_front();
        }
    }
    result += fileData.substr(current_index, fileData.size() - current_index);
    return result;
}

int main()
{
    std::ofstream out("out.cpp");
    string fileData = readFile("test1in.cpp");
    string result = deleteComments(fileData);
    //string_view dataView(fileData);
    out << result;
    return 0;
}