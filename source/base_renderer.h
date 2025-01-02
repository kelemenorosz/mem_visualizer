#pragma once

#include <windows.h>

class Base_Renderer {

	public:

		virtual ~Base_Renderer() {};
		virtual void OnRender() = 0;
		virtual void OnUpdate(double value) = 0;
		virtual void OnMouseButtonLeftMove(WORD x1, WORD x2, WORD y1, WORD y2) = 0;
		virtual void OnMouseWheel(float value) = 0;
		virtual void OnResize(int width, int height) = 0;
		virtual void OnKeyPress(unsigned __int8 scancode, unsigned __int8 is_repeat) = 0;

	private:


};