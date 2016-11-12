#ifndef FLVPARSE_H
#define FLVPARSE_H

#include <QtWidgets/QMainWindow>
#include "ui_flvparse.h"
#include "FLVStructParse.h"

class flvParse : public QMainWindow
{
	Q_OBJECT

public:
	flvParse(QWidget *parent = 0);
	~flvParse();
private:
	void displayFLV();
	void displayHex(unsigned char* pData, int len);
	void displayFLVHeader(QTreeWidgetItem* root);
	void displayFLVTags(QTreeWidgetItem* root);

	void setHighlight(int start, int len);

private:
	void setItemFLVPosition(QTreeWidgetItem* item, FLVPosition* pos);
	FLVPosition* getItemFLVPosition(QTreeWidgetItem* item);

private slots:
	void on_openButton_clicked();
	void on_testButton_clicked();
	void on_test2Button_clicked();
	
	void on_flvStructTree_itemClicked(QTreeWidgetItem * item, int column);

private:
	FLVStruct* flv;
	class FLVStructParse* parser;
private:
	Ui::flvParseClass ui;
};

#endif // FLVPARSE_H
