/**
 * This file defines the trackball class, which is independent to the project itself.
 */
#pragma once
#ifndef _OTV_TRACKBALL_H_
#define _OTV_TRACKBALL_H_

#include "otv_common.h"

namespace otv {
	//
	// following the implementation of http://image.diku.dk/research/trackballs/index.html
	//
	class Trackball {
	private:
		float radius = 1.0f;
		float rate = 200.0f;
		GLboolean trackball_mode = true;
		GLboolean inverse_mode = false;
		Trackball *coord = nullptr;
		cy::Matrix4f matrix = cy::Matrix4f::MatrixIdentity();
		cy::Matrix4f matrix_prev = cy::Matrix4f::MatrixIdentity();
		cy::Point3f  position;
		cy::Point3f  position_prev;
		cy::Matrix4f view;
		cy::Matrix4f view_prev;
		float position_surf[2];
	public:
		/** constractors */
		Trackball() {}
		Trackball(bool i, bool n, Trackball* worldcoord = nullptr) :
			inverse_mode(i),
			trackball_mode(n),
			coord(worldcoord)
		{}

		void SetRadius(const float r) { radius = r; }
		void SetRate(const float r) { rate = r; }
		void SetInverseMode(bool r) { inverse_mode = r; }

		/**
		* @brief ChangeMode: to switch between FPS mode and Trackball mode
		*/
		void ChangeMode() {
			trackball_mode = !trackball_mode;
			if (trackball_mode) {
				std::cout << " -- Trackball Mode --" << std::endl;
			}
			else {
				std::cout << " -- FPS Camera Mode --" << std::endl;
			}
		}

		/**
		* @brief BeginDrag/Zoom: initialize drag/zoom
		* @param x: previous x position
		* @param y: previous y position
		*/
		void BeginDrag(float x, float y)
		{
			position_prev = proj2surf(x, y);
			position_surf[0] = x;
			position_surf[1] = y;
			matrix_prev = matrix;
		}

		/**
		* @brief Drag/Zoom: execute Drag/Zoom
		* @param x: current x position
		* @param y: current y position
		*/
		void Drag(float x, float y)
		{
			cy::Matrix4f rot;
			if (trackball_mode) {
				// get direction
				position = proj2surf(x, y);
				cy::Point3f dir = (position_prev.Cross(position)).GetNormalized();
				// modify 
				dir = inverse_mode ? -dir : dir;
				dir = coord == nullptr ? dir : coord->Matrix().GetSubMatrix3().GetInverse() * dir;
				// compute rotation angle
				float product = position_prev.Dot(position);
				float angle = product / position_prev.Length() / position.Length();
				if (angle > 0.99999999999) { // to prevent position_prev == position, this will cause invalid value
					return;
				}
				else { // compute rotation
					rot.SetRotation(dir, acos(angle));
				}
			}
			else {
				float alphaX = +180 * (x - position_prev[0]) / 200.0f;
				float alphaY = -180 * (y - position_prev[1]) / 200.0f;
				rot = cy::Matrix4f::MatrixRotationX(alphaY) * cy::Matrix4f::MatrixRotationY(alphaX);
			}
			matrix = rot * matrix_prev;
		}


		/**
		 * @brief matrix: trackball matrix accessor
		 * @return current trackball matrix
		 */
		cy::Matrix4f Matrix()
		{
			return matrix;
		}

		void Reset() { matrix = cy::Matrix4f::MatrixIdentity(); }

	private:
		/**
		 * @brief proj2surf: project (x,y) mouse pos on surface
		 * @param x: X position
		 * @param y: Y position
		 * @return projected position
		 */
		cy::Point3f proj2surf(const float x, const float y) const
		{
			float r = x * x + y * y;
			float R = radius * radius;
			float z = r > R / 2 ? R / 2 / sqrt(r) : sqrt(R - r);
			return cy::Point3f(x, y, z);
		}

	};
};

#endif//_OTV_TRACKBALL_H_
