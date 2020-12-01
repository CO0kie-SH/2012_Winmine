// 2012_Winmine.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>


int g1005338_高度 = 9;
int g1005334_宽度 = 14;
int g1005A20_源句柄 = 0x1005A20;

int g1005360_矩阵 = 0x1005360;

/* 初始化矩阵（） */
void 初始化矩阵_(HDC 参_DC_句柄) {
	int 局部4_X = 1;
	int 局部8_Y;
	int 局部C_高度;

	int eax = 0; eax++;
	局部8_Y = 55;		//矩阵像素偏移
	局部C_高度 = eax;		//eax=1 用于循环高度

	//取出矩阵基地址
	char* ebx = (char*)g1005360_矩阵;
	while (eax <= g1005338_高度)
	{
		//循环宽度，调用画图
		for (int esi = 1; esi <= g1005334_宽度; esi++)
		{
			//计算源句柄
			eax = 0;
			char al = ebx[esi];
			eax = eax & 0x1F;
			eax = eax * 4 + g1005A20_源句柄;

			BitBlt(参_DC_句柄, 局部4_X, 局部8_Y, 16, 16, (HDC)eax, 0, 0, 0xCC0020);
			局部4_X += 16;
		}

		//矩阵换行
		局部C_高度++;
		eax = 局部C_高度;

		//绘画Y换行
		局部8_Y += 16;

		//指针换2行
		ebx += 32;
	}
	return;
}

//定义游戏数据
constexpr auto InfoSizeNULL = (0xFC-0x14)/4-1;
const LPBYTE G_INFOADDRESS = (LPBYTE)0x10056A0;
const LPBYTE G_GAMEMAP = (LPBYTE)0x1005360;

const char* szGameInfo[] = { "难度","雷数","高度","宽度",
"窗口X","窗口Y","时间","安全块" ,"已点块" };
const char* szGameIndex[] = { "①","②","③","④","⑤","⑥","⑦","⑧","⑨" };

//定义游戏信息结构体
typedef struct _WinMineInfo
{
	DWORD dwDifficulty;
	DWORD dwMines;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwWindowX;
	DWORD dwWindowY;
	DWORD dwNull[InfoSizeNULL];
	DWORD dwTime;
	DWORD dwBlocks;
	DWORD dwBlocksClicked;
}WinMineInfo, * PWinMineInfo;

//定义进程控制类
class MyProcess
{
public:
	MyProcess();
	~MyProcess();

	BOOL ReadInfo();
	BOOL ReadMap(LPBYTE map, SIZE_T size);
	BOOL Read(DWORD add, LPVOID saveAdd, SIZE_T size);
public:
	//游戏信息
	WinMineInfo GameInfo;
private:
	SIZE_T m_InfoSize;
	HANDLE hProcess;
};

MyProcess::MyProcess()
{
	m_InfoSize = sizeof(WinMineInfo);
	ZeroMemory(&GameInfo, m_InfoSize);
	hProcess = OpenProcess(PROCESS_VM_READ, 0, 0x43D8);
}

MyProcess::~MyProcess()
{
	if (hProcess > 0)
		CloseHandle(hProcess);
}

BOOL MyProcess::ReadInfo()
{
	return ReadProcessMemory(hProcess, (LPCVOID)G_INFOADDRESS, &GameInfo, m_InfoSize, 0);
}

BOOL MyProcess::ReadMap(LPBYTE map, SIZE_T size)
{
	return ReadProcessMemory(hProcess, (LPCVOID)G_GAMEMAP, map, size, 0);
}

BOOL MyProcess::Read(DWORD add,LPVOID saveAdd, SIZE_T size)
{
	return ReadProcessMemory(hProcess, (LPCVOID)add, saveAdd, size, 0);
}

MyProcess myPro;

int show()
{
	myPro.ReadInfo();

	LPDWORD lpInfo = (LPDWORD)&myPro.GameInfo;
	for (int i = 0; i < 9; i++)
	{
		printf_s("%s\t%lu\n", szGameInfo[i], i < 6 ? lpInfo[i] : lpInfo[i + InfoSizeNULL]);
	}

	DWORD mapInfo[4] = { 0 };
	myPro.Read(0x1005330, (LPVOID)mapInfo, sizeof(mapInfo));
	BYTE map[1024];
	if (myPro.ReadMap(map, sizeof(map)) == 0)
		return -2;

	//计算宽度，并格式化输出
	mapInfo[3] = mapInfo[1] + 2;
	printf("0x%08X\t%02lu|", 0, 0);
	for (DWORD i = 1; i <= mapInfo[1]; i++)
	{
		printf("%02d|", i);
	}
	printf("\n");

	//循环地图数组
	//定义高度从1起，≤高度，add表示偏移，从0起，每次循环加32
	for (DWORD height = 1, add = 0; height <= mapInfo[2]; add += 0x20)
	{
		printf("0x%08X\t%02lu|", (DWORD)G_GAMEMAP + add, height++);

		//循环宽度，定义宽度从1起，≤宽度
		for (DWORD i = 1; i <= mapInfo[1]; i++)
		{
			BYTE& buf = map[i + add];
			if (buf == 0x8F)
				printf("雷|");
			else if (buf > 0x40 && buf < 0x50)
				printf("%s|", szGameIndex[buf - 0x41]);
			else
				printf("  |");
			//printf("%02X|", buf);
		}
		printf("\n");
	}
	return 0;
}

int main()
{
	std::cout << "Hello World!欢迎使用CO0kie丶的扫雷提示控制台！\n";
	char buff[1024];
	do
	{
		std::cout << "\n输入'show'可查看雷坐标；输入'go'可一键扫雷；'exit'退出程序！\n" << "请输入命令：";
		std::cin >> buff;

		if (strcmp(buff, "show") == 0)
		{
			show();
		}
		else if (strcmp(buff, "go"))
		{

		}
	} while (strcmp(buff, "exit") != 0);
	return 0;
}
