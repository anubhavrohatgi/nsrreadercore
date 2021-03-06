#include "nsrpopplerdocument.h"
#include "nsrpagecropper.h"
#include "text/nsrtextbox.h"
#include "text/nsrtextpage.h"

#include "poppler/poppler/Outline.h"
#include "poppler/poppler/ErrorCodes.h"
#include "poppler/poppler/Link.h"
#include "poppler/goo/GooList.h"

#include <qmath.h>

#include <QFile>
#include <QHash>

#define NSR_CORE_PDF_MIN_ZOOM	25.0

QMutex NSRPopplerDocument::_mutex;
int NSRPopplerDocument::_refcount = 0;
UnicodeMap * NSRPopplerDocument::_utf8Map = NULL;

NSRPopplerDocument::NSRPopplerDocument (const QString& file, const QString& passwd, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_doc (NULL),
	_catalog (NULL),
	_page (NULL),
	_dev (NULL)
{
	_mutex.lock ();

	if (_refcount == 0) {
		globalParams = new GlobalParams ();

		GooString encoding ("UTF-8");
		_utf8Map = globalParams->getUnicodeMap (&encoding);
		_utf8Map->incRefCnt ();
	}

	++_refcount;
	_mutex.unlock ();

	if (!QFile::exists (file))
		return;

	setPassword (passwd);
}

NSRPopplerDocument::~NSRPopplerDocument ()
{
	if (_dev != NULL)
		delete _dev;

	if (_doc != NULL)
		delete _doc;

	_mutex.lock ();
	--_refcount;

	if (_refcount == 0) {
		_utf8Map->decRefCnt ();
		delete globalParams;
	}

	_mutex.unlock ();
}

int
NSRPopplerDocument::getPageCount () const
{
	if (_doc == NULL)
		return 0;

	return _doc->getNumPages ();
}

bool
NSRPopplerDocument::isValid () const
{
	return (_doc != NULL && _doc->isOk ());
}

NSRRenderInfo
NSRPopplerDocument::renderPage (int page)
{
	NSRRenderInfo	rinfo;
	double		dpix, dpiy;

	if (_doc == NULL || page > getPageCount () || page < 1)
		return rinfo;

	_page = _catalog->getPage (page);

	double cropWidth  = _page->getCropWidth ();
	double cropHeight = _page->getCropHeight ();

	if (isTextOnly ()) {
		TextOutputDev *		dev;
		QList<NSRTextBox *>	textList;

		dev = new TextOutputDev (0, gFalse, 0, gFalse, gFalse);

		_doc->displayPageSlice (dev, _page->getNum (), 72, 72, 0, gFalse, gFalse, gFalse, -1, -1, -1, -1);

		TextWordList *wordList = dev->makeWordList ();

		if (wordList == NULL) {
			delete dev;
			_text = QString ();
			rinfo.setSuccessRender (true);

			return rinfo;
		}

		QHash<TextWord *, NSRTextBox*> wordBoxMap;

		for (int i = 0; i < wordList->getLength (); i++) {
			TextWord *	word    = wordList->get (i);
			GooString *	gooWord = word->getText ();
			QString		string  = QString::fromUtf8 (gooWord->getCString ());
			double		xMin, yMin, xMax, yMax;

			delete gooWord;

			word->getBBox (&xMin, &yMin, &xMax, &yMax);

			NSRTextBox* textBox = new NSRTextBox (string, QRectF (xMin, yMin, xMax - xMin, yMax - yMin));
			textBox->setHasSpaceAfter (word->hasSpaceAfter () == gTrue);
			textBox->getCharBoundingBoxes().reserve (word->getLength ());

			for (int j = 0; j < word->getLength (); ++j) {
				word->getCharBBox (j, &xMin, &yMin, &xMax, &yMax);
				textBox->getCharBoundingBoxes().append (QRectF (xMin, yMin, xMax - xMin, yMax - yMin));
			}

			wordBoxMap.insert (word, textBox);
			textList.append (textBox);
		}

		for (int i = 0; i < wordList->getLength (); i++) {
			TextWord *	word    = wordList->get (i);
			NSRTextBox *	textBox = wordBoxMap.value (word);

			textBox->setNextWord (wordBoxMap.value (word->nextWord ()));
		}

		delete wordList;
		delete dev;

		/* Text page processing */
		NSRTextPage *	textPage = new NSRTextPage (QSizeF (cropWidth, cropHeight),
							    getRotation (),
							    (NSRAbstractDocument::NSRDocumentRotation)
								(((_page->getRotate () % 360) + 360) % 360));
		NSRTextBox *	next;
		QString		s;
		bool		addChar;

		foreach (NSRTextBox *word, textList) {
			const int qstringCharCount = word->getText().length ();
			next = word->getNextWord ();
			int textBoxChar = 0;

			for (int j = 0; j < qstringCharCount; j++) {
				const QChar c = word->getText().at (j);

				if (c.isHighSurrogate ()) {
					s       = c;
					addChar = false;
				} else if (c.isLowSurrogate ()) {
					s       += c;
					addChar  = true;
				} else {
					s       = c;
					addChar = true;
				}

				if (addChar) {
					QRectF charBBox = word->getCharBoundingBox (textBoxChar);
					textPage->append (s, NSRNormalizedRect (charBBox.left   () / cropWidth,
										charBBox.top    () / cropHeight,
										charBBox.right  () / cropWidth,
										charBBox.bottom () / cropHeight));
					textBoxChar++;
				}
			}

			if (word->hasSpaceAfter () && next) {
				QRectF wordBBox     = word->getBoundingBox ();
				QRectF nextWordBBox = next->getBoundingBox ();

				textPage->append (" ",
						  NSRNormalizedRect (wordBBox.right    () / cropWidth,
								     wordBBox.top      () / cropHeight,
								     nextWordBBox.left () / cropWidth,
								     wordBBox.bottom   () / cropHeight));
			}
		}

		qDeleteAll (textList);
		textPage->correctTextOrder ();
		_text = textPage->text ();
		delete textPage;

		rinfo.setSuccessRender (true);

		return rinfo;
	}

	bool isLandscape = (qAbs (_page->getRotate ()) % 180) == 90;

	double pageWidth = (((getRotation () % 180) == 90 && !isLandscape) ||
			    ((getRotation () % 180) == 0 && isLandscape)) ?
				cropHeight : cropWidth;

	double minZoom, maxZoom;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = cropWidth * cropHeight * 4;

	maxZoom = qMin (sqrt (NSR_CORE_DOCUMENT_MAX_HEAP) * sqrt (72 * 72 / pageSize) / 72 * 100 + 0.5,
			getMaxZoom (QSize (cropWidth, cropHeight)));

	if (pageSize > NSR_CORE_DOCUMENT_MAX_HEAP)
		minZoom = maxZoom;
	else
		minZoom = (maxZoom / 10) > NSR_CORE_PDF_MIN_ZOOM ? NSR_CORE_PDF_MIN_ZOOM
								 : maxZoom / 10;

	if (isZoomToWidth ()) {
		int zoomWidth = getPageWidth ();

		if (isAutoCrop ()) {
			NSRCropPads pads = getCropPads ();
			pads.setRotation ((unsigned int) getRotation ());

			zoomWidth = (int) (zoomWidth / (1 - (pads.getLeft () + pads.getRight ())) + 0.5);
		}

		double wZoom = ((double) zoomWidth / pageWidth * 100.0);
		setZoomSilent (wZoom);
	}

	if (getZoom () < minZoom)
		setZoomSilent (minZoom);

	setZoomSilent (validateMaxZoom (QSize (cropWidth, cropHeight), getZoom ()));

	_dev->startPage (0, NULL, _doc->getXRef ());

	dpix = 72.0 * getZoom () / 100.0;
	dpiy = 72.0 * getZoom () / 100.0;

	_page->display (_dev, dpix, dpiy, (int) getRotation (), gFalse, gFalse, gTrue, NULL, NULL, NULL, NULL);

	rinfo.setMinZoom (minZoom);
	rinfo.setMaxZoom (maxZoom);
	rinfo.setSuccessRender (true);

	return rinfo;
}

NSR_CORE_IMAGE_DATATYPE
NSRPopplerDocument::getCurrentPage ()
{
	if (_dev == NULL || (_dev->getBitmapHeight () == 1 && _dev->getBitmapWidth () == 1))
		return NSR_CORE_IMAGE_DATATYPE ();

	SplashBitmap *bitmap = _dev->getBitmap ();
	int bw = bitmap->getWidth ();
	int bh = bitmap->getHeight ();

	char *dataPtr = (char *) _dev->getBitmap()->getDataPtr ();

	int rowBytes = bw * 3;
	while (rowBytes % 4)
		rowBytes += 1;

	NSRCropPads pads = isAutoCrop () ? getCropPads () : NSRCropPads ();
	pads.setRotation ((unsigned int) getRotation ());

	int top    = (int) (bh * pads.getTop () + 0.5);
	int bottom = (int) (bh * pads.getBottom () + 0.5);
	int left   = (int) (bw * pads.getLeft () + 0.5);
	int right  = (int) (bw * pads.getRight () + 0.5);

#ifdef Q_OS_BLACKBERRY
	bb::ImageData imgData (bb::PixelFormat::RGBX,
			       bw - left - right,
			       bh - top - bottom);

	unsigned char *addr = (unsigned char *) imgData.pixels ();
	int stride = imgData.bytesPerLine ();

	for (int i = top; i < bh - bottom; ++i) {
		unsigned char *inAddr = (unsigned char *) (dataPtr + i * rowBytes);

		for (int j = left; j < bw - right; ++j) {
			if (isInvertedColors ()) {
				unsigned char meanVal = (unsigned char) (((unsigned int) 255 * 3 - inAddr[j * 3 + 0] -
												   inAddr[j * 3 + 1] -
												   inAddr[j * 3 + 2]) / 3);

				addr[(j - left) * 4 + 0] = meanVal;
				addr[(j - left) * 4 + 1] = meanVal;
				addr[(j - left) * 4 + 2] = meanVal;
			} else {
				addr[(j - left) * 4 + 0] = inAddr[j * 3 + 0];
				addr[(j - left) * 4 + 1] = inAddr[j * 3 + 1];
				addr[(j - left) * 4 + 2] = inAddr[j * 3 + 2];
			}
		}

		addr += stride;
	}

	_dev->startPage (0, NULL, _doc->getXRef ());

	return imgData;
#else
	Q_UNUSED (dataPtr);
	Q_UNUSED (top);
	Q_UNUSED (bottom);
	Q_UNUSED (left);
	Q_UNUSED (right);
	return NSR_CORE_IMAGE_DATATYPE ();
#endif
}

QString
NSRPopplerDocument::getText ()
{
	if (_text.isEmpty ())
		return NSRAbstractDocument::getText ();
	else {
		QString ret = _text;
		_text.clear ();

		return ret;
	}
}

void
NSRPopplerDocument::setPassword (const QString &passwd)
{
	if (_doc != NULL)
		return;

	NSRAbstractDocument::setPassword (passwd);
	createInternalDoc (passwd);
}

bool
NSRPopplerDocument::isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const
{
	return (style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC ||
		style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
}

NSRTocEntry *
NSRPopplerDocument::getToc () const
{
	if (_doc == NULL)
		return NULL;

	Outline *outline = _doc->getOutline ();

	if (outline == NULL)
		return NULL;

	GooList * items = outline->getItems ();

	if (!items || items->getLength () < 1)
		return NULL;

	NSRTocEntry *toc = new NSRTocEntry (QString (), -1);

	if (items->getLength () > 0)
		addTocChildren (toc, items);

	return toc;
}

void
NSRPopplerDocument::createInternalDoc (QString passwd)
{
	SplashColor	bgColor;
	GooString	*fileName;
	GooString	*passwdStr;

	if (_doc != NULL)
		return;

	bgColor[0] = 255;
	bgColor[1] = 255;
	bgColor[2] = 255;
	_dev = new SplashOutputDev (splashModeRGB8, 4, gFalse, bgColor);

	fileName = new GooString (getDocumentPath().toUtf8().data ());

	if (!passwd.isEmpty ())
		passwdStr = new GooString (passwd.toUtf8().data ());
	else
		passwdStr = NULL;

	_doc = new PDFDoc (fileName, passwdStr);

	if (!_doc->isOk ()) {
		if (_doc->getErrorCode () == errEncrypted)
			setLastError (NSR_DOCUMENT_ERROR_PASSWD);
		else
			setLastError (NSR_DOCUMENT_ERROR_UNKNOWN);

		delete _doc;
		delete _dev;
		_dev = NULL;
		_doc = NULL;

		return;
	}

	_catalog = _doc->getCatalog ();
	_page = _catalog->getPage (1);
	_dev->startDoc (_doc);
}

void
NSRPopplerDocument::addTocChildren (NSRTocEntry *parent, GooList *items) const
{
	if (parent == NULL || items == NULL)
		return;

	int numItems = items->getLength ();

	for (int i = 0; i < numItems; ++i) {
		/* Iterate over every object in items */
		OutlineItem * outlineItem = (OutlineItem *) items->get (i);

		/* 1. Create entry using outline item's title */
		Unicode *uniChar = outlineItem->getTitle ();
		int titleLength  = outlineItem->getTitleLength ();
		QString name     = unicodeToQString (uniChar, titleLength);

		NSRTocEntry *entry = new NSRTocEntry (name, -1);
		parent->appendChild (entry);

		/* 2. Find the page the link refers to */
		LinkAction * action = outlineItem->getAction ();

		if (action != NULL) {
			switch (action->getKind ()) {
			case actionGoTo:
			{
				GBool      freeDest    = gFalse;
				LinkGoTo * goToLink    = static_cast<LinkGoTo *> (action);
				LinkDest * destination = goToLink->getDest ();

				if (destination == NULL && goToLink->getNamedDest () != NULL) {
					destination = _doc->findDest (goToLink->getNamedDest ());

					if (destination != NULL)
						freeDest = gTrue;

					if (entry->getTitle().isEmpty ())
						entry->setTitle (QString (goToLink->getNamedDest()->getCString ()));
				}

				if (destination != NULL && destination->isOk ()) {
					if (!destination->isPageRef ())
						entry->setPage (destination->getPageNum ());
					else {
						Ref ref = destination->getPageRef ();
						entry->setPage (_doc->findPage (ref.num, ref.gen));
					}
				}

				if (destination != NULL && freeDest)
					delete destination;

				break;
			}
			case actionGoToR:
			{
				GBool       freeDest    = gFalse;
				LinkGoToR * goToRLink   = static_cast<LinkGoToR *> (action);
				LinkDest *  destination = goToRLink->getDest ();

				entry->setExternal (goToRLink->getFileName () != NULL);
				entry->setExternalFile (QString (goToRLink->getFileName()->getCString ()));

				if (destination == NULL && goToRLink->getNamedDest () != NULL && !entry->isExternal ()) {
					destination = _doc->findDest (goToRLink->getNamedDest ());

					if (destination != NULL)
						freeDest = gTrue;
				}

				if (entry->getTitle().isEmpty () && goToRLink->getNamedDest () != NULL)
					entry->setTitle (QString (goToRLink->getNamedDest()->getCString ()));

				if (destination != NULL && destination->isOk ()) {
					if (!destination->isPageRef ())
						entry->setPage (destination->getPageNum ());
					else {
						Ref ref = destination->getPageRef ();
						entry->setPage (_doc->findPage (ref.num, ref.gen));
					}
				}

				if (destination != NULL && freeDest)
					delete destination;

				break;
			}
			case actionURI:
			{
				LinkURI * uri = static_cast<LinkURI *> (action);
				entry->setUri (QString (uri->getURI()->getCString ()));
				entry->setExternal (true);

				if (entry->getTitle().isEmpty ())
					entry->setTitle (entry->getUri ());
			}
			default:
				break;
			}
		}

		if (entry->getTitle().isEmpty ())
			/* Skip entries without title */
			delete entry;
		else {
			/* 3. Recursively descend over children */
			outlineItem->open ();
			GooList * children = outlineItem->getKids ();

			if (children)
				addTocChildren (entry, children);
		}
	}

}

QString
NSRPopplerDocument::unicodeToQString (Unicode *u, int len) const
{
	if (u == NULL || len <= 0)
		return QString ();

	/* Ignore the last character if it is 0x0 */
	if (u[len - 1] == 0)
		--len;

	GooString convertedStr;

	for (int i = 0; i < len; ++i) {
		char buf[8];
		const int n = _utf8Map->mapUnicode (u[i], buf, sizeof (buf));
		convertedStr.append (buf, n);
	}

	return QString::fromUtf8 (convertedStr.getCString (), convertedStr.getLength ());
}
