#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_

#include <sys/stat.h>
#include <string>
#include <unordered_map>

class Buffer;
class HttpResponse {
 public:
    HttpResponse(const std::string& srcDir, std::string& path, bool isKeepAlive, int code = -1);
    ~HttpResponse();

    void makeResponse(Buffer& buff);
    void errorContent(Buffer& buff, const char* errorMessage);
    int code() const { return _code; }
    char* file() const { return _mmFile; }
    size_t fileLen() const { return _mmFileStat.st_size; }

 private:
    void _addStateLine(Buffer& buff);
    void _addHeader(Buffer& buff);
    void _addContent(Buffer& buff);
    void _errorHtml();
    std::string _getFileType() const;

    int _code;
    bool _isKeepAlive;
    std::string _path;
    std::string _srcDir;
    char* _mmFile;
    struct stat _mmFileStat;
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;
};

#endif  // END HTTPRESPONSE_H_
