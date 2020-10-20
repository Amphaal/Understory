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

#include "USApplication.h"

using namespace Magnum::Math::Literals;

UnderStory::USApplication::USApplication(const Arguments& arguments): Magnum::Platform::Application{
        arguments,
        Configuration{}.setTitle(APP_FULL_DENOM)
                       .setSize(Magnum::Vector2i{MINIMUM_WIDTH, MINIMUM_HEIGHT})
                       .setWindowFlags(Configuration::WindowFlag::Resizable),
        GLConfiguration{}
    },
    _cache{Magnum::Vector2i{2048}, Magnum::Vector2i{512}, 22},
    _rs("data"),
    _mmh(&_timeline, &_transformationWorld),
    _msh(&_timeline, &_transformationWorld),
    _kmh(&_timeline, &_transformationWorld),
    _sth(&_timeline, &_scaleMatrixShortcutsText),
    _atomSelector(&_timeline, &_flatShader),
    _selectionRect(_rs),
    _grid(_rs, MAP_SIZE, MINIMUM_HEIGHT) {
    // set minimum size
    SDL_SetWindowMinimumSize(this->window(), MINIMUM_WIDTH, MINIMUM_HEIGHT);

    //
    _updateChecker.start();

    // define animation mutual exclusions
    _kmh.setExcludedWhenPlaying({&_mmh});
    _mmh.setExcludedWhenPlaying({&_kmh});

    // activate VSync
    setSwapInterval(-1);

    // define
    _defineHaulder();

    /* Load a TrueTypeFont plugin and open the font */
    _font = _fontManager.loadAndInstantiate("TrueTypeFont");
    if(!_font || !_font->openData(_rs.getRaw("SourceSansPro-Regular.ttf"), 180.0f))
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
    _debugText.reset(new Magnum::Text::Renderer2D(*_font, _cache, 14.0f, Magnum::Text::Alignment::TopLeft));
    _debugText->reserve(100, Magnum::GL::BufferUsage::DynamicDraw, Magnum::GL::BufferUsage::StaticDraw);

    // define shortcut text as static
    const std::string sText =
                "[LeftClick  (DoubleClick)] Center screen on cursor\n"
                "[LeftClick  (Maintain)]                       Move camera\n"
                "[RightClick (Maintain / Release)]             Snapshot zoom-in\n"
                "[D-Pad]                       Move camera\n"
                "[Esc]                       Reset camera\n"
                "[MouseScroll]                                       Zoom\n"
                "[NumPad +/-]                                       Zoom";
    _shortcutsText.reset(new Magnum::Text::Renderer2D(*_font, _cache, 14.0f, Magnum::Text::Alignment::TopRight));
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

void UnderStory::USApplication::_defineHaulder() {
    // define indices
    Magnum::GL::Buffer indices;
    indices.setData({
        0, 1, 2,
        0, 2, 3
    });

    // define vertices
    struct HaulderVertex {
        Magnum::Vector2 position;
    };
    const HaulderVertex vertex[]{
        {{-1.f, -1.f}},
        {{ 1.f, -1.f}},
        {{ 1.f,  1.f}},
        {{-1.f,  1.f}}
    };

    // bind buffer
    _haulderBuffer.setData(vertex, Magnum::GL::BufferUsage::StaticDraw);

    // define mesh
    _haulder.setCount(indices.size())
            .setIndexBuffer (std::move(indices),        0, Magnum::MeshIndexType::UnsignedInt)
            .addVertexBuffer(std::move(_haulderBuffer), 0, Magnum::Shaders::Flat2D::Position{});
}

void UnderStory::USApplication::_updateProjections() {
    //
    Widget::Helper wh {windowSize()};

    //
    _projectionWorld = Magnum::Matrix3::projection(
        Magnum::Vector2::xScale(wh.ws.aspectRatio())
    );

    // stick to top left corner + 5 pixels padding
    _transformationProjectionDebugText = wh.baseProjMatrix *
        Magnum::Matrix3::translation(
            wh.ws *
            (Magnum::Vector2 {-.5f, .5f} - wh.pixelSize * 5 * Magnum::Vector2{-1.f, 1.f})
        );

    // stick to top right corner + 5 pixels padding
    _transformationProjectionShortcutsText = wh.baseProjMatrix *
        Magnum::Matrix3::translation(
            wh.ws *
            (Magnum::Vector2 {.5f} - wh.pixelSize * 5)
        );

    // TODO(amphaal) include padding in rect calculation
    _shortcutsTextRect = Navigation::ShortcutsTextHelper::trNormalized(
        _shortcutsText->rectangle(),
        this->framebufferSize()
    );

    //
    _atomSelector.onViewportChange(wh);
    _selectionRect.onViewportChange();
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
    _sth.advance();
    _atomSelector.advance();

    // world
    auto worldMatrix = _projectionWorld * _transformationWorld;
    auto scale = _transformationWorld.uniformScaling();
    _textShader.bindVectorTexture(_cache.texture());
    _textShader
        .setTransformationProjectionMatrix(worldMatrix)
        .setColor(0x2f83cc_rgbf)
        .setOutlineColor(0xdcdcdc_rgbf)
        .setOutlineRange(0.45f, 0.35f)
        .setSmoothness(0.015f/ scale)
        .draw(_worldText);

    // grid
    _grid.draw(worldMatrix, scale);

    // selection rect
    _selectionRect.mayDraw(_projectionWorld);

    // debug text
    _updateDebugText();
    _textShader
        .setColor(DEBUG_TEXT_COLOR)
        .setOutlineColor(DEBUG_TEXT_COLOR)
        .setOutlineRange(0.47f, 0.44f)
        .setSmoothness(0.33f)
        .setTransformationProjectionMatrix(_transformationProjectionDebugText)
        .draw(_debugText->mesh());

    // atom selector
    _atomSelector.draw();

    // haulder
    _flatShader
        .setTransformationProjectionMatrix({})
        .setColor(_sth.haulderColor())
        .draw(_haulder);

    // shortcuts text
    _textShader
        .setColor(_sth.textColor())
        .setOutlineColor(0xffffffAA_rgbaf)
        .setTransformationProjectionMatrix(_transformationProjectionShortcutsText * _scaleMatrixShortcutsText)
        .draw(_shortcutsText->mesh());

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
            _selectionRect.init(event, this);
        }
        break;

        default:
        break;
    }
}

