#ifndef GAME_CLIENT_COMPONENTS_SKINS_H
#define GAME_CLIENT_COMPONENTS_SKINS_H
#include <base/vmath.h>
#include <base/tl/sorted_array.h>
#include <game/client/component.h>

class CSkins : public CComponent
{
public:
	// do this better and nicer
	struct CSkin
	{
		int m_OrgTexture;
		int m_ColorTexture;
		char m_aName[24];
		vec3 m_BloodColor;

		bool operator<(const CSkin &Other) { return str_comp(m_aName, Other.m_aName) < 0; }
	};
	
	void Init();
	
	vec4 GetColor(int v);
	int Num();
	const CSkin *Get(int Index);
	int Find(const char *pName);
	
	static vec3 HslToRgb(vec3 in);
	static float HueToRgb(float v1, float v2, float h);
	
private:
	sorted_array<CSkin> m_aSkins;

	static void SkinScan(const char *pName, int IsDir, int DirType, void *pUser);
};
#endif
