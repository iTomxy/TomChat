#include "common.h"

/*--- IP压缩进32位整数 ---*/
quint32 ip_str2dig(const QString &ipstr)
{
    std::string s = ipstr.toStdString();
    int a[4] = {0, 0, 0, 0};
    for(int i=0, j=0, tmp=0; i<s.size(); ++i)
    {
        if(s[i] == '.')
        {
            a[j++] = tmp;
            tmp = 0;
        }
        else
            tmp = tmp * 10 + s[i] - '0';
        if(i + 1 == s.size())
            a[j++] = tmp;
    }
    quint32 res = a[0] << 24 | a[1] << 16 | a[2] << 8 | a[3];
    return res;
}
