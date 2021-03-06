#ifndef __NSRREGIONTEXT_H__
#define __NSRREGIONTEXT_H__

/**
 * @file nsrregiontext.h
 * @author Alexander Saprykin
 * @brief Text page region
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrwordwithcharacters.h"

#include <QList>

/**
 * @class NSRRegionText nsrregiontext.h
 * @brief Text page region
 * @since 1.5.1
 *
 * We will divide the whole page in some regions depending on the horizontal and
 * vertical spacing among different regions. Each region will have an area and an
 * associated #NSRWordWithCharactersList in sorted order.
 *
 * This class acts like a light wrapper around given word list. It doesn't manage
 * the memory allocation, so you should manually free the memory allocated
 * for given word list.
 */
class NSRRegionText
{
public:
	/** Creates a null text region */
	NSRRegionText ();

	/**
	 * @brief Creates text region
	 * @param wordsWithCharacters List of words with characters.
	 * @param area Area of the text region.
	 */
	NSRRegionText (const NSRWordWithCharactersList& wordsWithCharacters, const QRect& area);

	/** Destructor */
	~NSRRegionText ();

	/**
	 * @brief Gets the whole text as a string
	 * @return The whole text as a string.
	 * @since 1.5.1
	 */
	QString getString () const;

	/**
	 * @brief Gets list of words with characters
	 * @return List of words with characters.
	 * @since 1.5.1
	 */
	inline NSRWordWithCharactersList getText () const {
		return _words;
	}

	/**
	 * @brief Gets text region area
	 * @return Text region area.
	 * @since 1.5.1
	 */
	inline QRect getArea () const {
		return _area;
	}

	/**
	 * @brief Sets list of words with characters
	 * @param wordsWithCharacters List of words with characters.
	 * @since 1.5.1
	 */
	inline void setText (const NSRWordWithCharactersList& wordsWithCharacters) {
		_words = wordsWithCharacters;
	}

	/**
	 * @brief Sets text region area
	 * @param area Text region area.
	 * @since 1.5.1
	 */
	inline void setArea (const QRect& area) {
		_area = area;
	}

private:
	NSRWordWithCharactersList	_words;	/**< List of words with characters	*/
	QRect				_area;	/**< Text region area			*/
};

/** A list of #NSRRegionText, it keeps a bunch of #NSRTinyTextEntityList with their bounding rectangles */
typedef QList<NSRRegionText> NSRRegionTextList;

#endif /* __NSRREGIONTEXT_H__ */
