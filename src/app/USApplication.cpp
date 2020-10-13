// UnderStory
// An intuitive Pen & Paper experience
// Copyright (C) 2020 Guillaume Vara

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Graphical resources without explicit references to a
// different license and copyright still refer to this GPL.

// TODO grid precision, antialiasing further

#include "USApplication.h"

using namespace Magnum::Math::Literals;

UnderStory::USApplication::USApplication(const Arguments& arguments): Magnum::Platform::Application{
        arguments,
        Configuration{}.setTitle(APP_FULL_DENOM)
                       .setWindowFlags(Configuration::WindowFlag::Resizable),
        GLConfiguration{}
    },
    _cache{Magnum::Vector2i{2048}, Magnum::Vector2i{512}, 22},
    _mmh(&_timeline, &_transformationWorld),
    _msh(&_timeline, &_transformationWorld),
    _kmh(&_timeline, &_transformationWorld) {
    //
    _updateChecker.start();

    // define animation mutual exclusions
    _kmh.setExcludedWhenPlaying({&_mmh});
    _mmh.setExcludedWhenPlaying({&_kmh});

    // activate VSync
    setSwapInterval(-1);

    // define
    Magnum::Utility::Resource rs("data");
    _defineSelectionRect(rs);
    _defineGrid(rs);

    /* Load a TrueTypeFont plugin and open the font */
    _font = _fontManager.loadAndInstantiate("TrueTypeFont");
    if(!_font || !_font->openData(rs.getRaw("SourceSansPro-Regular.ttf"), 180.0f))
        Magnum::Fatal{} << "Cannot open font file";

    /* Glyphs we need to render everything */
    _font->fillGlyphCache(_cache,
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789:-+,.!°()/[] ");

    /* Text that rotates using mouse wheel. Size relative to the window size
       (1/10 of it) -- if you resize the window, it gets bigger */
    std::tie(_worldText, std::ignore) = Magnum::Text::Renderer2D::render(*_font, _cache, 0.2f,
        "Welcome to the Understory developper early demo !\n"
        "This is a placeholder meant to showcase the text displaying capabilities.\n"
        "Thank you for trying this early stage build and supporting our efforts !\n"
        "More infos available on our Discord server and Github page (https://github.com/Amphaal/Understory)",
        _textVertices, _textIndices, Magnum::GL::BufferUsage::StaticDraw, Magnum::Text::Alignment::MiddleCenter);

    // define debug text as dynamic
    _debugText.reset(new Magnum::Text::Renderer2D(*_font, _cache, 24.0f, Magnum::Text::Alignment::TopLeft));
    _debugText->reserve(100, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);

    // define shortcut text as static
    const std::string sText = "[D-pad] move camera\n"
                "[Esc] reset camera\n"
                "[MouseScroll] zoom-in on cursor / zoom out\n"
                "[NumPad +/-] inplace zoom\n"
                "[LeftClick (Double click)] center screen on cursor\n"
                "[LeftClick (Maintain)] move camera\n"
                "[RightClick (Maintain/Release)] Snapshot zoom-in";
    _shortcutsText.reset(new Magnum::Text::Renderer2D(*_font, _cache, 18.0f, Magnum::Text::Alignment::TopRight));
    _shortcutsText->reserve(sText.size(), Magnum::GL::BufferUsage::StaticDraw, Magnum::GL::BufferUsage::StaticDraw);
    _shortcutsText->render(sText);

    /* Set up premultiplied alpha blending to avoid overlapping text characters
       to cut into each other */
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);

    //
    _updateProjections();
    _updateDebugText();

    //
    _timeline.start();
}

void UnderStory::USApplication::_defineSelectionRect(const Magnum::Utility::Resource &rs) {
    // compile shader
    _selectRectShader = Shader::SelectionRect{rs};

    // define indices
    Magnum::GL::Buffer indices;
    indices.setData({
        0, 1, 2,
        0, 2, 3
    });

    // set state tracker
    _srs = Navigation::SelectionRectState(&_selectRectShader, &_selectRectBuffer);

    // bind buffer
    _selectRectBuffer.setData(_srs.vertexes(), Magnum::GL::BufferUsage::DynamicDraw);

    // define mesh
    _selectRect.setCount(indices.size())
            .addVertexBuffer(_selectRectBuffer, 0, Magnum::Shaders::Flat2D::Position{})
            .setIndexBuffer(std::move(indices), 0, Magnum::MeshIndexType::UnsignedInt);
}

