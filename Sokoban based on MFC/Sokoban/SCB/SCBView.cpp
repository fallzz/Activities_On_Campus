
// SCBView.cpp : CSCBView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "SCB.h"
#endif

#include "SCBDoc.h"
#include "SCBView.h"
#include "resource.h"
#include "afxcmn.h"
#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAXSTAGE 5

unsigned  static curr_x, curr_y, curr_record, start_pos, curr_stage, max_record, best_record;
char     player[9], record_player[9];
unsigned char *record;
unsigned char table[18][21];      //현재 스테이지 데이터
void print_position();
BOOL check_complete();						//성공여부 판독
static void move_attr_up(unsigned x, unsigned y, int attr);
static void move_attr_down(unsigned x, unsigned y, int attr);
static void move_attr_left(unsigned x, unsigned y, int attr);
static void move_attr_right(unsigned x, unsigned y, int attr);
int move_up(int mode);
int move_down(int mode);
int move_left(int mode);
int move_right(int mode);
HBITMAP hBit[8];    // 각 비트맵의 핸들을 저장

void redo();
void undo();


struct MoveInfo
{
	char dx : 3;
	char dy : 3;
	char bWithPack : 2;			//사람과 같이 움직였나
};

unsigned char g_table[MAXSTAGE][18][21] =   //기존 스테이지 데이터
{ { "####################",
"####################",
"####################",
"#####   ############",
"#####O  ############",
"#####  O############",
"###  O O ###########",
"### # ## ###########",
"#   # ## #####  ..##",
"# O  O   @      ..##",
"##### ### # ##  ..##",
"#####     ##########",
"####################",
"####################",
"####################",
"####################",
"####################",
"####################"
	}, {
		"####################",
		"####################",
		"####################",
		"####################",
		"####..  #     ######",
		"####..  # O  O  ####",
		"####..  #O####  ####",
		"####..    @ ##  ####",
		"####..  # #  O #####",
		"######### ##O O ####",
		"###### O  O O O ####",
		"######    #     ####",
		"####################",
		"####################",
		"####################",
		"####################",
		"####################",
		"####################"
	},
	{ "####################",
	"####################",
	"##    #######  #####",
	"##   O#  #  O  #   #",
	"##  O  O  O # O O  #",
	"###O O   # @# O    #",
	"##  O ########### ##",
	"## #   #.......# O##",
	"## ##  # ......#  ##",
	"## #   O....... O ##",
	"## # O #.... ..#  ##",
	"##  O O####O#### O##",
	"## O   ### O   O  ##",
	"## O     O O  O    #",
	"### ###### O ##### #",
	"##         #       #",
	"####################",
	"####################"
	},
	{
		"####################",
		"####################",
		"######  #  #########",
		"###### O#O #  ######",
		"##.. #  #  #   #####",
		"##.. # O#O #  O#####",
		"##.  #     #O  #  ##",
		"##..   O#  # O    ##",
		"##..@#  #O #O  #  ##",
		"##.. # O#     O#  ##",
		"##.. #  #OO#O  #  ##",
		"##.. # O#  #  O#O  #",
		"##.. #  #  #   #   #",
		"###. ####  #####   #",
		"####################",
		"####################",
		"####################",
		"####################"
	},
	{
		"####################",
		"####################",
		"##  ################",
		"##  #   ..#......###",
		"##  # # ##### ...###",
		"###O#    ........###",
		"##   ##O######  ####",
		"## O #     ######@ #",
		"###O # O   ######  #",
		"##  O #OOO##       #",
		"##      #    #O#O###",
		"## #### #OOOOO    ##",
		"## #    O     #   ##",
		"## #   ## ##     ###",
		"## ######O###### O #",
		"##        ######   #",
		"####################",
		"####################"
	}
};


void SetStage()
{
	int ch;
	memcpy(table, g_table[curr_stage], sizeof(table));
	for (int stagex = 0; stagex < 21; stagex++)
	{
		for (int stagey = 0; stagey < 18; stagey++)
		{
			if (g_table[curr_stage][stagey][stagex] == '@')
			{
				curr_x = stagex;
				curr_y = stagey;
			}
		}
	}
}

// CSCBView

IMPLEMENT_DYNCREATE(CSCBView, CView)

BEGIN_MESSAGE_MAP(CSCBView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_UPDATE_COMMAND_UI(ID_DUMMY_STATUS, &CSCBView::OnUpdateDummyStatus)
	ON_UPDATE_COMMAND_UI(ID_DUMMY_STATUS2, &CSCBView::OnUpdateDummyStatus2)
	ON_COMMAND(ID_Restart, &CSCBView::OnRestart)
	ON_COMMAND(ID_Next, &CSCBView::OnNext)
	ON_COMMAND(ID_Prev, &CSCBView::OnPrev)
END_MESSAGE_MAP()

// CSCBView 생성/소멸

CSCBView::CSCBView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CSCBView::~CSCBView()
{
}

BOOL CSCBView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	SetStage();

	return CView::PreCreateWindow(cs);
}

// CSCBView 그리기

