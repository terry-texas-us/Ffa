#include "stdafx.h"

#include "PegAEsys.h"
#include "PegAEsysDoc.h"

CQuadTree::CQuadTree()
{
	root = new SQuadTreeNode;
}

void CQuadTree::Unload()
{
	root->Unload();
}

void CQuadTree::CalculateTree()
{
	CPegDoc *pDoc = CPegDoc::GetDoc();

	double xMin = 999999;
	double xMax = -999999;
	double yMin = 999999;
	double yMax = -999999;

	for(int a = 0; a < pDoc->LayersGetSize(); a++)
	{
		CLayer *pLayer = pDoc->LayersGetAt(a);

		if((pLayer->IsHot()) || (pLayer->IsWarm()))
		{

			POSITION pos1 = pLayer->GetHeadPosition();
			while(pos1 != 0)
			{
				CSeg *pSeg = pLayer->GetNext(pos1);

				root->pSegs->AddTail(pSeg);

				if(!pSeg->IsEmpty())
				{
					CPrim *pPrim = pSeg->GetAt(pSeg->GetHeadPosition());

					// polygon is crashing with getctrlpt
					if((!pPrim->Is(CPrim::PRIM_POLYGON)) && (!pPrim->Is(CPrim::PRIM_POLYLINE)))
					{
						CPnt pt = pPrim->GetCtrlPt();

						if(pt[0] > xMax)
							xMax = pt[0];
						if(pt[0] < xMin)
							xMin = pt[0];
						if(pt[1] > yMax)
							yMax = pt[1];
						if(pt[1] < yMin)
							yMin = pt[1];
					}
				}
			}
		}
	}

	RECT temp;
	temp.left	= (LONG)xMin;
	temp.right	= (LONG)xMax;
	temp.bottom = (LONG)yMin;
	temp.top	= (LONG)yMax;

	root->rect = temp;
}

// ------------------------------------------
// |					|					|				
// |		1			|		2			|
// |					|					|
// |					|					|
// ------------------------------------------
// |					|					|				
// |					|					|
// |		3			|		4			|
// |					|					|
// ------------------------------------------

void CQuadTree::PopulateTree()
{
	// 15 level max or 5 segment maximum
	root->PopulateTree(15, 5, 0);
}

int CQuadTree::RemoveDuplicates()
{
	int count = 0;

	root->RemoveDuplicates(count);

	return count;
}

void SQuadTreeNode::PopulateTree(int MaxLevels, int MaxSegs, int CurLevel)
{
	if((pSegs->GetSize() <= MaxSegs) || ((CurLevel + 1) >= MaxLevels))
		return;

	CSegs *pSegs0 = new CSegs;
	CSegs *pSegs1 = new CSegs;
	CSegs *pSegs2 = new CSegs;
	CSegs *pSegs3 = new CSegs;

	RECT rect0;
	RECT rect1;
	RECT rect2;
	RECT rect3;

	double xMid = (rect.right - rect.left) / 2;
	double yMid = (rect.top - rect.bottom) / 2;

	POSITION pos = pSegs->GetHeadPosition();
	while(pos != 0)
	{
		CSeg *pSeg = pSegs->GetNext(pos);

		if(!pSeg->IsEmpty())
		{
			CPrim *pPrim = pSeg->GetAt(pSeg->GetHeadPosition());

			if(!pPrim->Is(CPrim::PRIM_POLYGON))
			{
				CPnt pt = pPrim->GetCtrlPt();

				// 0
				if((pt[0] < xMid) && (pt[1] >= yMid))
				{
					pSegs0->AddTail(pSeg);
					pSegs->Remove(pSeg);
				}

				// 1
				else if((pt[0] >= xMid) && (pt[1] >= yMid))
				{
					pSegs1->AddTail(pSeg);
					pSegs->Remove(pSeg);
				}

				// 2
				else if((pt[0] < xMid) && (pt[1] < yMid))
				{
					pSegs2->AddTail(pSeg);
					pSegs->Remove(pSeg);
				}

				// 3
				else if((pt[0] >= xMid) && (pt[1] < yMid))
				{
					pSegs3->AddTail(pSeg);
					pSegs->Remove(pSeg);
				}

			} // not polygon
		}// not empty
	}// end of while loop

	rect0.bottom	= (LONG)yMid;
	rect0.top		= rect.top;
	rect0.left		= rect.left;
	rect0.right		= (LONG)xMid;

	rect1.bottom	= (LONG)yMid;
	rect1.top		= rect.top;
	rect1.left		= (LONG)xMid;
	rect1.right		= rect.right;

	rect2.bottom	= rect.bottom;
	rect2.top		= (LONG)yMid;
	rect2.left		= rect.left;
	rect2.right		= (LONG)xMid;

	rect3.bottom	= rect.bottom;
	rect3.top		= (LONG)yMid;
	rect3.left		= (LONG)xMid;
	rect3.right		= rect.right;

	// 0
	children[0] = new SQuadTreeNode;
	children[0]->pSegs = pSegs0;
	children[0]->rect = rect0;
	if((children[0]->pSegs->GetSize() > MaxSegs) && (++CurLevel < MaxLevels))
		children[0]->PopulateTree(MaxLevels, MaxSegs, CurLevel);	

	// 1
	children[1] = new SQuadTreeNode;
	children[1]->pSegs = pSegs1;
	children[1]->rect = rect1;
	if((children[1]->pSegs->GetSize() > MaxSegs) && (++CurLevel < MaxLevels))
		children[1]->PopulateTree(MaxLevels, MaxSegs, CurLevel);

	// 2
	children[2] = new SQuadTreeNode;
	children[2]->pSegs = pSegs2;
	children[2]->rect = rect2;
	if((children[2]->pSegs->GetSize() > MaxSegs) && (++CurLevel < MaxLevels))
		children[2]->PopulateTree(MaxLevels, MaxSegs, CurLevel);

	// 3
	children[3] = new SQuadTreeNode;
	children[3]->pSegs = pSegs3;
	children[3]->rect = rect3;
	if((children[3]->pSegs->GetSize() > MaxSegs) && (++CurLevel < MaxLevels))
		children[3]->PopulateTree(MaxLevels, MaxSegs, CurLevel);
}

void SQuadTreeNode::RemoveDuplicates(int &count)
{
	if(HasChildren())
	{
		for(int i = 0; i < 4; i++)
			children[i]->RemoveDuplicates(count);
	}
	else
	{
		POSITION posF = pSegs->GetHeadPosition();
		while(posF != 0)
		{
			CSeg *pSeg = pSegs->GetNext(posF);

			POSITION posX = posF;
			while(posX != 0)
			{
				CSeg *pSegF = pSegs->GetNext(posX);

				POSITION pos1 = pSeg->GetHeadPosition();
				POSITION pos2 = pSegF->GetHeadPosition();
			
				CPrim *p1 = pSeg->GetAt(pos1);
				CPrim *p2 = pSegF->GetAt(pos2);

				CString str1, str2;
				p1->FormatGeometry(str1);
				p2->FormatGeometry(str2);

				if(str1 == str2)
				{
					CPegDoc::GetDoc()->AnyLayerRemove(pSegF);
					count++;
				}

			}// look for copy of segment

		}// get next segment

	}// duplicate segments removed
}

void SQuadTreeNode::Unload()
{
	if(HasChildren())
	{
		for(int i = 0; i < 4; i++)
			children[i]->Unload();
	}

	delete pSegs;
	delete this;
}