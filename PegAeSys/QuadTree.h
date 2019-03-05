#pragma once

#include "stdafx.h"
#include "PegAEsys.h"

struct SQuadTreeNode
{
	SQuadTreeNode() 
	{
		pSegs = new CSegs;
		
		for(int i = 0; i < 4; i++)
			children[i] = 0;
	}

	bool HasChildren() { return (children[0] != NULL); }
	void Unload();
	void PopulateTree(int MaxLevels, int MaxSegs, int CurLevel);

	void RemoveDuplicates(int &count);

	RECT rect;
	SQuadTreeNode *children[4];
	CSegs *pSegs;
};

class CQuadTree
{
	public:
		CQuadTree();
		
		void Unload();
		void CalculateTree();
		void PopulateTree();

		int RemoveDuplicates();

	private:
		int MaxLevels;
		int MaxSegs;

		SQuadTreeNode *root;
};