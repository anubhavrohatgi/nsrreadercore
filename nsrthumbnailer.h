#ifndef NSRTHUMBNAILER_H_
#define NSRTHUMBNAILER_H_

#include "nsrrenderrequest.h"

#include <QString>

class NSRThumbnailer
{
public:
	static bool isThumbnailExists (const QString& path);
	static void saveThumbnail (const QString&		path,
				   const NSRRenderRequest&	page);
	static void saveThumbnailEncrypted (const QString&	path);
	static QString getThumbnailText (const QString& path);
	static QString getThumnailPath (const QString& path);
	static bool isThumbnailEncrypted (const QString& path);
	static void cleanOldFiles ();
	static void removeThumbnail (const QString& path);
	static bool isThumbnailOutdated (const QString& path);

private:
	static QString filePathToHash (const QString& path);
	static QString getThumbnailPathFromHash (const QString& hash);

	NSRThumbnailer () {}
	~NSRThumbnailer () {}
};

#endif /* NSRTHUMBNAILER_H_ */
