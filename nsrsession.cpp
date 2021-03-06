#include "nsrsession.h"

NSRSession::NSRSession () :
	_file (QString ()),
	_pos (QPointF (0, 0)),
	_textPos (QPointF (0, 0)),
	_zoomGraphic (100.0),
	_rotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0),
	_page (0),
	_zoomText (100),
	_isFitToWidth (false)
{
}

NSRSession::NSRSession (const QString& file,
			int page,
			int zoomText,
			double zoomGraphic,
			bool isFitToWidth,
			const QPointF& pos,
			const QPointF& textPos,
			NSRAbstractDocument::NSRDocumentRotation rotation) :
	_file (file),
	_pos (pos),
	_textPos (textPos),
	_zoomGraphic (zoomGraphic),
	_rotation (rotation),
	_page (page),
	_zoomText (zoomText),
	_isFitToWidth (isFitToWidth)
{
}
