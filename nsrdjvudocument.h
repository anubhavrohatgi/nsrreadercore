#ifndef NSRDJVUDOCUMENT_H
#define NSRDJVUDOCUMENT_H

#include "nsrabstractdocument.h"

#include <djvu/ddjvuapi.h>
#include <djvu/miniexp.h>

enum NSRDjVuErrorType {
	NSR_DJVU_ERROR_NONE	= 0,
	NSR_DJVU_ERROR_FILENAME	= 1,
	NSR_DJVU_ERROR_OTHER	= 2
};

struct NSRDjVuError {
	NSRDjVuErrorType	type;
	QString			text;
};

class NSRDjVuDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRDjVuDocument (const QString& file, QObject *parent = 0);
	virtual ~NSRDjVuDocument ();
	int getNumberOfPages () const;
	void renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	double getMaxZoom ();
	double getMinZoom ();
	QString getText ();
	void setZoom (double zoom);
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	inline NSRAbstractDocument::NSRDocumentStyle getPrefferedDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}
	inline void setEncoding (const QString& encoding) {
		Q_UNUSED (encoding);
	}
	inline QString getEncoding () const {
		return QString ("UTF-8");
	}

private:
	void handleEvents (ddjvu_context_t* context, bool wait, NSRDjVuError *error);
	void handleMessage (const ddjvu_message_t *msg, NSRDjVuError *error);
	void waitForMessage (ddjvu_context_t* context, ddjvu_message_tag_t message, NSRDjVuError *error);
	QSize getPageSize (int page);

	QSize			_cachedPageSize;
	QSize			_imgSize;
	QString			_text;
	ddjvu_context_t		*_context;
	ddjvu_document_t	*_doc;
	ddjvu_page_t		*_page;
	ddjvu_format_t		*_format;
	ddjvu_render_mode_t	_renderMode;
	double			_cachedMinZoom;
	double			_cachedMaxZoom;
	int			_cachedResolution;
	int			_pageCount;
	bool			_readyForLoad;
	char			*_imgData;
};

#endif // NSRDJVUDOCUMENT_H
