#include "nsrrenderedpage.h"

NSRRenderedPage::NSRRenderedPage (QObject *parent) :
	QObject (parent),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0),
	_number (0),
	_cropped (false),
	_cached (false)
{
}

NSRRenderedPage::NSRRenderedPage (int number, QObject *parent) :
	QObject (parent),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0),
	_number (number),
	_cropped (false),
	_cached (false)
{
}

NSRRenderedPage::NSRRenderedPage (int number, NSRRenderReason reason, QObject *parent) :
		QObject (parent),
		_reason (reason),
		_zoom (0),
		_number (number),
		_cropped (false),
		_cached (false)
{
}

NSRRenderedPage::NSRRenderedPage (const NSRRenderedPage& page) :
	QObject (page.parent ())
{
	_reason		= page._reason;
	_image		= page._image;
	_zoom		= page._zoom;
	_number		= page._number;
	_text		= page._text;
	_lastPos	= page._lastPos;
	_lastTextPos	= page._lastTextPos;
	_cropped	= page._cropped;
	_cached		= page._cached;
}

NSRRenderedPage::~NSRRenderedPage ()
{
}

NSRRenderedPage&
NSRRenderedPage::operator = (const NSRRenderedPage& page)
{
	if (this != &page) {
		_reason		= page._reason;
		_image		= page._image;
		_zoom		= page._zoom;
		_number		= page._number;
		_text		= page._text;
		_lastPos	= page._lastPos;
		_lastTextPos	= page._lastTextPos;
		_cropped	= page._cropped;
		_cached		= page._cached;
	}

	return *this;
}

NSRRenderedPage::NSRRenderReason
NSRRenderedPage::getRenderReason () const
{
	return _reason;
}

int
NSRRenderedPage::getNumber () const
{
	return _number;
}

double
NSRRenderedPage::getZoom () const
{
	return _zoom;
}

QSize
NSRRenderedPage::getSize () const
{
	return QSize (_image.width (), _image.height ());
}

NSR_CORE_IMAGE_DATATYPE
NSRRenderedPage::getImage () const
{
	return _image;
}

QString
NSRRenderedPage::getText () const
{
	return _text;
}

QPointF
NSRRenderedPage::getLastPosition () const
{
	return _lastPos;
}

QPointF
NSRRenderedPage::getLastTextPosition () const
{
	return _lastTextPos;
}

bool
NSRRenderedPage::isValid () const
{
	return _number > 0;
}

bool
NSRRenderedPage::isImageValid () const
{
#ifdef Q_OS_BLACKBERRY
	return _number > 0 && _image.isValid ();
#else
	return _number > 0 && !_image.isNull ();
#endif
}

bool

NSRRenderedPage::isEmpty () const
{
#ifdef Q_OS_BLACKBERRY
	return _text.isEmpty () && !_image.isValid ();
#else
	return _text.isEmpty () && _image.isNull ();
#endif
}

bool
NSRRenderedPage::isCropped () const
{
	return _cropped;
}

bool
NSRRenderedPage::isCached () const
{
	return _cached;
}

void
NSRRenderedPage::setRenderReason (NSRRenderedPage::NSRRenderReason reason)
{
	_reason = reason;
}

void
NSRRenderedPage::setNumber (int number)
{
	_number = number;
}

void
NSRRenderedPage::setZoom (double zoom)
{
	if (zoom < 0)
		zoom = 0;

	_zoom = zoom;
}

void
NSRRenderedPage::setImage (NSR_CORE_IMAGE_DATATYPE img)
{
	_image = img;
}

void
NSRRenderedPage::setText (const QString& text)
{
	_text = text;
}

void
NSRRenderedPage::setLastPosition (const QPointF& pos)
{
	_lastPos = pos;
}

void
NSRRenderedPage::setLastTextPosition (const QPointF& pos)
{
	_lastTextPos = pos;
}

void
NSRRenderedPage::setCropped (bool cropped)
{
	_cropped = cropped;
}

void
NSRRenderedPage::setCached (bool cached)
{
	_cached = cached;
}