void UnderStory::USApplication::_defineGrid(const Magnum::Utility::Resource &rs) {
    // load texture
    Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;
    auto importer = manager.loadAndInstantiate("PngImporter");
    if(!importer) std::exit(1);
    if(!importer->openData(rs.getRaw("grid.png"))) std::exit(2);

    // set texture
    auto image = importer->image2D(0);
    auto levels = Magnum::Math::log2(image->size().x()) + 1;
    CORRADE_INTERNAL_ASSERT(image);
    _gridTexture
        .setWrapping(Magnum::GL::SamplerWrapping::Repeat)
        .setMagnificationFilter(Magnum::GL::SamplerFilter::Nearest)
        .setMinificationFilter(Magnum::GL::SamplerFilter::Nearest)
        .setMaxAnisotropy(Magnum::GL::Sampler::maxMaxAnisotropy())
        .setStorage(levels, Magnum::GL::textureFormat(image->format()), image->size())
        .setSubImage(0, {}, *image)
        .generateMipmap();

    // compile shader
    _gridShader = Shader::Grid{rs};

    // define indices
    Magnum::GL::Buffer indices;
    indices.setData({
        0, 1, 2,
        2, 3, 0
    });

    // define data and structure
    auto squareSize = Magnum::Vector2 {image->size()} * (1.f / static_cast<float>(framebufferSize().y()));
    auto squareCount = 1000.f;  // CAREFUL, higher need a better precision
    auto gridRadius = squareSize * squareCount / 2.f;
    const Shader::Grid::Vertex gridVData[]{
        { {-gridRadius.x(),  gridRadius.y()}, {.0f,        squareCount} },
        { { gridRadius.x(),  gridRadius.y()}, {squareCount, squareCount} },
        { { gridRadius.x(), -gridRadius.y()}, {squareCount, .0f} },
        { {-gridRadius.x(), -gridRadius.y()}, {.0f,        .0f} }
    };

    // bind to buffer
    Magnum::GL::Buffer gridVertices;
    gridVertices.setData(gridVData);

    // define mesh
    _grid.setCount(indices.size())
        .addVertexBuffer(std::move(gridVertices), 0, Shader::Grid::Position{}, Shader::Grid::TextureCoordinates{})
        .setIndexBuffer (std::move(indices),      0, Magnum::MeshIndexType::UnsignedInt);
}

void UnderStory::USApplication::_updateProjections() {
    auto ws = Magnum::Vector2{windowSize()};

    _projectionWorld = Magnum::Matrix3::projection(
        Magnum::Vector2::xScale(ws.aspectRatio())
    );

    // stick to top left corner
    _transformationProjectionDebugText =
        Magnum::Matrix3::projection(ws) *
        Magnum::Matrix3::translation(ws * Magnum::Vector2::xAxis(-.5f)) *
        Magnum::Matrix3::translation(ws * Magnum::Vector2::yAxis(.5f)
    );

    // stick to top right corner
    _transformationProjectionShortcutsText =
        Magnum::Matrix3::projection(ws) *
        Magnum::Matrix3::translation(ws * Magnum::Vector2::xAxis(.5f)) *
        Magnum::Matrix3::translation(ws * Magnum::Vector2::yAxis(.5f)
    );

    //
    _srs.onFramebufferChange();
}

void UnderStory::USApplication::viewportEvent(ViewportEvent& event) {
    Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    _updateProjections();
}

