# include <iostream>
# include <assert.h>
# include <sys/mman.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <string.h>
# include <fcntl.h>
# include <stdio.h>

#define MAX_LOAD_SIZE 209715200  //200M
	
class CMMapFile
{
public:

	CMMapFile(){
		m_szFileName[0] = 0;
	 	m_nLength = 0;
		m_nLeftLength = 0;
	   	m_pBase = NULL;
	    	m_fd = -1;
	}
	virtual ~CMMapFile(){
		close();
	}
	
	bool open(const char* pszName)
	{
		strcpy(m_szFileName,pszName);
		m_fd = ::open(pszName, O_RDONLY, 0644);
	
		if (m_fd == -1) {
			printf("打开文件出错 %s 文件可能不存在\n", pszName);
			return false;
		}
		struct stat statInfo;
		if (fstat(m_fd, &statInfo ) < 0) {
			printf("文件 %s fstat失败!\n", pszName);
	   		::close(m_fd);
	   		m_fd = -1;
			return false;
		}
		m_nLength = 0;
		m_nLeftLength = statInfo.st_size;		//文件长度

		int nLength = 0;
		
		if (m_nLeftLength <= MAX_LOAD_SIZE)
		{
			nLength = m_nLeftLength;
		}
		else 
		{
			nLength = MAX_LOAD_SIZE;
		}
		
		m_pBase = (char*)mmap(0, nLength, PROT_READ, MAP_SHARED, m_fd, 0);

		if (m_pBase == MAP_FAILED) {
			printf("文件 %s 映射内存文件出错! \n", pszName);
			::close(m_fd);
			m_fd = -1;
			m_pBase = NULL;
			return false;
		}

		m_nLength = nLength;
		m_nLeftLength -= m_nLength;		
		
		madvise(m_pBase, m_nLength, MADV_SEQUENTIAL);	//Expect  page  references in sequential order.
		
		
		if(::close(m_fd) != 0){
			printf("CMMapFile: close file fail: %s",pszName );
		}
		m_fd = -1;
		return true;
	}
	bool reopen(int nOffset, int nLength){
		if(m_fd == -1)
			m_fd = ::open(m_szFileName, O_RDONLY, 0644);
		if (m_fd == -1) {
			printf("打开文件出错 %s 文件可能不存在 %s\n", m_szFileName);
			return false;
		}
		if (m_pBase!=NULL)
		{
			if(munmap(m_pBase, m_nLength) != 0)
			{
				printf("CMMapFile::reopen munmap fail: %s", m_szFileName);
				return false;
			}
			m_pBase = NULL;
			m_nLength = 0;
		}
		
	    if (nLength > m_nLeftLength)
		{
			nLength = m_nLeftLength;
		}
			
	 	m_pBase = (char*)mmap(0, nLength, PROT_READ, MAP_SHARED, m_fd, nOffset);
			
	 	if (m_pBase == MAP_FAILED)
	 	{
			printf("CMMapFile::reopen 文件 %s 映射内存文件出错! \n", m_szFileName);
			::close(m_fd);
			m_fd = -1;
			m_pBase = NULL;
			return false;
	  	}
		m_nLength = nLength;
	 	m_nLeftLength -= nLength;
		
	 	madvise(m_pBase, m_nLength, MADV_SEQUENTIAL); 
	 	if(::close(m_fd) != 0){
			printf("CMMapFile: close file fail: %s", m_szFileName);
	 	}
	    m_fd = -1;

		return true;
	}
	bool close(){
		bool bRes = true;
		if (m_pBase!=NULL) {
			if(munmap(m_pBase, m_nLength) != 0){
				printf("CMMapFile: munmap fail: %s", m_szFileName);
				bRes = false;
			}
			if(m_fd != -1 && ::close(m_fd) != 0){
				
		  		printf("CMMapFile: close file fail: %s", m_szFileName);
				bRes = false;
			}
			m_pBase = NULL;
			m_nLength = 0;
			m_nLeftLength = 0;
			m_fd = -1;
		}
		
		return bRes;
	}
	inline char *offset2Addr(int nOffset) const
	{
		return (m_pBase + nOffset);
	}
  	int  getSize()
	{
		return m_nLength;
	}
	int getLeftSize()
	{
		return m_nLeftLength;
	}

private:
	char m_szFileName[100];
	int m_nLength;				//文件大小
	char* m_pBase;					//数据基指针
	int m_fd;		//文件描述符
	int m_nLeftLength;
	
};


class CMMapLoader
{
public:
	
	CMMapLoader(void){
		m_file = NULL;
		m_pCurrent = NULL;
		m_nLeftLen = 0;
		m_nFullLen = 0;
		m_nOffset = 0;
	}

	bool load(const char* szFileName){
		m_file = new CMMapFile();
		if (!(m_file->open(szFileName))) 
		{
			printf("打开文件 %s 出错,文件名不正确或不存在.\n", szFileName);
			return false;
		}	
		m_pCurrent = m_file->offset2Addr(0);
		m_nLeftLen = m_file->getLeftSize();
		m_nFullLen = m_file->getSize();
		m_nOffset = 0;
		m_nRedFileLen = 0;
		return true;
	}
	
	int next(){

		while (m_nFullLen > 0)
		{
			m_nRedFileLen += m_nFullLen;
			char ch;
			for (int i=0;i< m_nFullLen;i++) ch = *(m_pCurrent + i);
			m_nFullLen = 0;
			m_pCurrent = NULL;
			if (m_nLeftLen > 0)
			{	
				m_nOffset += MAX_LOAD_SIZE;
				if(!m_file->reopen(m_nOffset,MAX_LOAD_SIZE))
				{
					printf("CMMapLoader::next reload failed, get document over.");
				}
				else
				{
				m_pCurrent = m_file->offset2Addr(0);
					m_nLeftLen = m_file->getLeftSize();
					m_nFullLen = m_file->getSize();
				}
				
			}
			
		}
		return m_nRedFileLen;
			
	}


	/* 关闭打开的xml文件 */
	bool unload(){
		if (m_file != NULL)
		{
			m_file->close();
			delete m_file;
		}
		return true;
	}
	

private:
	/* 文件句柄 */
	CMMapFile* m_file;
	char* m_pCurrent;
	int m_nLeftLen;
	int m_nFullLen;
	int m_nOffset;
	int m_nRedFileLen;
};

int main(int argc, char* argv[]){
	
	CMMapLoader *fileloader;
	fileloader = new CMMapLoader();
	if (argv[1]!= NULL)
	{
		fileloader->load(argv[1]);
		printf ("File %s Len: %d\n",argv[1],fileloader->next());
		fileloader->unload();
	}
	else
	{
		printf ("please input file route!\n");
	}
	return 0;	
	
}

