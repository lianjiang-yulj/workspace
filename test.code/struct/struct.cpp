#include <iostream>
using namespace std;

namespace basis {

/* 位置索引信息定义 */
#pragma pack(push,2)
struct SOccInfo {
	unsigned short m_nWordPos  ;
	unsigned short m_nFieldbit ;
	unsigned short m_nPower    ;
};
#pragma pack(pop)
/* 基本索引内容, STRING, NUMBER等类型 */
struct SDocInfo {
	unsigned int  m_nSid ;
	unsigned int  m_nFlag ;
};
/* TEXT类型扩展的索引内容 */
struct SExtDocInfo : public SDocInfo {
	unsigned int  m_nNumOccs ;
	unsigned int  m_nFirstOcc ;
	unsigned int  m_nFieldMap ;
};

}
namespace basis_plan {

/* 位置索引信息定义 */
#pragma pack(push,2)
struct SOccInfo {
        unsigned short m_nWordPos_nFieldbit_nPower;//12:3:1
};
#pragma pack(pop)
/* 基本索引内容, STRING, NUMBER等类型 */
struct SDocInfo {
        unsigned int  m_nFlag_nSid ;//1:31
};
/* TEXT类型扩展的索引内容 */
struct SExtDocInfo : public SDocInfo {
        unsigned short m_nNumOccs ;//16
        unsigned char  m_nFirstOcc ;//8
        unsigned char  m_nFieldMap ;//8
};

}
int main()
{
	printf("Design: SExtDocInfo %d\n",sizeof(basis_plan::SExtDocInfo));
	printf("Design: SOccInfo %d\n",sizeof(basis_plan::SOccInfo));
	printf("Design: SDocInfo %d\n",sizeof(basis_plan::SDocInfo));

	printf("Act: SExtDocInfo %d\n",sizeof(basis::SExtDocInfo));
        printf("Act: SOccInfo %d\n",sizeof(basis::SOccInfo));
        printf("Act: SDocInfo %d\n",sizeof(basis::SDocInfo));
}

