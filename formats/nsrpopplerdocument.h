#ifndef __NSRPOPPLERDOCUMENT_H__
#define __NSRPOPPLERDOCUMENT_H__

/**
 * @file nsrpopplerdocument.h
 * @author Alexander Saprykin
 * @brief PDF file handler
 */

#include <QMutex>

#include "nsrabstractdocument.h"

#include "poppler/poppler/GlobalParams.h"
#include "poppler/poppler/PDFDoc.h"
#include "poppler/poppler/SplashOutputDev.h"
#include "poppler/poppler/TextOutputDev.h"
#include "poppler/poppler/OutputDev.h"
#include "poppler/poppler/UnicodeMap.h"
#include "poppler/splash/SplashTypes.h"
#include "poppler/splash/SplashBitmap.h"

/**
 * @class NSRPopplerDocument nsrpopplerdocument.h
 * @brief Class for PDF file handler
 */
class NSRPopplerDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	/**
	 * @brief Constructor with parameters
	 * @param file Path to file.
	 * @param passwd File password if any.
	 * @param parent Parent object.
	 */
	NSRPopplerDocument (const QString& file, const QString& passwd = QString (), QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRPopplerDocument ();

	/* Reimplemented from NSRAbstractDocument */
	int getPageCount () const;
	NSRRenderInfo renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	QString getText ();
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	NSRAbstractDocument::NSRDocumentStyle getPreferredDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}

	bool hasDynamicPages () const {
		return false;
	}

	NSRTocEntry * getToc () const;

	void setPassword (const QString &passwd);

	inline void setEncoding (const QString& encoding) {
		Q_UNUSED (encoding);
	}

	inline QString getEncoding () const {
		return QString ("UTF-8");
	}

private:
	/**
	 * @brief Creates internal structure
	 * @param passwd File password.
	 */
	void createInternalDoc (QString passwd = QString ());

	/**
	 * @brief Appends TOC entries to parent item
	 * @param parent Parent TOC item.
	 * @param items TOC entries to append.
	 * @since 1.5.2
	 */
	void addTocChildren (NSRTocEntry *parent, GooList *items) const;

	/**
	 * @brief Converts Poppler Unicode string to Qt's one
	 * @param u Unicode string to convert.
	 * @param len String length, in charcters.
	 * @return Converted Qt string.
	 * @since 1.5.2
	 */
	QString unicodeToQString (Unicode* u, int len) const;

	static QMutex		_mutex;		/**< Mutex for global config	*/
	static int		_refcount;	/**< Reference count for config	*/
	static UnicodeMap *	_utf8Map;	/**< Unicode map		*/

	QString		_text;			/**< Page text			*/
	PDFDoc		*_doc;			/**< PDF file handler		*/
	Catalog		*_catalog;		/**< PDF catalog		*/
	Page		*_page;			/**< PDF page			*/
	SplashOutputDev *_dev;			/**< PDF rendering device	*/
};

#endif /* __NSRPOPPLERDOCUMENT_H__ */
