#include "pch.h"
#include "dataStruct.h"

void to_json(json& j, const ROLEINFO& p)
{
	string utf8name = DataConvert::Utf16ToUtf8(p.name);
	j = json{
		{"name", utf8name},
		{"character", p.character},
		{"advancement", p.advancement},
		{"awakening", p.awakening},
		{"level", p.level},
		{"prestige", p.prestige},
		{"position", p.position}
	};
}


