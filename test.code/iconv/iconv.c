#include <stdint.h>
#include <errno.h>
#include <iconv.h>
#include <string.h>

int main()
{
    iconv_t iconv_handle;
    if((iconv_handle = iconv_open("utf-16", "gbk")) < 0)
    {
        return -1;
    }
    char str[100];
    strcpy(str,"阿里巴巴在线交易");
    char * src = str;
    size_t inLen = strlen(str);
    char szBufOut[100];
    size_t nBufOutLen = 100;
    memset(szBufOut, 0x0, nBufOutLen);
    char * dest = szBufOut;
    int ret = iconv(iconv_handle,(char**)&src, (size_t*)&inLen, (char **)&dest, (size_t*)&nBufOutLen);
    iconv_close((iconv_t)-1);
    printf("%s\n", __FILE__);

    return 1;
}
