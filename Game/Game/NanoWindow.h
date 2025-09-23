#pragma once

namespace window
{
	bool Init(uint16_t width, uint16_t height, std::string_view title);
	void Close() noexcept;

	bool WindowShouldClose() noexcept;

	void Swap();

	inline GLFWwindow* windowHandle{ nullptr };

} // namespace window