#include "HttpResponse.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include "Buffer.h"
#include "Logger.h"
#include "util.h"

const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE{
    {".html", "text/html"},          {".xml", "text/xml"},        {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},          {".rtf", "application/rtf"}, {".pdf", "application/pdf"},
    {".word", "application/nsword"}, {".png", "image/png"},       {".gif", "image/gif"},
    {".jpg", "image/jpeg"},          {".jpeg", "image/jpeg"},     {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},         {".mpg", "video/mpeg"},      {".mp4", "video/mp4"},
    {".mkv", "video/x-matroska"},    {".avi", "video/x-msvideo"}, {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},   {".css", "text/css"},        {".js", "text/javascript"},
    {".ico", "image/x-icon"}};
const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS{
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
};
const std::unordered_map<int, std::string> HttpResponse::CODE_PATH{
    {400, "/400.html"},
    {403, "/403.html"},
    {404, "/404.html"},
};
HttpResponse::HttpResponse(const std::string& srcDir, std::string& path, bool isKeepAlive, int code)
    : _code(code), _isKeepAlive(isKeepAlive), _path(path), _srcDir(srcDir), _mmFile(nullptr) {
    // assert(!srcDir.empty());
    memset(&_mmFileStat, 0, sizeof(_mmFileStat));
}

HttpResponse::~HttpResponse() {
    if (_mmFile) {
        munmap(_mmFile, _mmFileStat.st_size);
        _mmFile = nullptr;
    }
}

void HttpResponse::makeResponse(Buffer& buff) {
    LOG_INFO("resource path: %s", (_srcDir + _path).c_str());
    if (stat((_srcDir + _path).c_str(), &_mmFileStat) < 0 || S_ISDIR(_mmFileStat.st_mode)) {
        _code = 404;
    } else if (!(_mmFileStat.st_mode & S_IROTH)) {
        _code = 403;
    } else if (_code == -1) {
        _code = 200;
    }
    _errorHtml();
    _addStateLine(buff);
    _addHeader(buff);
    _addContent(buff);
}

/*根据状态码查找对应的错误页面路径，并设置为当前响应的内容路径*/
void HttpResponse::_errorHtml() {
    if (CODE_PATH.count(_code) == 1) {
        _path = CODE_PATH.at(_code);
        if (stat((_srcDir + _path).data(), &_mmFileStat) < 0) {
            _code = 404;
        }
    }
}

/* 添加HTTP响应的状态行，包含HTTP版本、状态码及其对应的状态描述。 */
void HttpResponse::_addStateLine(Buffer& buff) {
    std::string status;
    if (CODE_STATUS.count(_code) == 1) {
        status = CODE_STATUS.at(_code);
    } else {
        _code = 400;
        status = CODE_STATUS.at(_code);
    }
    std::string responseLine = "HTTP/1.1 " + std::to_string(_code) + " " + status + "\r\n";
    LOG_INFO("%s", responseLine.c_str());
    buff.append(responseLine.c_str(), responseLine.size());
}

/* 添加HTTP响应的头部信息，主要是连接方式和内容类型。 */
void HttpResponse::_addHeader(Buffer& buff) {
    buff.append("Connection: ", strlen("Connection: "));
    if (_isKeepAlive) {
        buff.append("keep-alive\r\n", strlen("keep-alive\r\n"));
        buff.append("keep-alive: max=6, timeout=120\r\n", strlen("keep-alive: max=6, timeout=120\r\n"));
    } else {
        buff.append("close\r\n", strlen("close\r\n"));
    }
    std::string contentType = "Content-type: " + _getFileType() + "\r\n";
    buff.append(contentType.c_str(), contentType.size());
}

/* 根据请求文件的后缀名确定文件的MIME类型 */
std::string HttpResponse::_getFileType() const {
    std::string::size_type idx = _path.find_last_of('.');
    if (idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = _path.substr(idx);
    if (SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}

/* 添加HTTP响应的内容部分，包括文件的内容和内容长度。 */
void HttpResponse::_addContent(Buffer& buff) {
    int srcFd = open((_srcDir + _path).data(), O_RDONLY);
    if (srcFd < 0) {
        errorContent(buff, "File Not Found!");
        return;
    }

    _mmFileStat.st_size = lseek(srcFd, 0, SEEK_END);
    lseek(srcFd, 0, SEEK_SET);
    char* mmFile = static_cast<char*>(mmap(0, _mmFileStat.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0));
    if (mmFile == MAP_FAILED) {
        errorContent(buff, "File Not Found!");
        close(srcFd);
        return;
    }
    LOG_INFO("mmap success");
    _mmFile = mmFile;
    close(srcFd);

    std::string contentLength = "Content-length: " + std::to_string(_mmFileStat.st_size) + "\r\n\r\n";
    buff.append(contentLength.c_str(), contentLength.size());
    LOG_INFO("%zu", _mmFileStat.st_size);
    buff.append(mmFile, _mmFileStat.st_size);
    LOG_INFO("%s", mmFile);
    // if (m_path.find("mp4") != std::string::npos || m_path.find("avi") != std::string::npos) {
    //   // 打开或创建一个本地文件以写入视频数据
    //   FILE* out_file = fopen("output_video.mp4", "wb");
    //   fwrite(mmFile, 1, m_mmFileStat.st_size, out_file);
    //   fclose(out_file);
    // }
}

/* 构建错误内容的响应 */
void HttpResponse::errorContent(Buffer& buff, const char* errorMessage) {
    std::string errorResponse =
        "HTTP/1.1 404 Not Found\r\nContent-length: " + std::to_string(strlen(errorMessage)) + "\r\n\r\n" + errorMessage;
    buff.append(errorResponse.c_str(), errorResponse.size());
}
