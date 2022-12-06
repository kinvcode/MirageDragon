#include "pch.h"
#include "dataStruct.h"

void to_json(json& j, const ROLEINFO& p)
{
	string utf8name = DataConvert::Utf16ToUtf8(p.name);
	j = json{
		{"name", utf8name},
		{"account", p.account},
		{"character", p.character},
		{"advancement", p.advancement},
		{"awakening", p.awakening},
		{"level", p.level},
		{"prestige", p.prestige},
		{"position", p.position}
	};
}

void to_json(json& j, const TOWNCOOR& p)
{
	string utf8name = DataConvert::Utf16ToUtf8(p.name);
	j = json{
		{"name", utf8name},
		{"word", p.word},
		{"area", p.area},
		{"x", p.x},
		{"y", p.y}
	};
}


