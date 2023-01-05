#pragma once

struct RenderData;
struct Init;

class ImGuiVulkan
{
public:
	static int SetUpImgui(Init& init, RenderData& data);
	static int UpdateImgui(Init& init, RenderData& data);
	static void DestroyImgui(Init& init, RenderData& data);
};
