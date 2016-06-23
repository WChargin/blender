// Copyright (c) 2011, 2016 libmv authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Author: Tianwei Shen <shentianweipku@gmail.com>
//
// This is a autotrack equivalent bundle set, adapted from simple_pipeline,
// which replaces libmv with mv, includeing tracks and markers

#ifndef LIBMV_AUTOTRACK_BUNDLE_H
#define LIBMV_AUTOTRACK_BUNDLE_H

#include "libmv/numeric/numeric.h"
#include "libmv/autotrack/tracks.h"
#include "libmv/autotrack/reconstruction.h"
#include "libmv/simple_pipeline/callbacks.h"
#include "libmv/simple_pipeline/camera_intrinsics.h"

using libmv::Mat;
using libmv::CameraIntrinsics;
using mv::Tracks;
using mv::Reconstruction;

namespace mv {

class EuclideanReconstruction;
class ProjectiveReconstruction;

struct BundleEvaluation {
	BundleEvaluation() :
	    num_cameras(0),
	    num_points(0),
	    evaluate_jacobian(false) {
	}

	int num_cameras;		/* Number of cameras appeared in bundle adjustment problem */
	int num_points;			/* Number of points appeared in bundle adjustment problem */

	/* When set to truth, jacobian of the problem after optimization will
	 * be evaluated and stored in \parameter jacobian */
	bool evaluate_jacobian;

	// Contains evaluated jacobian of the problem.
	// Parameters are ordered in the following way:
	//   - Intrinsics block
	//   - Cameras (for each camera rotation goes first, then translation)
	//   - Points
	Mat jacobian;
};

/*!
	Refine camera poses and 3D coordinates using bundle adjustment.

	This routine adjusts all cameras positions, points, and the camera
	intrinsics (assumed common across all images) in \a *reconstruction. This
	assumes a full observation for reconstructed tracks; this implies that if
	there is a reconstructed 3D point (a bundle) for a track, then all markers
	for that track will be included in the minimization. \a tracks should
	contain markers used in the initial reconstruction.

	The cameras, bundles, and intrinsics are refined in-place.

	Constraints denotes which blocks to keep constant during bundling.
	For example it is useful to keep camera translations constant
	when bundling tripod motions.

	If evaluaiton is not null, different evaluation statistics is filled in
	there, plus all the requested additional information (like jacobian) is
	also calculating there. Also see comments for BundleEvaluation.

	\note This assumes an outlier-free set of markers.

	\sa EuclideanResect, EuclideanIntersect, EuclideanReconstructTwoFrames
*/
enum BundleIntrinsics {
	BUNDLE_NO_INTRINSICS = 0,
	BUNDLE_FOCAL_LENGTH = 1,
	BUNDLE_PRINCIPAL_POINT = 2,
	BUNDLE_RADIAL_K1 = 4,
	BUNDLE_RADIAL_K2 = 8,
	BUNDLE_RADIAL = 12,
	BUNDLE_TANGENTIAL_P1 = 16,
	BUNDLE_TANGENTIAL_P2 = 32,
	BUNDLE_TANGENTIAL = 48,
};
enum BundleConstraints {
	BUNDLE_NO_CONSTRAINTS = 0,
	BUNDLE_NO_TRANSLATION = 1,
};

void EuclideanBundleCommonIntrinsics(
        const Tracks &tracks,
        const int bundle_intrinsics,
        const int bundle_constraints,
        Reconstruction *reconstruction,
        CameraIntrinsics *intrinsics,
        BundleEvaluation *evaluation = NULL);

/*! Refine all camera poses and 3D coordinates from all clips using bundle adjustment.
	This is a renewed version for autotrack, adapted from libmv/simple_pipeline

    This routine adjusts all cameras and points in \a *reconstruction. This
    assumes a full observation for reconstructed tracks; this implies that if
    there is a reconstructed 3D point (a bundle) for a track, then all markers
    for that track will be included in the minimization. \a tracks should
    contain markers used in the initial reconstruction.

    The cameras and bundles (3D points) are refined in-place.

    \note This assumes an outlier-free set of markers.
    \note This assumes a calibrated reconstruction, e.g. the markers are
          already corrected for camera intrinsics and radial distortion.

    \sa EuclideanResect, EuclideanIntersect, EuclideanReconstructTwoFrames
*/
bool EuclideanBundleAll(const Tracks &tracks,
                        Reconstruction *reconstruction);

}  // namespace mv

#endif   // LIBMV_AUTOTRACK_BUNDLE_H
