#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include <string>
#include <unordered_map>
#include <unordered_set>

class Buffer;
class HttpRequest {
 public:
    HttpRequest();
    ~HttpRequest() = default;

    std::string path() const { return _path; }
    std::string& path() { return _path; }
    std::string method() const { return _method; }
    std::string version() const { return _version; }

    bool parse(Buffer& buff);
    bool isKeepAlive() const;

 private:
    bool _parseRequestLine(const std::string& line);
    void _parsePath();
    void _parseHeader(const std::string& line);
    void _parseBody(const std::string& line);
    void _parsePost();
    void _parseFromUrlEncoded();
    bool userVerify(const std::string& name, const std::string& pwd, bool isLogin);
    int char2Hex(char ch);

    enum PARSE_STATE { REQUEST_LINE, HEADERS, BODY, FINISH };
    PARSE_STATE _state;
    std::string _method, _path, _version, _body;
    std::unordered_map<std::string, std::string> _header, _post;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;
};

#endif  // END HTTPREQUEST_H_
