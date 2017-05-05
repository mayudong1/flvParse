#ifndef FLVPARSE_H
#define FLVPARSE_H

#include <QtWidgets/QMainWindow>
#include "ui_flvparse.h"
#include "FLVStructParse.h"

#define SHOW_MORE_BYTES_PRE 128
#define SHOW_MORE_BYTES_REAR 1024
#define SHOW_MORE_BYTES_ALL	SHOW_MORE_BYTES_PRE+SHOW_MORE_BYTES_REAR

class flvParse : public QMainWindow
{
	Q_OBJECT

public:
	flvParse(QWidget *parent = 0);
	~flvParse();
private:
	void displayFLV(QString fileName);
	void displayHex(unsigned char* pData, int& len);
	void displayFLVHeader(QTreeWidgetItem* root);
	void displayFLVTags(QTreeWidgetItem* root);
	void displayFLVTagDetail(QTreeWidgetItem* tagItem, FLVTag* tag);
    void displayMetadataDetail(QTreeWidgetItem* dataItem, FLVMetadataTagBody* metadataTag);
    void displayVideoDetail(QTreeWidgetItem* dataItem, FLVVideoTagBody* videoTag);

	void setHighlight(int start, int len);
	void clearDisplay();

private:
	void setItemFLVPosition(QTreeWidgetItem* item, FLVPosition* pos);
	FLVPosition* getItemFLVPosition(QTreeWidgetItem* item);

private slots:
	void on_openButton_clicked();			
	void on_flvStructTree_itemClicked(QTreeWidgetItem * item, int column);

private:
	FLVStruct* flv;
	class FLVStructParse* parser;

	int curShowHexDataStartInFLV;
	int curShowHexDataLen;

private:
	Ui::flvParseClass ui;
};

#endif // FLVPARSE_H
