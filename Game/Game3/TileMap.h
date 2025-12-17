#pragma once

enum class TileGeometryType
{
	None,

	FullBox1,       // бокс 1 х 1    х 1
	FullBox0_25,    // бокс 1 х 0.25 х 1
	FullBox0_50,    // бокс 1 х 0.5  х 1
	FullBox0_75,    // бокс 1 х 0.75 х 1
	FullBox2,       // бокс 1 х 2    х 1
	FullBox1_25,    // бокс 1 х 1.25 х 1
	FullBox1_50,    // бокс 1 х 1.5  х 1
	FullBox1_75,    // бокс 1 х 1.75 х 1

	SlopedBoxLeftFull,
	SlopedBoxLeft0_25,
	SlopedBoxLeft0_50,
	SlopedBoxLeft0_75,

	SlopedBoxRightFull,
	SlopedBoxRight0_25,
	SlopedBoxRight0_50,
	SlopedBoxRight0_75,

	SlopedBoxForwardFull,
	SlopedBoxForward0_25,
	SlopedBoxForward0_50,
	SlopedBoxForward0_75,

	SlopedBoxBackFull,
	SlopedBoxBack0_25,
	SlopedBoxBack0_50,
	SlopedBoxBack0_75,

	SlopedBoxLeftBackFull,
	SlopedBoxLeftBack0_25,
	SlopedBoxLeftBack0_50,
	SlopedBoxLeftBack0_75,

	SlopedBoxBackRightFull,
	SlopedBoxBackRight0_25,
	SlopedBoxBackRight0_50,
	SlopedBoxBackRight0_75,

	SlopedBoxRightForwardFull,
	SlopedBoxRightForward0_25,
	SlopedBoxRightForward0_50,
	SlopedBoxRightForward0_75,

	SlopedBoxForwardLeftFull,
	SlopedBoxForwardLeft0_25,
	SlopedBoxForwardLeft0_50,
	SlopedBoxForwardLeft0_75,

	// TODO: stairs
};

struct TileInfo final
{
	bool operator==(const TileInfo&) const noexcept = default;

	TileGeometryType type{ TileGeometryType::None };
	glm::vec4 color{ 1.0f };
	Texture2D textureFloor;
	Texture2D textureCeil;
	Texture2D textureWall;
};

namespace TileBank
{
	size_t AddTileInfo(const TileInfo& temp);

	TileInfo* GetTileInfo(size_t id);
} // namespace TileBank