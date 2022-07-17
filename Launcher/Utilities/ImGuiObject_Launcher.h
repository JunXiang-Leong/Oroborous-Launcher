#pragma once
#include <string>
#include <functional>
#include <imgui/imgui.h>



class ImGuiObject_Launcher
{
public:
	ImGuiObject_Launcher(const bool enable,const ImGuiWindowFlags_ flag,std::function<void()>fnc)
		:m_enabled{ enable }, m_flags{ flag }, m_UIupdate{ fnc } {};
	std::function<void()> m_UIupdate;
	ImGuiWindowFlags_ m_flags;
	bool m_enabled = true;
private:
};
