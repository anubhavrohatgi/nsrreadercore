#include "nsrpagecropper.h"

#include <qmath.h>

#define NSR_CORE_CROP_PIXEL_THRESHOLD	2
#define NSR_CORE_CROP_MAX_PAD_PERCENT	0.15
#define NSR_CORE_CROP_AGGRESSIVE_VALUE	0.15

NSRPageCropper::NSRPageCropper ()
{
}

NSRCropPads
NSRPageCropper::findCropPads (unsigned char *data,
			      NSRPixelOrder order,
			      int width,
			      int height,
			      int stride,
			      int widthLimit)
{
	NSRCropPads	pads;
	unsigned char *	dataPtr = data;
	double		rDelta, gDelta, bDelta;
	int		badCount = 0;
	int		px;			/* Bytes in pixel 					*/
	int		pxShift;		/* Shift in bytes to the first color component in pixel	*/

	if (data == NULL || stride < width * 3 || width < 3 || height < 3)
		return pads;

	if (width <= widthLimit)
		return pads;

	int maxWCrop = (int) (width * NSR_CORE_CROP_MAX_PAD_PERCENT);
	int maxHCrop = (int) (height * NSR_CORE_CROP_MAX_PAD_PERCENT);

	int top    = 0;
	int bottom = 0;
	int left   = 0;
	int right  = 0;

	switch (order) {
	case NSR_PIXEL_ORDER_RGB:
	case NSR_PIXEL_ORDER_BGR:
		px = 3;
		pxShift = 0;
		break;
	case NSR_PIXEL_ORDER_ARGB:
		px = 4;
		pxShift = 1;
		break;
	case NSR_PIXEL_ORDER_BGRA:
	case NSR_PIXEL_ORDER_RGBA:
		px = 4;
		pxShift = 0;
		break;
	default:
		px = 0;
		pxShift = 0;
	}

	if (px == 0 && pxShift == 0)
		return pads;

	/* Use mean of two corner pixels as background */
	double red = (dataPtr[pxShift] +
		      dataPtr[(width - 1) * px + pxShift] +
		      dataPtr[stride * (height - 1) + pxShift] +
		      dataPtr[stride * (height - 1) + (width - 1) * px + pxShift]) / 4.0;
	double green = (dataPtr[pxShift + 1] +
			dataPtr[(width - 1) * px + pxShift + 1] +
			dataPtr[stride * (height - 1) + pxShift + 1] +
			dataPtr[stride * (height - 1) + (width - 1) * px + pxShift + 1]) / 4.0;
	double blue = (dataPtr[pxShift + 2] +
		       dataPtr[(width - 1) * px + pxShift + 2] +
		       dataPtr[stride * (height - 1) + pxShift + 2] +
		       dataPtr[stride * (height - 1) + (width - 1) * px + pxShift + 2]) / 4.0;

	/* First, find top crop pad */
	for (int row = 0; row < maxHCrop; ++row) {
		for (int col = 0; col < width; ++col) {
			rDelta = (red - dataPtr[col * px + pxShift]) / red;
			gDelta = (green - dataPtr[col * px + pxShift + 1]) / green;
			bDelta = (blue - dataPtr[col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CORE_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE))
				++badCount;
		}

		if (badCount > NSR_CORE_CROP_PIXEL_THRESHOLD) {
			top = (row + 1);
			break;
		} else if (row == maxHCrop - 1)
			top = (row + 1);

		dataPtr += stride;
	}

	/* Next, find bottom crop pad */

	dataPtr = data + stride * (height - 1);
	badCount = 0;

	for (int row = height - 1; row >= height - maxHCrop; --row) {
		for (int col = 0; col < width; ++col) {
			rDelta = (red - dataPtr[col * px + pxShift]) / red;
			gDelta = (green - dataPtr[col * px + pxShift + 1]) / green;
			bDelta = (blue - dataPtr[col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CORE_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE))
				++badCount;
		}

		if (badCount > NSR_CORE_CROP_PIXEL_THRESHOLD) {
			bottom = (height - row - 1);
			break;
		} else if (row == height - maxHCrop)
			bottom = (height - row - 1);

		dataPtr -= stride;
	}

	/* Next, find left crop pad */

	dataPtr = data;
	badCount = 0;

	for (int col = 0; col < maxWCrop; ++col) {
		for (int row = 0; row < height; ++row) {
			rDelta = (red - dataPtr[row * stride + col * px + pxShift]) / red;
			gDelta = (green - dataPtr[row * stride + col * px + pxShift + 1]) / green;
			bDelta = (blue - dataPtr[row * stride + col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CORE_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE))
				++badCount;		}

		if (badCount > NSR_CORE_CROP_PIXEL_THRESHOLD) {
			left = (col + 1);
			break;
		} else if (col == maxWCrop - 1)
			left = (col + 1);
	}

	/* Finally, find right crop pad */

	dataPtr = data;
	badCount = 0;

	for (int col = width - 1; col >= width - maxWCrop; --col) {
		for (int row = 0; row < height; ++row) {
			rDelta = (red - dataPtr[row * stride + col * px + pxShift]) / red;
			gDelta = (green - dataPtr[row * stride + col * px + pxShift + 1]) / green;
			bDelta = (blue - dataPtr[row * stride + col * px + pxShift + 2]) / blue;

			if (fabs (rDelta) > NSR_CORE_CROP_AGGRESSIVE_VALUE ||
			    fabs (gDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE) ||
			    fabs (bDelta > NSR_CORE_CROP_AGGRESSIVE_VALUE))
				++badCount;
		}

		if (badCount > NSR_CORE_CROP_PIXEL_THRESHOLD) {
			right = (width - col - 1);
			break;
		} else if (col == width - maxWCrop)
			right = (width - col - 1);
	}

	top    = (qMax (top - 5, 0));
	right  = (qMax (right - 5, 0));
	bottom = (qMax (bottom - 5, 0));
	left   = (qMax (left - 5, 0));

	if (widthLimit > 0 && (width - left - right) < widthLimit) {
		double limitDelta = width - widthLimit;

		double leftPadRatio = left / (double) (left + right);
		double rightPadRatio = right / (double) (left + right);

		left  = (int) (leftPadRatio * limitDelta + 0.5);
		right = (int) (rightPadRatio * limitDelta + 0.5);
	}

	pads.setTop ((double) top / height);
	pads.setBottom ((double) bottom / height);
	pads.setLeft ((double) left / width);
	pads.setRight ((double) right / width);
	pads.setDetected (true);

	return pads;
}
