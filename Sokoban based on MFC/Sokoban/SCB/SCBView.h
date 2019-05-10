
// SCBView.h : CSCBView Ŭ������ �������̽�
//

#pragma once


class CSCBView : public CView
{
protected: // serialization������ ��������ϴ�.
	CSCBView();
	DECLARE_DYNCREATE(CSCBView)

// Ư���Դϴ�.
public:
	CSCBDoc* GetDocument() const;

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CSCBView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateDummyStatus(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDummyStatus2(CCmdUI *pCmdUI);
	afx_msg void OnRestart();
	afx_msg void OnNext();
	afx_msg void OnPrev();
};

#ifndef _DEBUG  // SCBView.cpp�� ����� ����
inline CSCBDoc* CSCBView::GetDocument() const
   { return reinterpret_cast<CSCBDoc*>(m_pDocument); }
#endif

