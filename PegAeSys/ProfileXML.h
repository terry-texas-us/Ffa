#pragma once

// there should be 26 of each

void ProfileXMLAnnotate(xml::IrrXMLReader* xml);
void ProfileXMLAppearance(xml::IrrXMLReader* xml);
void ProfileXMLAxonometric(xml::IrrXMLReader* xml);
void ProfileXMLDimension(xml::IrrXMLReader* xml);
void ProfileXMLDraw(xml::IrrXMLReader* xml);
void ProfileXMLDraw2(xml::IrrXMLReader* xml);
void ProfileXMLEdit(xml::IrrXMLReader* xml);
void ProfileXMLFixup(xml::IrrXMLReader* xml);
void ProfileXMLGrid(xml::IrrXMLReader* xml);
void ProfileXMLHatch(xml::IrrXMLReader* xml);
void ProfileXMLImport(xml::IrrXMLReader* xml);
void ProfileXMLIsometric(xml::IrrXMLReader* xml);
void ProfileXMLLPD(xml::IrrXMLReader* xml);
void ProfileXMLMouse(xml::IrrXMLReader* xml);
void ProfileXMLNote(xml::IrrXMLReader* xml);
void ProfileXMLOblique(xml::IrrXMLReader* xml);
void ProfileXMLPens(xml::IrrXMLReader* xml);
void ProfileXMLPerspective(xml::IrrXMLReader* xml);
void ProfileXMLPipe(xml::IrrXMLReader* xml);
void ProfileXMLPrint(int nIndex, xml::IrrXMLReader* xml);
void ProfileXMLRLPD(xml::IrrXMLReader* xml);
void ProfileXMLScale(xml::IrrXMLReader* xml);
void ProfileXMLScroll(xml::IrrXMLReader* xml);
void ProfileXMLUnits(xml::IrrXMLReader* xml);
void ProfileXMLUserAxis(xml::IrrXMLReader* xml);
void ProfileXMLView(xml::IrrXMLReader* xml);

void ProfileXMLAnnotateWriter(ofstream &outFile);
void ProfileXMLAppearanceWriter(ofstream &outFile);
void ProfileXMLAxonometricWriter(ofstream &outFile);
void ProfileXMLDimensionWriter(ofstream &outFile);
void ProfileXMLDrawWriter(ofstream &outFile);
void ProfileXMLDraw2Writer(ofstream &outFile);
void ProfileXMLEditWriter(ofstream &outFile);
void ProfileXMLFixupWriter(ofstream &outFile);
void ProfileXMLGridWriter(ofstream &outFile);
void ProfileXMLHatchWriter(ofstream &outFile);
void ProfileXMLImportWriter(ofstream &outFile);
void ProfileXMLIsometricWriter(ofstream &outFile);
void ProfileXMLLPDWriter(ofstream &outFile);
void ProfileXMLMouseWriter(ofstream &outFile);
void ProfileXMLNoteWriter(ofstream &outFile);
void ProfileXMLObliqueWriter(ofstream &outFile);
void ProfileXMLPensWriter(ofstream &outFile);
void ProfileXMLPerspectiveWriter(ofstream &outFile);
void ProfileXMLPipeWriter(ofstream &outFile);
void ProfileXMLPrintWriter(int nIndex, ofstream &outFile);
void ProfileXMLRLPDWriter(ofstream &outFile);
void ProfileXMLScaleWriter(ofstream &outFile);
void ProfileXMLScrollWriter(ofstream &outFile);
void ProfileXMLUnitsWriter(ofstream &outFile);
void ProfileXMLUserAxisWriter(ofstream &outFile);
void ProfileXMLViewWriter(ofstream &outFile);