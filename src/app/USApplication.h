#pragma once

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Complex.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/DistanceFieldVector.h>
#include <Magnum/Text/AbstractFont.h>
#include <Magnum/Text/DistanceFieldGlyphCache.h>
#include <Magnum/Text/Renderer.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Timeline.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/ImageView.h>

#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/GL/TextureFormat.h>

#include <utility>
#include <string>

#include "navigation/MouseMoveHelper.hpp"
#include "navigation/MapScaleHelper.hpp"
#include "navigation/KeyboardMoveHelper.hpp"

#include "navigation/base/states/MouseMovements.h"

#include "navigation/shaders/selectRect/SelectionRectState.h"
#include "navigation/shaders/grid/Grid.hpp"

class USApplication: public Magnum::Platform::Application {
 public:
    explicit USApplication(const Arguments& arguments);

 private:
    void viewportEvent(ViewportEvent& event) override;
    void drawEvent() override;
    void mouseScrollEvent(MouseScrollEvent& event) override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;

    void mousePressEvent(MouseEvent& event) override;
    void mouseReleaseEvent(MouseEvent& event) override;
    void mouseMoveEvent(MouseMoveEvent& event) override;

    // left click
      bool _lMousePressed = false;
      bool _lMousePressedMoved = false;
      std::chrono::system_clock::time_point _lMousePressedT;

    // right click
      bool _rMousePressed = false;

    void _updateProjections();
    void _updateDebugText();
    void _resetWorldMatrix();

    Magnum::PluginManager::Manager<Magnum::Text::AbstractFont> _fontManager;
    Magnum::Containers::Pointer<Magnum::Text::AbstractFont> _font;

    Magnum::Text::DistanceFieldGlyphCache _cache;
    Magnum::GL::Mesh _worldText{Magnum::NoCreate};
    Magnum::GL::Buffer _textVertices, _textIndices;
    Magnum::Containers::Pointer<Magnum::Text::Renderer2D> _debugText, _shortcutsText;
    Magnum::Shaders::DistanceFieldVector2D _textShader;

    Magnum::GL::Texture2D _gridTexture;
    Grid _gridShader{Magnum::NoCreate};
    Magnum::GL::Mesh _grid{Magnum::GL::MeshPrimitive::Triangles};
    void _defineGrid(Magnum::Utility::Resource &rs);

    SelectionRect _selectRectShader{Magnum::NoCreate};
    Magnum::GL::Buffer _selectRectBuffer;
    Magnum::GL::Mesh _selectRect{Magnum::GL::MeshPrimitive::Triangles};
    SelectionRectState _srs;
    void _defineSelectionRect();

    Magnum::Matrix3 _transformationWorld,
                    _projectionWorld,
                    _transformationProjectionShortcutsText,
                    _transformationProjectionDebugText;

    static const int DOUBLE_CLICK_DELAY_MS = 200;

    Magnum::Timeline _timeline;
    MouseMoveHelper _mmh;
    MapScaleHelper _msh;
    KeyboardMoveHelper _kmh;
};
