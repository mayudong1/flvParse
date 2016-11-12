#include "flvparse.h"
#include <QMessageBox>
#include <qtextedit.h>
#include <qtreewidget.h>
#include <qtextcursor.h>
#include <QtGlobal>  

#include "Util.h"
#include "FLVStructParse.h"

flvParse::flvParse(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	flv = NULL;
	parser = new FLVStructParse();
	
}

flvParse::~flvParse()
{
	if (parser)
	{
		delete parser;
		parser = NULL;
	}
}

QString convert(unsigned char* pData, int len)
{
	QString strResult;
	for (int i = 0; i < len; i++)
	{
		QString tmp;
		tmp.sprintf("%02X ", pData[i]);
		strResult.append(tmp);
	}
	return strResult;
}

void flvParse::displayHex(unsigned char* pData, int len)
{
	Q_ASSERT(pData && len >= 0);

	int minLen = MYD_MIN(len, 1024);
	QString str = convert(pData, minLen);
	this->ui.hexView->setText(str);
}

void flvParse::setItemFLVPosition(QTreeWidgetItem* item, FLVPosition* pos)
{	
	QVariant value = QVariant::fromValue((void *)pos);
	item->setData(0, Qt::UserRole, value);
}

FLVPosition* flvParse::getItemFLVPosition(QTreeWidgetItem* item)
{
	FLVPosition* pos = NULL;
	QVariant value = item->data(0, Qt::UserRole);
	pos = (FLVPosition*)value.value<void *>();
	return pos;
}

void flvParse::displayFLVHeader(QTreeWidgetItem* root)
{
	QTreeWidgetItem *header = new QTreeWidgetItem(QStringList("FLV Header"));
	setItemFLVPosition(header, &flv->header.pos);
	root->addChild(header);

	QString strTmp;
	strTmp.sprintf("signature: %s", flv->header.signature.value);
	QTreeWidgetItem *signature = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(signature, &flv->header.signature.pos);
	header->addChild(signature);

	strTmp.sprintf("version: %d", flv->header.version.value);
	QTreeWidgetItem *version = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(version, &flv->header.version.pos);
	header->addChild(version);

	strTmp.sprintf("has video: %d", flv->header.hasVideo.value);
	QTreeWidgetItem *hasVideo = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(hasVideo, &flv->header.hasVideo.pos);
	header->addChild(hasVideo);

	strTmp.sprintf("has audio: %d", flv->header.hasAudio.value);
	QTreeWidgetItem *hasAudio = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(hasAudio, &flv->header.hasAudio.pos);
	header->addChild(hasAudio);

	strTmp.sprintf("header size: %d", flv->header.headerLen.value);
	QTreeWidgetItem *headerSize = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(headerSize, &flv->header.headerLen.pos);
	header->addChild(headerSize);
}

void flvParse::displayFLVTags(QTreeWidgetItem* root)
{
	QString strTmp;

	FLVTag* tag = flv->tagList->next;
	int videoIndex = 0;
	int audioIndex = 0;
	while (tag != NULL)
	{
		QString strTagType("unknown");
		switch (tag->type.value)
		{
		case 8:
			strTagType.sprintf("Audio Tag%d", audioIndex++);
			break;
		case 9:
			strTagType.sprintf("Video Tag%d", videoIndex++);
			break;
		case 18:
			strTagType = "Metadata Tag";
			break;
		default:
			break;
		}
		QTreeWidgetItem *tagItem = new QTreeWidgetItem(QStringList(strTagType));
		setItemFLVPosition(tagItem, &tag->pos);
		root->addChild(tagItem);

		strTmp.sprintf("preTagSize: %d", tag->tagSize);
		QTreeWidgetItem *preTagSizeItem = new QTreeWidgetItem(QStringList(strTmp));
		setItemFLVPosition(preTagSizeItem, &tag->tagSize.pos);
		root->addChild(preTagSizeItem);
		tag = tag->next;
	}
}

void flvParse::displayFLV()
{
	flv = parser->parseFile("d:\\ffmpeg\\a.flv");
	if (flv == NULL)
		return;

	QTreeWidget* tree = this->ui.flvStructTree;

	QTreeWidgetItem *root = new QTreeWidgetItem(QStringList("FLVStruct"));
	tree->addTopLevelItem(root);
	root->setExpanded(true);

	displayHex(flv->data, flv->dataLen);
	displayFLVHeader(root);
	displayFLVTags(root);
}

void flvParse::setHighlight(int start, int len)
{
	QTextEdit* edit = this->ui.hexView;
	QTextCursor cur = edit->textCursor();
	
	int startTextPos = start * 3;
	int lenText = len * 3 - 1;

	cur.setPosition(startTextPos);
	cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, lenText);
	QTextEdit::ExtraSelection selection;
	selection.cursor = cur;
	QColor lineColor = QColor(Qt::yellow).lighter(160);
	selection.format.setBackground(lineColor);
	QList<QTextEdit::ExtraSelection> extraSelections;
	extraSelections.append(selection);
	edit->setExtraSelections(extraSelections);
}

void flvParse::on_flvStructTree_itemClicked(QTreeWidgetItem * item, int column)
{
	FLVPosition* pos = getItemFLVPosition(item);
	if (pos == NULL)
		return;
			
	setHighlight(pos->start, pos->len);
}

void flvParse::on_openButton_clicked()
{

}

void flvParse::on_testButton_clicked()
{	
	displayFLV();
}

void flvParse::on_test2Button_clicked()
{

}