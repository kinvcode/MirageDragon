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

void from_json(const json& j, TOWNCOOR& p)
{
	p.name = DataConvert::Utf8ToUtf16(j["name"]).c_str();
	j.at("word").get_to(p.word);
	j.at("area").get_to(p.area);
	j.at("x").get_to(p.x);
	j.at("y").get_to(p.y);
}

void from_json(const json& j, DUNGEONJOB& p)
{
	j.at("dungeon_code").get_to(p.dungeon_code);
	j.at("times").get_to(p.times);
}

void to_json(json& j, const DUNGEONJOB& p)
{
	j = json{
	{"times", p.times},
	{"dungeon_code", p.dungeon_code}
	};
}