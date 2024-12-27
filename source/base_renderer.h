#pragma once

#include <windows.h>

class Base_Renderer {

	public:

		virtual ~Base_Renderer() {};
		virtual void OnRender() = 0;
		virtual void OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2) = 0;
		virtual void OnMouseWheel(float value) = 0;

	private:


};