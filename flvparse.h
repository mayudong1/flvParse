#ifndef FLVPARSE_H
#define FLVPARSE_H

#include <QtWidgets/QMainWindow>
#include "ui_flvparse.h"
#include "FLVStructParse.h"
#include <QLabel>

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
    void displayFLV(bool bShowVideo = true, bool bShowAudio = true);
    void displayHex(unsigned char* pData, int& len);
	void displayFLVHeader(QTreeWidgetItem* root);
    void displayFLVTags(QTreeWidgetItem* root, bool bShowVideo = true, bool bShowAudio = true);
	void displayFLVTagDetail(QTreeWidgetItem* tagItem, FLVTag* tag);
    void displayMetadataDetail(QTreeWidgetItem* dataItem, FLVMetadataTagBody* metadataTag);
    void displayVideoDetail(QTreeWidgetItem* dataItem, FLVVideoTagBody* videoTag);

	void setHighlight(int start, int len);
	void clearDisplay();

private:
	void setItemFLVPosition(QTreeWidgetItem* item, FLVPosition* pos);
	FLVPosition* getItemFLVPosition(QTreeWidgetItem* item);

private slots:
	void on_flvStructTree_itemClicked(QTreeWidgetItem * item, int column);

    void on_actionOpen_triggered();

    void on_actionVideo_Only_triggered();

    void on_actionAudio_Only_triggered();

    void on_actionAll_Tags_triggered();

private:
	FLVStruct* flv;
	class FLVStructParse* parser;

	int curShowHexDataStartInFLV;
	int curShowHexDataLen;

private:
	Ui::flvParseClass ui;
    QLabel* statusLable;
};

#endif // FLVPARSE_H
