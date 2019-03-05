#pragma once

class CExportState
{
	public:
		bool bVectorizeText;
		double dXShift;
		double dYShift;
		double dWidthFactor;

		CExportState()
		{
			bVectorizeText = false;
			dXShift = 0;//-.5;
			dYShift = 0;
			dWidthFactor = .94;
		}
};