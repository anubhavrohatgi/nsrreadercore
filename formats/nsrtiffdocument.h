#ifndef __NSRTIFFDOCUMENT_H__
#define __NSRTIFFDOCUMENT_H__

/**
 * @file nsrtiffdocument.h
 * @author Alexander Saprykin
 * @brief TIFF file handler
 */

#include "nsrabstractdocument.h"

#include "tiff/tiffio.h"

/**
 * @class NSRTIFFDocument nsrtiffdocument.h
 * @brief Class for TIFF file handler
 */
class NSRTIFFDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	/**
	 * @brief Constructor with parameters
	 * @param file Path to file.
	 * @param parent Parent object.
	 */
	NSRTIFFDocument (const QString& file, QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRTIFFDocument ();

	/* Reimplemented from NSRAbstractDocument */
	int getPageCount () const;
	NSRRenderInfo renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;

	NSRAbstractDocument::NSRDocumentStyle getPreferredDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}

	bool hasDynamicPages () const {
		return false;
	}

	NSRTocEntry * getToc () const {
		return NULL;
	}

	QString getPassword () const {
		return QString ();
	}

	void setPassword (const QString& passwd) {
		Q_UNUSED (passwd);
	}

	QString getEncoding () const {
		return QString ();
	}

	void setEncoding (const QString& encoding) {
		Q_UNUSED (encoding);
	}

private:
	/**
	 * @brief Rotates image right and mirrors horizontal
	 * @param[in, out] image Image to transform and transformed one.
	 * @param[out] buf Pointer to image data buffer.
	 */
	void rotateRightMirrorHorizontal (QImage ** image, char **buf);

	/**
	 * @brief Rotates image right and mirrors vertical
	 * @param[in, out] image Image to transform and transformed one.
	 * @param[out] buf Pointer to image data buffer.
	 */
	void rotateRightMirrorVertical (QImage ** image, char **buf);

	QImage			_origImage;		/**< Original rendered image	*/
	QImage			_image;			/**< Transformed original image	*/
	TIFF			*_tiff;			/**< File handler		*/
	int			_pagesCount;		/**< Pages count		*/
	int			_cachedPage;		/**< Cached page number		*/
};

#endif /* __NSRTIFFDOCUMENT_H__ */