void CSCBView::OnDraw(CDC* pDC)
{
	/*CBitmap imgBmp;
	CImageList* list;
	imgBmp.LoadBitmap(IDB_BITMAP1);
	list->Create(50, 50, ILC_COLOR8, 13, 13);
	list->Add(&imgBmp);
	*/
	CSCBDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	BITMAP bmpInfo;
	CDC MemDC;
	MemDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	CBitmap *pOldbmp;
	bmp.LoadBitmap(IDB_BITMAP1);
	bmp.GetBitmap(&bmpInfo);
	pOldbmp = MemDC.SelectObject(&bmp);
	char temp;
	for (int i = 0; i < 21; i++)
	{
		for (int j = 0; j < 18; j++)
		{
			temp = table[j][i];
			switch (temp)
			{
			case ' ':
				pDC->BitBlt(100 + i * 50, j * 50, 50, 50, &MemDC, 0, 0, SRCCOPY);
				break;
			case 'O':
				pDC->BitBlt(100 + i * 50, j * 50, 50, 50, &MemDC, 50, 0, SRCCOPY);
				break;
			case '.':
				pDC->BitBlt(100 + i * 50, j * 50, 50, 50, &MemDC, 150, 0, SRCCOPY);
				break;
			case '!':
				pDC->BitBlt(100 + i * 50, j * 50, 50, 50, &MemDC, 250, 0, SRCCOPY);
				break;
			case '@':
				pDC->BitBlt(100 + i * 50, j * 50, 50, 50, &MemDC, 350, 0, SRCCOPY);
				break;
			case '#':
				pDC->BitBlt(100 + i * 50, j * 50, 50, 50, &MemDC, 450, 0, SRCCOPY);
				break;
			}


		}
	}

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CSCBView 인쇄

BOOL CSCBView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CSCBView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CSCBView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CSCBView 진단

#ifdef _DEBUG
void CSCBView::AssertValid() const
{
	CView::AssertValid();
}

void CSCBView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSCBDoc* CSCBView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSCBDoc)));
	return (CSCBDoc*)m_pDocument;
}
#endif //_DEBUG


// CSCBView 메시지 처리기
BOOL check_complete()
{
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 21; j++)
		{
			if ((g_table[curr_stage][i][j] == '.') != (table[i][j] == '!'))
				return FALSE;
		}
	}
	return TRUE;
}

void CSCBView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int dx = 0;
	int dy = 0;
	// TODO: 여기에 메시지 처리기 코드를 추가및/또는 기본값을 호출합니다.
	switch (nChar)
	{
	case VK_LEFT:
		dx = -1;
		break;
	case VK_RIGHT:
		dx = 1;
		break;
	case VK_UP:
		dy = -1;
		break;
	case VK_DOWN:
		dy = 1;
		break;
	}

	if (table[curr_y + dy][curr_x + dx] != '#')   //목적지가 벽이 아니고
	{
		if (table[curr_y + dy][curr_x + dx] == 'O' || table[curr_y + dy][curr_x + dx] == '!')		//목적지에 짐이 있으며
		{
			if (table[curr_y + dy * 2][curr_x + dx * 2] == ' ' || table[curr_y + dy * 2][curr_x + dx * 2] == '.')	//짐 다음칸이 빈공간이거나 도착지일 때
			{
				
				if (table[curr_y + dy * 2][curr_x + dx * 2] == '.')	//1칸이 짐인데 2칸이 목적지이면
				{
					table[curr_y + dy * 2][curr_x + dx * 2] = '!';	//2칸을 완성으로 설정
				}
				else if (table[curr_y + dy * 2][curr_x + dx * 2] == ' ')
				{
					table[curr_y + dy * 2][curr_x + dx * 2] = 'O';	//2번째 칸은 박스를 설정한다.
				}
			}
			else
			{
				return;			//2번째 칸이 짐이거나 벽일 때는 if문 탈출 
			}
		}
		if (g_table[curr_stage][curr_y][curr_x] == '.')		//원래 칸이 목적지라면
		{
			table[curr_y][curr_x] = '.';					//목적지로 변경
		}
		else
		{
			table[curr_y][curr_x] = ' ';					//그 외는 빈칸으로 설정
		}
		table[curr_y + dy][curr_x + dx] = '@';
		curr_x += dx;			//현재 x좌표 갱신
		curr_y += dy;			//현재 y좌표 갱신
		curr_record++;			//무브 갯수 추가
	}
	if (check_complete())
	{
		if (curr_stage < MAXSTAGE - 1)
		{
			//CDC* pDC;
			//CString clrmsg = _T("%d Stage clear!! Let's go to the next stage.", curr_stage + 1);
			curr_record = 0;
			curr_stage++;
			SetStage();
		}
		else
		{
			CDC* pDC;
			CString allclrmsg = _T("All Stage Clear!!");
			pDC = GetDC();
			pDC->TextOut(500, 400, allclrmsg);
		}
	}
	this->Invalidate(true);
	CString strStage, strMove;
	strStage.Format(L"Stage : %d", curr_stage+1);
	strMove.Format(L"Move : %d", curr_record);
	CMainFrame* pFrame;
	pFrame = (CMainFrame*)AfxGetMainWnd();
	CStatusBar* pStatus = &(pFrame->m_wndStatusBar);
	pStatus->SetPaneText(0, strStage);
	pStatus->SetPaneText(1, strMove);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CSCBView::OnUpdateDummyStatus(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CSCBView::OnUpdateDummyStatus2(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CSCBView::OnRestart()
{
	SetStage();
	this->Invalidate(true);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSCBView::OnNext()
{
	if (curr_stage < MAXSTAGE - 1)
	{
		curr_stage++;
		SetStage();
	}
	else
	{
		MessageBox(L"마지막 레벨입니다.", L"MAXerror");
	}
	this->Invalidate(true);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CSCBView::OnPrev()
{
	if (curr_stage != 0)
	{
		curr_stage--;
		SetStage();
	}
	this->Invalidate(true);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}
