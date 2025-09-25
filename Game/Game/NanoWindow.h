#pragma once

namespace window
{
	bool Init(uint16_t width, uint16_t height, std::string_view title);
	void Close() noexcept;

	bool WindowShouldClose() noexcept;

	void Swap();

	uint16_t GetBufferWidth();
	uint16_t GetBufferHeight();
	uint16_t GetWidth();
	uint16_t GetHeight();

	bool* GetsKeys();
	float GetXChange();
	float GetYChange();

	inline GLFWwindow* windowHandle{ nullptr };


} // namespace window