Magnum::Vector2 UnderStory::USApplication::_cursorPosition(const Sdl2Application::MouseMoveEvent& event) const {
    Magnum::Vector2 fs{this->framebufferSize()};
    return (Magnum::Vector2{event.position()} - fs / 2.f) / fs * Magnum::Vector2{2.f, -2.f};
}

void UnderStory::USApplication::mouseMoveEvent(MouseMoveEvent& event) {
    if (_lMousePressed) {
        // TODO(amphaal) check context
        this->setCursor(Cursor::ResizeAll);

        //
        _lMousePressedMoved = true;
        _mmh.mouseMoveEvent(event, this);

    } else if (_selectionRect.isSelecting()) {
        // if selection, update rect
        _selectionRect.update(event, this);

    } else {
        // test hovering
        _sth.mouseMoveEvent(event, this, _shortcutsTextRect);
        //
        auto cursorPos = _cursorPosition(event);
        _atomSelector.onMouseMove(cursorPos, this);
    }
}

void UnderStory::USApplication::mouseReleaseEvent(MouseEvent& event) {
    //
    switch (event.button()) {
        case MouseEvent::Button::Left: {
            // TODO(amphaal) check context
            this->setCursor(Cursor::Arrow);

            //
            _lMousePressed = false;
            if (_lMousePressedMoved) _mmh.mouseMoveReleaseEvent();
            _atomSelector.onMouseClick();
        }
        break;

        case MouseEvent::Button::Right: {
            if (_selectionRect.isSelecting()) {
                _selectionRect.end(event, this);
                _msh.snapshotZoom(_selectionRect.asRectangle());
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
        "x: {:.2}\ny: {:.2}\nZoom: x{:.2}",
        tr[0] / _transformationWorld.uniformScaling(),
        tr[1] / _transformationWorld.uniformScaling(),
        _transformationWorld.uniformScaling()
    );

    _debugText->render(formatedText);
}

MAGNUM_APPLICATION_MAIN(UnderStory::USApplication)
