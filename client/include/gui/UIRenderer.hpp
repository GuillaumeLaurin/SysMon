#pragma once

#include "interfaces/IRenderer.hpp"

#include "interfaces/IPage.hpp"

#include "interfaces/IRouter.hpp"

#include <vector>
#include <memory>

class UIRenderer
{
public:
    explicit UIRenderer(std::shared_ptr<IRenderer> renderer, std::shared_ptr<IRouter> router);
    ~UIRenderer() = default;

    bool Initialize(HWND hwnd);
    void Render();
    void Shutdown();

private:
  std::shared_ptr<IRenderer>          _Renderer;
  std::shared_ptr<IRouter>            _Router;
};