#pragma once

#include "interfaces/IRenderer.hpp"

#include "interfaces/IPage.hpp"

#include <vector>
#include <memory>

class UIRenderer
{
public:
    explicit UIRenderer(std::shared_ptr<IRenderer> renderer);
    ~UIRenderer() = default;

    bool Initialize(HWND hwnd);
    void Render();
    void Shutdown();

    void AddPage(std::shared_ptr<IPage> page);

private:
  std::shared_ptr<IRenderer>          _Renderer;
  std::vector<std::shared_ptr<IPage>> _Pages;
};