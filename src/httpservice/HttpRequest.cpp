#include "HttpRequest.h"
#include <mysql/mysql.h>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <regex>
#include "Buffer.h"
#include "Logger.h"

const std::unordered_set<std::string> HttpRequest::DEFAULT_HTML{"/index", "/register", "/login", "/welcome",
                                                                "/video", "/picture",  "/error"};
const std::unordered_map<std::string, int> HttpRequest::DEFAULT_HTML_TAG{
    {"/htmldoc/register.html", 0},
    {"/htmldoc/login.html", 1},
};

HttpRequest::HttpRequest() : _state(REQUEST_LINE), _method(""), _path(""), _version(""), _body("") { _header.clear(); }

bool HttpRequest::parse(Buffer& buff) {
    const std::string CRLF = "\r\n";
    std::string bufferStr = buff.toStr();
    std::size_t readPos = 0;

    while (readPos < bufferStr.size() && _state != FINISH) {
        std::size_t lineEnd = bufferStr.find(CRLF, readPos);
        if (lineEnd == std::string::npos) break;

        std::string line = bufferStr.substr(readPos, lineEnd - readPos);
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        switch (_state) {
            case REQUEST_LINE:
                if (!_parseRequestLine(line)) return false;
                _parsePath();
                LOG_INFO("%s", line.c_str());
                _state = HEADERS;
                break;
            case HEADERS:
                if (line.empty()) {
                    _state = (_method == "POST") ? BODY : FINISH;
                } else {
                    _parseHeader(line);
                }
                LOG_INFO("%s", line.c_str());
                break;
            case BODY:
                if (_method == "POST") {
                    _parseBody(line);
                }
                _state = FINISH;
                LOG_INFO("%s", line.c_str());
                break;
            default:
                break;
        }
        readPos = lineEnd + CRLF.length();
    }

    if (_state == BODY && _method == "POST" && _header.count("Content-Length")) {
        size_t contentLength = std::stoul(_header["Content-Length"]);
        if (bufferStr.size() - readPos >= contentLength) {
            _parseBody(bufferStr.substr(readPos, contentLength));
            _state = FINISH;
        }
    }
    return _state == FINISH;
}

/* @brief 解析 HTTP 请求行 */
bool HttpRequest::_parseRequestLine(const std::string& line) {
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if (regex_match(line, subMatch, patten)) {
        _method = subMatch[1];
        _path = subMatch[2];
        _version = subMatch[3];
        return true;
    }
    return false;
}

void HttpRequest::_parsePath() {
    if (_path == "/") {
        _path = "/htmldoc/index.html";
    } else if (DEFAULT_HTML.count(_path)) {
        _path = "/htmldoc" + _path + ".html";
    }
}

/* @brief 解析 HTTP 请求头部 */
void HttpRequest::_parseHeader(const std::string& line) {
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if (regex_match(line, subMatch, patten)) {
        _header[subMatch[1]] = subMatch[2];
    }
}

bool HttpRequest::isKeepAlive() const {
    auto it = _header.find("Connection");
    return (it != _header.end() && it->second == "keep-alive" && _version == "1.1");
}

/* @brief 解析 HTTP 请求报文主体 */
void HttpRequest::_parseBody(const std::string& line) {
    if (_method == "POST") {
        _body = line;
        _parsePost();
    }
}
void HttpRequest::_parsePost() {
    LOG_INFO("Parsing POST data...");
    if (_method == "POST" && _header["Content-Type"] == "application/x-www-form-urlencoded") {
        _parseFromUrlEncoded();
        LOG_INFO("m_path before verification: %s", _path.c_str());
        if (auto it = DEFAULT_HTML_TAG.find(_path); it != DEFAULT_HTML_TAG.end()) {
            bool isLogin = (it->second == 1);
            if (userVerify(_post["username"], _post["password"], isLogin)) {
                _path = "/htmldoc/welcome.html";
                LOG_INFO("Verification successful, redirecting to welcome.html");
            } else {
                _path = "/htmldoc/error.html";
                LOG_INFO("Verification failed, redirecting to error.html");
            }
        }
        LOG_INFO("m_path after verification: %s", _path.c_str());
    }
}
void HttpRequest::_parseFromUrlEncoded() {
    if (_body.empty()) return;

    std::string key, value;
    size_t i = 0, j = 0;

    while (i < _body.size()) {
        char ch = _body[i];
        switch (ch) {
            case '=':
                key = _body.substr(j, i - j);
                j = i + 1;
                break;
            case '+':
                _body[i] = ' ';
                break;
            case '%':
                if (i + 2 < _body.size()) {
                    int num = char2Hex(_body[i + 1]) * 16 + char2Hex(_body[i + 2]);
                    _body[i] = static_cast<char>(num);
                    i += 2;
                }
                break;
            case '&':
                value = _body.substr(j, i - j);
                _post[key] = value;
                j = i + 1;
                LOG_INFO("%s = %s", key.c_str(), value.c_str());
                break;
            default:
                break;
        }
        ++i;
    }
    if (j < i) {
        value = _body.substr(j, i - j);
        _post[key] = value;
        LOG_INFO("Parsed Key-Value: %s = %s", key.c_str(), value.c_str());
    }
}
int HttpRequest::char2Hex(char ch) {
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return ch;
}
bool HttpRequest::userVerify(const std::string& name, const std::string& pwd, bool isLogin) {
    if (name.empty() || pwd.empty()) {
        LOG_ERROR("Username or password is empty.");
        return false;
    }
    MYSQL* sql = mysql_init(nullptr);
    if (!sql) {
        LOG_ERROR("mysql_init failed.");
        return false;
    }
    MYSQL* conn = mysql_real_connect(sql, "localhost", "cpp", "88888888", "yourdb", 0, nullptr, 0);
    if (!conn) {
        LOG_ERROR("mysql_real_connect failed: %s", mysql_error(sql));
        mysql_close(sql);
        return false;
    }

    std::string query = "SELECT password FROM user WHERE username='" + name + "' LIMIT 1";
    if (mysql_query(conn, query.c_str())) {
        LOG_ERROR("mysql_query failed: %s", mysql_error(conn));
        mysql_close(conn);
        return false;
    }
    MYSQL_RES* res = mysql_store_result(conn);
    if (!res) {
        LOG_ERROR("mysql_store_result failed: %s", mysql_error(conn));
        mysql_close(conn);
        return false;
    }
    bool flag = !isLogin;
    MYSQL_ROW row = mysql_fetch_row(res);
    if (row) {
        LOG_INFO("Database row: %s", row[0]);
        flag = isLogin ? (pwd == row[0]) : flag;
    }
    mysql_free_result(res);
    if (!isLogin && flag) {
        query = "INSERT INTO user(username, password) VALUES('" + name + "','" + pwd + "')";
        if (mysql_query(conn, query.c_str())) {
            LOG_ERROR("Insert user failed: %s", mysql_error(conn));
            flag = false;
        } else {
            LOG_INFO("Insert user succeeded: username = %s, password = %s", name.c_str(), pwd.c_str());
        }
    }
    mysql_close(conn);
    return flag;
}
