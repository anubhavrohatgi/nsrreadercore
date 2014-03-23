#include "nsrtextdocument.h"

#include <qmath.h>

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QTextCodec>

#define NSR_CORE_TEXT_PAGE_SIZE	5120
#define NSR_CORE_TEXT_MIN_ZOOM	25.0
#define NSR_CORE_TEXT_MAX_ZOOM	400.0

NSRTextDocument::NSRTextDocument (const QString &file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_pagesCount (0)
{
	QFileInfo info (file);

	NSRAbstractDocument::setTextOnly (true);

	_pagesCount = (int) (ceil ((double) info.size () / NSR_CORE_TEXT_PAGE_SIZE) + 0.5);
}

NSRTextDocument::~NSRTextDocument ()
{
}

int
NSRTextDocument::getNumberOfPages () const
{
	return _pagesCount;
}

void
NSRTextDocument::renderPage (int page)
{
	if (page < 1 || page > getNumberOfPages ())
		return;

	if (!isValid ())
		return;

	_text = QString ();

	QFile data (getDocumentPath ());

	if (data.open (QFile::ReadOnly)) {
		QDataStream	in (&data);
		QByteArray	ba, bn;
		int		strPos;
		int		bytesRead;

		ba.resize (NSR_CORE_TEXT_PAGE_SIZE + NSR_CORE_TEXT_PAGE_SIZE / 2);
		in.device()->seek ((page - 1) * NSR_CORE_TEXT_PAGE_SIZE);

		if ((bytesRead = in.readRawData (ba.data (), NSR_CORE_TEXT_PAGE_SIZE + NSR_CORE_TEXT_PAGE_SIZE / 2)) == -1) {
			data.close ();
			return;
		}

		ba.truncate (bytesRead);
		bn = ba.left (NSR_CORE_TEXT_PAGE_SIZE);

		/* Complete last word */
		if (ba.size () > NSR_CORE_TEXT_PAGE_SIZE) {
			strPos = NSR_CORE_TEXT_PAGE_SIZE;
			while (strPos < ba.size () && !QChar(ba.at (strPos)).isSpace ())
				bn.append (ba.at (strPos++));
		}

		_text = QTextCodec::codecForName(getEncoding().toAscii ())->toUnicode (bn);

		if (!_text.isEmpty () && page > 1) {
			/* Remove previous semi-full words and spaces */
			strPos = -1;
			for (int i = 0; i < _text.size () / 2; ++i)
				if (_text.at(i).isSpace ()) {
					while (_text.at(++i).isSpace () && i < _text.size () / 2);
					strPos = i;
					break;
				}

			if (strPos != -1)
				_text = _text.right (_text.size () - strPos);
		}

		data.close ();
	}
}

NSR_CORE_IMAGE_DATATYPE
NSRTextDocument::getCurrentPage ()
{
	return bb::ImageData ();
}

bool
NSRTextDocument::isValid () const
{
	QFileInfo info (getDocumentPath ());

	return info.exists();
}

double
NSRTextDocument::getMaxZoom ()
{
	return NSR_CORE_TEXT_MAX_ZOOM;
}

double
NSRTextDocument::getMinZoom ()
{
	return NSR_CORE_TEXT_MAX_ZOOM;
}

void
NSRTextDocument::setTextOnly (bool textOnly)
{
	/* This is only a text document */
	Q_UNUSED (textOnly);
}

QString
NSRTextDocument::getText ()
{
	QString ret = _text;
	_text.clear ();

	return ret;
}

bool
NSRTextDocument::isEncodingUsed () const
{
	return true;
}

bool
NSRTextDocument::isAutoCrop () const
{
	return false;
}

bool
NSRTextDocument::isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const
{
	return (style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
}
