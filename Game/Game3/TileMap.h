#pragma once

enum class TileGeometryType
{
	None,

	FullBox,

	NewBox,
	NewBox2,

	DiagonalLeftForwardBox,
	DiagonalLeftBackBox,
	DiagonalRightForwardBox,
	DiagonalRightBackBox,

	SlopedBoxLeft,
	SlopedBoxRight,
	SlopedBoxForward,
	SlopedBoxBack,

	SlopedBoxLeftBack,
	SlopedBoxBackRight,
	SlopedBoxRightForward,
	SlopedBoxForwardLeft,

	// TODO: stairs
};

struct TileInfo final
{
	bool operator==(const TileInfo&) const noexcept = default;

	TileGeometryType type{ TileGeometryType::None };
	float     height{ 1.0f };
	glm::vec4 color{ 1.0f };
	Texture2D textureFloor;
	Texture2D textureCeil;
	Texture2D textureWall;
};

constexpr size_t NoTile = std::numeric_limits<size_t>::max();

namespace TileBank
{
	size_t AddTileInfo(const TileInfo& temp);

	TileInfo* GetTileInfo(size_t id);
} // namespace TileBank