void UnderStory::USApplication::drawEvent() {
    Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);

    // advance animations
    _mmh.advance();
    _msh.advance();
    _kmh.advance();

    // world
    _textShader.bindVectorTexture(_cache.texture());
    _textShader
        .setTransformationProjectionMatrix(_projectionWorld * _transformationWorld)
        .setColor(0x2f83cc_rgbf)
        .setOutlineColor(0xdcdcdc_rgbf)
        .setOutlineRange(0.45f, 0.35f)
        .setSmoothness(0.025f/ _transformationWorld.uniformScaling())
        .draw(_worldText);

    // grid
    _gridShader.bindTexture(_gridTexture);
    _gridShader
        .setProjectionMatrix(_projectionWorld * _transformationWorld)
        .setScale(_transformationWorld.uniformScaling())
        .draw(_grid);

    // selection rect
    if (_srs.isSelecting()) {
        _selectRectShader
            .setProjectionMatrix(_projectionWorld)
            .draw(_selectRect);
    }

    // shortcuts text
    _textShader
        .setTransformationProjectionMatrix(_transformationProjectionShortcutsText)
        .setColor(0xffffff_rgbf)
        .setOutlineRange(0.5f, 1.0f)
        .setSmoothness(0.075f)
        .draw(_shortcutsText->mesh());

    // debug text
    _updateDebugText();
    _textShader
        .setTransformationProjectionMatrix(_transformationProjectionDebugText)
        .setColor(0xffffff_rgbf)
        .setOutlineRange(0.5f, 1.0f)
        .setSmoothness(0.075f)
        .draw(_debugText->mesh());

    // buffers swap, automatic redrawing and timeline frame check
    swapBuffers();
    redraw();
    _timeline.nextFrame();
}

void UnderStory::USApplication::mouseScrollEvent(MouseScrollEvent& event) {
    _msh.mouseScrollEvent(event, this);
}

void UnderStory::USApplication::keyReleaseEvent(KeyEvent& event) {
    _kmh.keyReleaseEvent(event);
}

void UnderStory::USApplication::keyPressEvent(KeyEvent& event) {
    _kmh.keyPressEvent(event);
    _msh.keyPressEvent(event);

    switch (event.key()) {
    case KeyEvent::Key::Esc:
            _resetWorldMatrix();
        break;
    default:
        break;
    }
}

void UnderStory::USApplication::mousePressEvent(MouseEvent& event) {
    //
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            //
            _lMousePressed = true;

            //
            auto now = std::chrono::system_clock::now();
            auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lMousePressedT);
            _lMousePressedT = now;
            _lMousePressedMoved = false;

            //
            _mmh.mousePressEvent();

            //
            if (elapsedMs.count() < DOUBLE_CLICK_DELAY_MS)
                _mmh.mouseDoubleClickEvent(event, this);
        }
        break;

        case MouseEvent::Button::Right: {
            _rMousePressed = true;
            _srs.init(event, this);
        }
        break;

        default:
        break;
    }
}

void UnderStory::USApplication::mouseMoveEvent(MouseMoveEvent& event) {
    if (_lMousePressed) {
        //
        this->setCursor(Cursor::ResizeAll);

        //
        _lMousePressedMoved = true;
        _mmh.mouseMoveEvent(event, this);

    } else if (_srs.isSelecting()) {
        _srs.update(event, this);
    }
}

void UnderStory::USApplication::mouseReleaseEvent(MouseEvent& event) {
    //
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            //
            this->setCursor(Cursor::Arrow);

            //
            _lMousePressed = false;
            if (_lMousePressedMoved) _mmh.mouseMoveReleaseEvent();
        }
        break;

        case MouseEvent::Button::Right: {
            if (_srs.isSelecting()) {
                _srs.end(event, this);
                _msh.snapshotZoom(_srs.asRectangle());
            }

            //
            _rMousePressed = false;
        }
        break;

        default:
        break;
    }
}

void UnderStory::USApplication::_resetWorldMatrix() {
    _mmh.stopAnim();
    _msh.stopAnim();
    _kmh.stopAnim();
    _transformationWorld = Magnum::Matrix3();
}

void UnderStory::USApplication::_updateDebugText() {
    auto tr = _transformationWorld.translation();

    auto formatedText = Magnum::Utility::formatString(
        "{:.2}\n{:.2}\nZoom: x{:.2}",
        tr[0] / _transformationWorld.uniformScaling(),
        tr[1] / _transformationWorld.uniformScaling(),
        _transformationWorld.uniformScaling()
    );

    _debugText->render(formatedText);
}

MAGNUM_APPLICATION_MAIN(UnderStory::USApplication)
