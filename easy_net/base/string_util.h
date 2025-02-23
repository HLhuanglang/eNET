#ifndef __EASYNET_STRING_UTIL_H
#define __EASYNET_STRING_UTIL_H

#include <string>
#include <vector>

namespace EasyNet {
void split_string(const std::string& s, std::vector<std::string>& v, const std::string& c) {
    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (std::string::npos != pos2) {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}
}  // namespace EasyNet
#endif  // !__EASYNET_STRING_UTIL_H
