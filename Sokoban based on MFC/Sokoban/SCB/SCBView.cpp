
// SCBView.cpp : CSCBView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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
unsigned char table[18][21];      //���� �������� ������
void print_position();
BOOL check_complete();						//�������� �ǵ�
static void move_attr_up(unsigned x, unsigned y, int attr);
static void move_attr_down(unsigned x, unsigned y, int attr);
static void move_attr_left(unsigned x, unsigned y, int attr);
static void move_attr_right(unsigned x, unsigned y, int attr);
int move_up(int mode);
int move_down(int mode);
int move_left(int mode);
int move_right(int mode);
HBITMAP hBit[8];    // �� ��Ʈ���� �ڵ��� ����

void redo();
void undo();


struct MoveInfo
{
	char dx : 3;
	char dy : 3;
	char bWithPack : 2;			//����� ���� ��������
};

unsigned char g_table[MAXSTAGE][18][21] =   //���� �������� ������
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
	// ǥ�� �μ� ����Դϴ�.
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

// CSCBView ����/�Ҹ�

CSCBView::CSCBView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CSCBView::~CSCBView()
{
}

BOOL CSCBView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	SetStage();

	return CView::PreCreateWindow(cs);
}

// CSCBView �׸���

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

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CSCBView �μ�

BOOL CSCBView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CSCBView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CSCBView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CSCBView ����

#ifdef _DEBUG
void CSCBView::AssertValid() const
{
	CView::AssertValid();
}

void CSCBView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSCBDoc* CSCBView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSCBDoc)));
	return (CSCBDoc*)m_pDocument;
}
#endif //_DEBUG


// CSCBView �޽��� ó����
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	int dx = 0;
	int dy = 0;
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰���/�Ǵ� �⺻���� ȣ���մϴ�.
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

	if (table[curr_y + dy][curr_x + dx] != '#')   //�������� ���� �ƴϰ�
	{
		if (table[curr_y + dy][curr_x + dx] == 'O' || table[curr_y + dy][curr_x + dx] == '!')		//�������� ���� ������
		{
			if (table[curr_y + dy * 2][curr_x + dx * 2] == ' ' || table[curr_y + dy * 2][curr_x + dx * 2] == '.')	//�� ����ĭ�� ������̰ų� �������� ��
			{
				
				if (table[curr_y + dy * 2][curr_x + dx * 2] == '.')	//1ĭ�� ���ε� 2ĭ�� �������̸�
				{
					table[curr_y + dy * 2][curr_x + dx * 2] = '!';	//2ĭ�� �ϼ����� ����
				}
				else if (table[curr_y + dy * 2][curr_x + dx * 2] == ' ')
				{
					table[curr_y + dy * 2][curr_x + dx * 2] = 'O';	//2��° ĭ�� �ڽ��� �����Ѵ�.
				}
			}
			else
			{
				return;			//2��° ĭ�� ���̰ų� ���� ���� if�� Ż�� 
			}
		}
		if (g_table[curr_stage][curr_y][curr_x] == '.')		//���� ĭ�� ���������
		{
			table[curr_y][curr_x] = '.';					//�������� ����
		}
		else
		{
			table[curr_y][curr_x] = ' ';					//�� �ܴ� ��ĭ���� ����
		}
		table[curr_y + dy][curr_x + dx] = '@';
		curr_x += dx;			//���� x��ǥ ����
		curr_y += dy;			//���� y��ǥ ����
		curr_record++;			//���� ���� �߰�
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
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
}


void CSCBView::OnUpdateDummyStatus2(CCmdUI *pCmdUI)
{
	// TODO: ���⿡ ��� ������Ʈ UI ó���� �ڵ带 �߰��մϴ�.
}


void CSCBView::OnRestart()
{
	SetStage();
	this->Invalidate(true);
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
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
		MessageBox(L"������ �����Դϴ�.", L"MAXerror");
	}
	this->Invalidate(true);
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void CSCBView::OnPrev()
{
	if (curr_stage != 0)
	{
		curr_stage--;
		SetStage();
	}
	this->Invalidate(true);
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}
