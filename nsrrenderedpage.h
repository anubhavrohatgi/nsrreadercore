#ifndef __NSRRENDEREDPAGE_H__
#define __NSRRENDEREDPAGE_H__

/**
 * @file nsrrenderedpage.h
 * @author Alexander Saprykin
 * @brief Rendered page
 */

#include "nsrrenderrequest.h"
#include "nsrabstractdocument.h"
#include "nsrreadercore_global.h"

#include <QSize>
#include <QPointF>

/**
 * @class NSRRenderedPage nsrrenderedpage.h
 * @brief Class for rendered page
 */
class NSRREADERCORE_SHARED NSRRenderedPage: public NSRRenderRequest
{
public:
	/**
	 * @brief Constructor
	 */
	NSRRenderedPage ();

	/**
	 * @brief Constructor from request
	 * @param req Render request.
	 */
	NSRRenderedPage (const NSRRenderRequest& req);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRRenderedPage ();

	/**
	 * @brief Gets page size
	 * @return Page size, in px.
	 */
	inline QSize getSize () const {
		return QSize (_image.width (), _image.height ());
	}

	/**
	 * @brief Gets rendered image
	 * @return Rendered image.
	 */
	inline NSR_CORE_IMAGE_DATATYPE getImage () const {
		return _image;
	}

	/**
	 * @brief Gets page text
	 * @return Page text.
	 */
	inline QString getText () const {
		return _text;
	}

	/**
	 * @brief Gets last image scroll position
	 * @return Last image scroll position.
	 * @note Useful to store scroll data right in the page object.
	 */
	inline QPointF getLastPosition () const {
		return _lastPos;
	}

	/**
	 * @brief Gets last text view position
	 * @return Last text view position.
	 * @note Useful to store scroll data right in the page object.
	 */
	inline QPointF getLastTextPosition () const {
		return _lastTextPos;
	}

	/**
	 * @brief Gets page zoom value
	 * @return Zoom value, in %.
	 */
	inline double getRenderedZoom () const {
		return _renderedZoom;
	}

	/**
	 * @brief Gets min page zoom value
	 * @return Min zoom value, in %.
	 * @since 1.4.3
	 */
	inline double getMinRenderZoom () const {
		return _minRenderZoom;
	}

	/**
	 * @brief Gets max page zoom value
	 * @return Max zoom value, in %.
	 * @since 1.4.3
	 */
	inline double getMaxRenderZoom () const {
		return _maxRenderZoom;
	}

	/**
	 * @brief Checks whether page was taken from cache
	 * @return True if page was taken from cache, false otherwise.
	 */
	inline bool isCached () const {
		return _cached;
	}

	/**
	 * @brief Checks whether page image is valid (non-empty)
	 * @return True if page image is valid, false otherwise.
	 */
	bool isImageValid () const;

	/**
	 * @brief Checks whether page is empty (no image and text)
	 * @return True if page is empty, false otherwise.
	 */
	bool isEmpty () const;

	/**
	 * @brief Sets page image
	 * @param image Page image.
	 */
	inline void setImage (NSR_CORE_IMAGE_DATATYPE image) {
		_image = image;
	}

	/**
	 * @brief Sets page text
	 * @param text Page text.
	 */
	inline void setText (const QString &text) {
		_text = text;
	}

	/**
	 * @brief Sets last image scroll position
	 * @param pos Last image scroll position.
	 * @note Useful to store scroll data right in the page object.
	 */
	inline void setLastPosition (const QPointF& pos) {
		_lastPos = pos;
	}

	/**
	 * @brief Sets last text view position
	 * @param pos Last text view position.
	 * @note Useful to store scroll data right in the page object.
	 */
	inline void setLastTextPosition (const QPointF& pos) {
		_lastTextPos = pos;
	}

	/**
	 * @brief Sets page zoom value
	 * @param zoom Page zoom value, in %.
	 */
	inline void setRenderedZoom (double zoom) {
		_renderedZoom = zoom;
	}

	/**
	 * @brief Sets page min zoom value
	 * @param minZoom Page min zoom value, in %.
	 * @since 1.4.3
	 */
	inline void setMinRenderZoom (double minZoom) {
		_minRenderZoom = minZoom;
	}

	/**
	 * @brief Sets page max zoom value
	 * @param maxZoom Page max zoom value, in %.
	 * @since 1.4.3
	 */
	inline void setMaxRenderZoom (double maxZoom) {
		_maxRenderZoom = maxZoom;
	}

	/**
	 * @brief Marks page as cached
	 * @param cached Whether page is cached.
	 */
	inline void setCached (bool cached) {
		_cached = cached;
	}

private:
	NSR_CORE_IMAGE_DATATYPE	_image;		/**< Page image			*/
	QString			_text;		/**< Page text			*/
	QPointF			_lastPos;	/**< Last image scroll position	*/
	QPointF			_lastTextPos;	/**< Last text view position	*/
	double			_renderedZoom;	/**< Page zoom value		*/
	double			_minRenderZoom;	/**< Page min zoom value	*/
	double			_maxRenderZoom;	/**< Page max zoom value	*/
	bool			_cached;	/**< Cache flag			*/
};

#endif /* __NSRRENDEREDPAGE_H__ */
