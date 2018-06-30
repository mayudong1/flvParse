#include "flvparse.h"
#include <QMessageBox>
#include <QLabel>
#include <qtextedit.h>
#include <qtreewidget.h>
#include <qtextcursor.h>
#include <qfiledialog.h>
#include <QtGlobal>  

#include "Util.h"
#include "FLVStructParse.h"

flvParse::flvParse(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	flv = NULL;
	parser = new FLVStructParse();
	curShowHexDataStartInFLV = 0;
	curShowHexDataLen = 0;	
    ui.flvStructTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.flvStructTree->header()->setStretchLastSection(false);
    ui.flvStructTree->setUniformRowHeights(true);

    statusLable = new QLabel("Ready", this);
    this->ui.statusBar->addPermanentWidget(statusLable);
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

void flvParse::displayHex(unsigned char* pData, int& len)
{
	Q_ASSERT(pData && len >= 0);

    len = FLV_MIN(len, 1024);
	QString str = convert(pData, len);
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

void flvParse::setItemTagData(QTreeWidgetItem* item, FLVTag* tag)
{
    QVariant value = QVariant::fromValue((void *)tag);
    item->setData(1, Qt::UserRole, value);
}

FLVTag* flvParse::getItemTagData(QTreeWidgetItem* item)
{
    FLVTag* tag = NULL;
    QVariant value = item->data(1, Qt::UserRole);
    tag = (FLVTag*)value.value<void *>();
    return tag;
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

	strTmp.sprintf("First Tag size: %d", flv->firstTagSize.value);
	QTreeWidgetItem *firstTagSize = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(firstTagSize, &flv->firstTagSize.pos);
	root->addChild(firstTagSize);

}
void flvParse::displayFLVTagDetail(QTreeWidgetItem* tagItem, FLVTag* tag)
{
	QTreeWidgetItem *header = new QTreeWidgetItem(QStringList("Tag Header"));
	setItemFLVPosition(header, &tag->header.pos);
	tagItem->addChild(header);

	QString strTmp;
	strTmp.sprintf("type: %d", tag->header.type.value);
	QTreeWidgetItem *type = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(type, &tag->header.type.pos);
	header->addChild(type);

	strTmp.sprintf("data size: %d", tag->header.dataSize.value);
	QTreeWidgetItem *dataSize = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(dataSize, &tag->header.dataSize.pos);
	header->addChild(dataSize);

	strTmp.sprintf("timestamp: %d", tag->header.timestamp.value);
	QTreeWidgetItem *timestamp = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(timestamp, &tag->header.timestamp.pos);
	header->addChild(timestamp);

	strTmp.sprintf("stream id: %d", tag->header.streamId.value);
	QTreeWidgetItem *streamId = new QTreeWidgetItem(QStringList(strTmp));
	setItemFLVPosition(streamId, &tag->header.streamId.pos);
	header->addChild(streamId);

    QTreeWidgetItem *dataItem = new QTreeWidgetItem(QStringList("Tag Data"));
    setItemFLVPosition(dataItem, &tag->data->pos);
    tagItem->addChild(dataItem);
    if(tag->header.type.value == 0x12)
    {
        FLVMetadataTagBody* metadataTag = (FLVMetadataTagBody*)tag->data;
        displayMetadataDetail(dataItem, metadataTag);
    }
    else if(tag->header.type.value == 0x09)
    {
        FLVVideoTagBody* videoTag = (FLVVideoTagBody*)tag->data;
        displayVideoDetail(dataItem, videoTag);
    }
}

void flvParse::displayMetadataDetail(QTreeWidgetItem* dataItem, FLVMetadataTagBody* metadataTag)
{
    QString strTmp;
    strTmp.sprintf("amf0 type: %d", metadataTag->amf0Type.value);
    QTreeWidgetItem *afm0TypeItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(afm0TypeItem, &metadataTag->amf0Type.pos);
    dataItem->addChild(afm0TypeItem);

    strTmp.sprintf("data: %s", metadataTag->amf0Data.value.c_str());
    QTreeWidgetItem *afm0DataItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(afm0DataItem, &metadataTag->amf0Data.pos);
    dataItem->addChild(afm0DataItem);


    strTmp.sprintf("amf1 type: %d", metadataTag->amf1Type.value);
    QTreeWidgetItem *amf1TypeItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(amf1TypeItem, &metadataTag->amf1Type.pos);
    dataItem->addChild(amf1TypeItem);

    strTmp.sprintf("amf1 count: %d", metadataTag->amf1Count.value);
    QTreeWidgetItem *amf1CountItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(amf1CountItem, &metadataTag->amf1Count.pos);
    dataItem->addChild(amf1CountItem);

    strTmp.sprintf("metadata");
    QTreeWidgetItem *metaDataItem = new QTreeWidgetItem(QStringList(strTmp));
    dataItem->addChild(metaDataItem);

    MetadataInfo* pMeta = metadataTag->metaArray;
    for(int i=0;i<metadataTag->amf1Count.value;i++)
    {
        MetadataInfo* metaKeyValue = pMeta->next;
        if(metaKeyValue == NULL)
            break;
        if(metaKeyValue->valueType == AMF_NUMBER)
        {
            strTmp.sprintf("%s: %f", metaKeyValue->key.c_str(), metaKeyValue->dValue);
        }
        else if(metaKeyValue->valueType == AMF_STRING)
        {
            strTmp.sprintf("%s: %s", metaKeyValue->key.c_str(), metaKeyValue->strValue.c_str());
        }
        else if(metaKeyValue->valueType == AMF_BOOLEAN)
        {
            strTmp.sprintf("%s: %d", metaKeyValue->key.c_str(), metaKeyValue->bValue);
        }

        QTreeWidgetItem *keyValueItem = new QTreeWidgetItem(QStringList(strTmp));
        keyValueItem->setToolTip(0, strTmp);
        metaDataItem->addChild(keyValueItem);
        pMeta = pMeta->next;
    }
}

void flvParse::displayVideoDetail(QTreeWidgetItem* dataItem, FLVVideoTagBody* videoTag)
{
    QString strTmp;
    strTmp.sprintf("Frame Type: %d", videoTag->frameType.value);
    QTreeWidgetItem *frameTypeItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(frameTypeItem, &videoTag->frameType.pos);
    dataItem->addChild(frameTypeItem);

    strTmp.sprintf("Codec ID: %d", videoTag->codecID.value);
    QTreeWidgetItem *codecIDItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(codecIDItem, &videoTag->codecID.pos);
    dataItem->addChild(codecIDItem);

    strTmp.sprintf("AVC Packet Type: %d", videoTag->avcPacketType.value);
    QTreeWidgetItem *avcPacketTypeItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(avcPacketTypeItem, &videoTag->avcPacketType.pos);
    dataItem->addChild(avcPacketTypeItem);

    strTmp.sprintf("Composition Time: %d", videoTag->compositionTime.value);
    QTreeWidgetItem *compositionTimeItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(compositionTimeItem, &videoTag->compositionTime.pos);
    dataItem->addChild(compositionTimeItem);

    strTmp.sprintf("Video Data");
    QTreeWidgetItem *videoDataItem = new QTreeWidgetItem(QStringList(strTmp));
    setItemFLVPosition(videoDataItem, &videoTag->nalu.pos);
    dataItem->addChild(videoDataItem);
}

void flvParse::displayFLVTags(QTreeWidgetItem* root, bool bShowVideo, bool bShowAudio)
{
	QString strTmp;

	FLVTag* tag = flv->tagList->next;
	int videoIndex = 0;
	int audioIndex = 0;
	while (tag != NULL)
	{
		QString strTagType("Unknown Tag");
		switch (tag->header.type.value)
		{
		case 8:
            if(!bShowAudio)
            {
                tag = tag->next;
                continue;
            }
			strTagType.sprintf("Audio Tag%d", audioIndex++);
			break;
		case 9:
            if(!bShowVideo)
            {
                tag = tag->next;
                continue;
            }
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
		setItemTagData(tagItem, tag);
		root->addChild(tagItem);		

		displayFLVTagDetail(tagItem, tag);

		strTmp.sprintf("preTagSize: %d", tag->preTagSize);
		QTreeWidgetItem *preTagSizeItem = new QTreeWidgetItem(QStringList(strTmp));
		setItemFLVPosition(preTagSizeItem, &tag->preTagSize.pos);
		root->addChild(preTagSizeItem);
		tag = tag->next;
	}
}

void flvParse::displayFLV(QString fileName)
{	
	flv = parser->parseFile(fileName.toStdString().c_str());
	if (flv == NULL)
	{		
		QMessageBox::information(this, "flv parse", "flv struct error");
		return;
	}		

    displayFLV();
}

void flvParse::displayFLV(bool bShowVideo, bool bShowAudio)
{
    if(flv == NULL)
        return;

    clearDisplay();
    QTreeWidget* tree = this->ui.flvStructTree;
    QTreeWidgetItem *root = new QTreeWidgetItem(QStringList("FLVStruct"));
    tree->addTopLevelItem(root);
    root->setExpanded(true);

    curShowHexDataLen = 1024;
    displayHex(flv->data, curShowHexDataLen);
    displayFLVHeader(root);
    displayFLVTags(root, bShowVideo, bShowAudio);
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

	cur.clearSelection();
	cur.setPosition(startTextPos);
	edit->setTextCursor(cur);
}

void flvParse::clearDisplay()
{
	QTreeWidget* tree = this->ui.flvStructTree;
	tree->clear();
	this->ui.hexView->clear();
}

void flvParse::on_flvStructTree_itemClicked(QTreeWidgetItem * item, int column)
{
	FLVPosition* pos = getItemFLVPosition(item);
	if (pos == NULL)
		return;
			
	if (pos->start < curShowHexDataStartInFLV 
		|| pos->start > curShowHexDataStartInFLV+curShowHexDataLen)
	{
		curShowHexDataStartInFLV = pos->start - SHOW_MORE_BYTES_PRE;
		if (curShowHexDataStartInFLV < 0)
			curShowHexDataStartInFLV = 0;
		curShowHexDataLen = pos->len + SHOW_MORE_BYTES_ALL;
		displayHex(flv->data+curShowHexDataStartInFLV, curShowHexDataLen);
	}
	
	int highlightStart = pos->start - curShowHexDataStartInFLV;
    int highlightLen = FLV_MIN((curShowHexDataLen-highlightStart), pos->len);

	setHighlight(highlightStart, highlightLen);

    QString str;
    str.sprintf("offset = %d, length = %d", pos->start, pos->len);
    statusLable->setText(str);
}

void flvParse::on_actionOpen_triggered()
{
    QFileDialog *dlg = new QFileDialog(this);
    if (dlg->exec() == QFileDialog::Accepted)
    {
        QStringList files = dlg->selectedFiles();
        QString fileName = files.at(0);
        displayFLV(fileName);
    }
}

void flvParse::on_actionVideo_Only_triggered()
{
    displayFLV(true, false);
    this->ui.actionAudio_Only->setChecked(false);
    this->ui.actionVideo_Only->setChecked(true);
    this->ui.actionAll_Tags->setChecked(false);
    this->ui.statusBar->showMessage("Show Video Tags Only");
}

void flvParse::on_actionAudio_Only_triggered()
{
    displayFLV(false, true);
    this->ui.actionAudio_Only->setChecked(true);
    this->ui.actionVideo_Only->setChecked(false);
    this->ui.actionAll_Tags->setChecked(false);
    this->ui.statusBar->showMessage("Show Audio Tags Only");
}

void flvParse::on_actionAll_Tags_triggered()
{
    displayFLV();
    this->ui.actionAudio_Only->setChecked(false);
    this->ui.actionVideo_Only->setChecked(false);
    this->ui.actionAll_Tags->setChecked(true);
    this->ui.statusBar->showMessage("Show All Tags");
}

void flvParse::on_actionKeyFrame_triggered()
{
    ui.flvStructTree->hide();
    QTreeWidgetItem* root = ui.flvStructTree->topLevelItem(0);
    for(int i=0; i<root->childCount(); i++)
    {
        QTreeWidgetItem* item = root->child(i);
        FLVTag* tag = getItemTagData(item);
        if(tag == NULL)
            continue;
        if(tag->header.type.value == 9
                && ((FLVVideoTagBody*)(tag->data))->frameType.value == 1
                && ((FLVVideoTagBody*)(tag->data))->avcPacketType.value == 1
                )
        {
            item->setBackgroundColor(0, Qt::yellow);
        }
    }
    ui.flvStructTree->show();
}

void flvParse::on_actionShowMetadata_triggered()
{
    ui.flvStructTree->hide();
    QTreeWidgetItem* root = ui.flvStructTree->topLevelItem(0);
    for(int i=0; i<root->childCount(); i++)
    {
        QTreeWidgetItem* item = root->child(i);
        FLVTag* tag = getItemTagData(item);
        if(tag == NULL)
            continue;
        if(tag->header.type.value == 18)
        {
            item->setBackgroundColor(0, Qt::yellow);
        }
    }
    ui.flvStructTree->show();
}

void flvParse::on_actionShowVideoSequenceHeader_triggered()
{
    ui.flvStructTree->hide();
    QTreeWidgetItem* root = ui.flvStructTree->topLevelItem(0);
    for(int i=0; i<root->childCount(); i++)
    {
        QTreeWidgetItem* item = root->child(i);
        FLVTag* tag = getItemTagData(item);
        if(tag == NULL)
            continue;
        if(tag->header.type.value == 9 && ((FLVVideoTagBody*)(tag->data))->avcPacketType.value == 0)
        {
            item->setBackgroundColor(0, Qt::yellow);
        }
    }
    ui.flvStructTree->show();
}
